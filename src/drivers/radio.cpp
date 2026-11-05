// TODO: Abstract the Pico SDK calls out into an interface?

#include <cstdio>
#include <cinttypes>
#include "drivers/radio.h"
#include "pico/stdlib.h"
#include "hardware/spi.h"

constexpr size_t RADIO_MAX_PAYLOAD = 255;

// 1MHz - keep some margin (10MHz max on radio's SX1276 chip)
constexpr uint32_t RADIO_CLOCK_RATE{1000 * 1000};
constexpr uint8_t RADIO_WRITE_FLAG = 0x80;
constexpr uint8_t RADIO_READ_MASK = 0x7F;
constexpr uint64_t RADIO_TX_TIMEOUT_US = 3'000'000; // 3 seconds

// Radio Settings
constexpr uint8_t RADIO_VERSION = 0x12;
constexpr uint8_t VERSION_REGISTER = 0x42;

constexpr uint8_t OP_MODE_REGISTER = 0x01;

constexpr uint8_t MODE_MASK = 0x07;
constexpr uint8_t MODE_SLEEP = 0x00;
constexpr uint8_t MODE_STANDBY = 0x01;
constexpr uint8_t MODE_TX = 0x03;

constexpr uint8_t LORA_MODE_FLAG = 0x80;

constexpr uint8_t FREQUENCY_REGISTER_MSB = 0x06;
constexpr uint8_t FREQUENCY_REGISTER_MID = 0x07;
constexpr uint8_t FREQUENCY_REGISTER_LSB = 0x08;
constexpr uint32_t FREQUENCY_FROM_HZ(uint64_t hz) {
    return (1 << 19) * hz / 32'000'000; // Frf = f_RF * 2^19 / F_XOSC (F_XOSC/2^19 is lowest freq synethesizer step (61hz))
}
constexpr uint32_t FREQUENCY_SETTING = FREQUENCY_FROM_HZ(915'000'000); // 915 MHz

constexpr uint64_t HZ_FROM_REGISTER(uint64_t reg) {
    return reg * 32'000'000 / (1 << 19); // Frf = f_RF * 2^19 / F_XOSC (F_XOSC/2^19 is lowest freq synethesizer step (61hz))
}

constexpr uint8_t PA_CONFIG_REGISTER = 0x09;

constexpr uint8_t PA_CONFIG_SELECT_BOOST = 0x80;
constexpr uint8_t PA_CONFIG_POWER_MASK = 0x0F;
constexpr uint8_t PA_CONFIG_POWER_MIN = 0x00; // 2 dBm (Bench testing)
constexpr uint8_t PA_CONFIG_POWER_MAX = 0x0F; // 17 dBm

// FIFO Register Values
constexpr uint8_t FIFO_REGISTER = 0x00;

constexpr uint8_t FIFO_TX_BASE_ADDR_REGISTER = 0x0E;
constexpr uint8_t FIFO_RX_BASE_ADDR_REGISTER = 0x0F;
constexpr uint8_t FIFO_BASE_ADDR_MIN = 0x00;

constexpr uint8_t FIFO_ADDR_PTR_REGISTER = 0x0D;

// Payload length
constexpr uint8_t PAYLOAD_LENGTH_REGISTER = 0x22;

// IRQ Registers
constexpr uint8_t IRQ_FLAGS_REGISTER = 0x12;

constexpr uint8_t IRQ_TX_BIT = 0x08;


// Wiring saved by init_radio() for use by the other radio functions.
namespace {
    struct RadioState {
        RadioConfig config{};
        bool initialized = false;
    };
    RadioState g_radio;

    void reset_radio_state() {
        gpio_put(g_radio.config.reset_pin, 0);
        sleep_ms(1);
        gpio_put(g_radio.config.reset_pin, 1);
        sleep_ms(10);
    }

    void write_buffer_to_register(uint8_t reg, const uint8_t *data, size_t size) {
        // Set chip select low to start SPI convo
        gpio_put(g_radio.config.chip_select_pin, 0);
        
        // Set write flag of register
        reg |= RADIO_WRITE_FLAG;
        spi_write_blocking(g_radio.config.spi, &reg, sizeof(reg));
        spi_write_blocking(g_radio.config.spi, data, size);

        // Set chip select back to high
        gpio_put(g_radio.config.chip_select_pin, 1);
    }

    void write_byte_to_radio_register(uint8_t reg, uint8_t data) {
        // Set chip select low to start SPI convo
        gpio_put(g_radio.config.chip_select_pin, 0);

        reg |= RADIO_WRITE_FLAG;
        spi_write_blocking(g_radio.config.spi, &reg, sizeof(reg));
        spi_write_blocking(g_radio.config.spi, &data, sizeof(data));

        // Set chip select back to high
        gpio_put(g_radio.config.chip_select_pin, 1);
    }

    uint8_t read_radio_byte_from_register(uint8_t reg) {
        // Set chip select low to start SPI convo
        gpio_put(g_radio.config.chip_select_pin, 0);

        reg &= RADIO_READ_MASK;
        spi_write_blocking(g_radio.config.spi, &reg, sizeof(reg));

        uint8_t read_byte;
        spi_read_blocking(g_radio.config.spi, 0x00, &read_byte, sizeof(read_byte));

        // Set chip select back to high
        gpio_put(g_radio.config.chip_select_pin, 1);

        return read_byte;
    }

    void set_mode(uint8_t mode) {
        mode &= MODE_MASK; // Ensure we only touch the mode bits
        uint8_t op_mode_byte = read_radio_byte_from_register(OP_MODE_REGISTER);
        op_mode_byte &= ~MODE_MASK;
        write_byte_to_radio_register(OP_MODE_REGISTER, mode | op_mode_byte);
    }
}


// Initializes the Radio and sets to standby mode
[[nodiscard]] RadioStatus init_radio(const RadioConfig& config) {
    g_radio.config = config;

    spi_init(g_radio.config.spi, RADIO_CLOCK_RATE);
    gpio_set_function(g_radio.config.clock_pin, GPIO_FUNC_SPI);
    gpio_set_function(g_radio.config.tx_pin, GPIO_FUNC_SPI);
    gpio_set_function(g_radio.config.rx_pin, GPIO_FUNC_SPI);

    gpio_init(g_radio.config.chip_select_pin);
    gpio_init(g_radio.config.reset_pin);
    // Set direction to output
    gpio_set_dir(g_radio.config.chip_select_pin, GPIO_OUT);
    // Initially set chip select high
    gpio_put(g_radio.config.chip_select_pin, 1);

    // Clear radio state
    gpio_set_dir(g_radio.config.reset_pin, GPIO_OUT);
    reset_radio_state();

    // Verify Radio connectivity by reading version
    if (read_radio_byte_from_register(VERSION_REGISTER) != RADIO_VERSION) {
        return RadioStatus::NotConnected;
    }

    // Set radio to sleep mode and mode to LORA
    write_byte_to_radio_register(OP_MODE_REGISTER, MODE_SLEEP);
    write_byte_to_radio_register(OP_MODE_REGISTER, MODE_SLEEP | LORA_MODE_FLAG);

    // Set frequency to 915MHz
    write_byte_to_radio_register(FREQUENCY_REGISTER_MSB, (FREQUENCY_SETTING >> 16) & 0xFF);
    write_byte_to_radio_register(FREQUENCY_REGISTER_MID, (FREQUENCY_SETTING >> 8) & 0xFF);
    write_byte_to_radio_register(FREQUENCY_REGISTER_LSB, FREQUENCY_SETTING & 0xFF);

    // Set output pin to PA_BOOST
    write_byte_to_radio_register(PA_CONFIG_REGISTER, PA_CONFIG_SELECT_BOOST | PA_CONFIG_POWER_MIN);

    // Set radio to standby mode
    write_byte_to_radio_register(OP_MODE_REGISTER, LORA_MODE_FLAG | MODE_STANDBY);

    // Set TX/RX base addresses to 0x00 (each radio uses entirety of FIFO cache for TX/RX respectively)
    write_byte_to_radio_register(FIFO_TX_BASE_ADDR_REGISTER, FIFO_BASE_ADDR_MIN);
    write_byte_to_radio_register(FIFO_RX_BASE_ADDR_REGISTER, FIFO_BASE_ADDR_MIN);

    g_radio.initialized = true;
    return RadioStatus::Ok;
}

RadioStatus read_radio_mode(uint8_t& mode_out) {
    if (!g_radio.initialized) {
        return RadioStatus::NotInitialized;
    }

    mode_out = read_radio_byte_from_register(OP_MODE_REGISTER);
    return RadioStatus::Ok;
}

RadioStatus read_radio_frequency(uint32_t& frequency_out) {
    if (!g_radio.initialized) {
        return RadioStatus::NotInitialized;
    }

    uint8_t lsb_frq = read_radio_byte_from_register(FREQUENCY_REGISTER_LSB);
    uint8_t mid_frq = read_radio_byte_from_register(FREQUENCY_REGISTER_MID);
    uint8_t msb_frq = read_radio_byte_from_register(FREQUENCY_REGISTER_MSB);

    uint64_t reg_value = (static_cast<uint64_t>(msb_frq) << 16) | (mid_frq << 8) | lsb_frq;

    std::printf("MSB: 0x%02X MID: 0x%02X LSB: 0x%02X\n", msb_frq, mid_frq, lsb_frq);

    frequency_out = HZ_FROM_REGISTER(reg_value);

    return RadioStatus::Ok;
}

[[nodiscard]] RadioStatus transmit_data_blocking(const uint8_t *data, size_t len){
    if (!g_radio.initialized) {
        return RadioStatus::NotInitialized;
    }
    if (data == nullptr) {
        return RadioStatus::NullData;
    }
    if (len == 0) {
        return RadioStatus::EmptyPayload;
    }
    if (len > RADIO_MAX_PAYLOAD) {
        return RadioStatus::PayloadTooLarge;
    }

    // Ensure radio is in standby mode before FIFO write
    set_mode(MODE_STANDBY);

    // Set FIFO address pointer to the base addr min (0x00)
    write_byte_to_radio_register(FIFO_ADDR_PTR_REGISTER, FIFO_BASE_ADDR_MIN);

    // Write data to FIFO register
    write_buffer_to_register(FIFO_REGISTER, data, len);
    // Write payload length
    write_byte_to_radio_register(PAYLOAD_LENGTH_REGISTER, static_cast<uint8_t>(len));

    // Set radio to transmit mode
    set_mode(MODE_TX);

    uint64_t start = time_us_64();
    // Wait for IRQ TX bit to go high (TX done) TODO: Use interrupts and set IRQ Mask register to avoid interrupts for other events
    while (!(read_radio_byte_from_register(IRQ_FLAGS_REGISTER) & IRQ_TX_BIT)) {
        // Wait for TX to be done
        sleep_us(100);
        if (time_us_64() - start > RADIO_TX_TIMEOUT_US) {
            set_mode(MODE_STANDBY); // Leave radio in standby state on failure
            return RadioStatus::TxTimeout;
        }
    }

    #ifndef NDEBUG
        std::printf("Radio TX took %" PRIu64 "ms\n", (time_us_64() - start) / 1000);
    #endif
    write_byte_to_radio_register(IRQ_FLAGS_REGISTER, IRQ_TX_BIT);

    return RadioStatus::Ok;
}
