#include "drivers/radio.h"

#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/spi.h"

// Wiring saved by init_radio() for use by the other radio functions.
static RadioConfig radio;

void reset_radio_state() {
    gpio_put(radio.reset_pin, 0);
    sleep_ms(1);
    gpio_put(radio.reset_pin, 1);
    sleep_ms(10);
}

void write_byte_to_radio_register(uint8_t reg, uint8_t data) {
    // Set chip select low to start SPI convo
    gpio_put(radio.chip_select_pin, 0);

    reg |= RADIO_WRITE_FLAG;
    spi_write_blocking(radio.spi, &reg, sizeof(reg));
    spi_write_blocking(radio.spi, &data, sizeof(data));

    // Set chip select back to high
    gpio_put(radio.chip_select_pin, 1);
}

uint8_t read_radio_byte_from_register(uint8_t reg) {
    // Set chip select low to start SPI convo
    gpio_put(radio.chip_select_pin, 0);

    reg &= RADIO_READ_MASK;
    spi_write_blocking(radio.spi, &reg, sizeof(reg));

    uint8_t read_byte;
    spi_read_blocking(radio.spi, 0x00, &read_byte, sizeof(read_byte));

    // Set chip select back to high
    gpio_put(radio.chip_select_pin, 1);

    return read_byte;
}

uint init_radio(const RadioConfig& config) {
    radio = config;

    uint baud = spi_init(radio.spi, RADIO_CLOCK_RATE);
    gpio_set_function(radio.clock_pin, GPIO_FUNC_SPI);
    gpio_set_function(radio.tx_pin, GPIO_FUNC_SPI);
    gpio_set_function(radio.rx_pin, GPIO_FUNC_SPI);

    gpio_init(radio.chip_select_pin);
    gpio_init(radio.reset_pin);
    // Set direction to output
    gpio_set_dir(radio.chip_select_pin, GPIO_OUT);
    // Initially set chip select high
    gpio_put(radio.chip_select_pin, 1);

    // Clear radio state
    gpio_set_dir(radio.reset_pin, GPIO_OUT);
    reset_radio_state();

    // Set radio state to LORA
    write_byte_to_radio_register(OP_MODE_REGISTER, SLEEP_MODE);
    write_byte_to_radio_register(OP_MODE_REGISTER, SLEEP_MODE | LORA_MODE);
    std::printf("Radio Mode Initialized to 0x%02X\n", read_radio_byte_from_register(OP_MODE_REGISTER));

    // Set frequency to 915MHz
    write_byte_to_radio_register(FREQUENCY_REGISTER_MSB, (FREQUENCY_SETTING >> 16) & 0xFF);
    write_byte_to_radio_register(FREQUENCY_REGISTER_MID, (FREQUENCY_SETTING >> 8) & 0xFF);
    write_byte_to_radio_register(FREQUENCY_REGISTER_LSB, FREQUENCY_SETTING & 0xFF);

    std::printf("Radio Frequency Initialized to 0x%02X,  0x%02X,  0x%02X\n", read_radio_byte_from_register(FREQUENCY_REGISTER_MSB), \
        read_radio_byte_from_register(FREQUENCY_REGISTER_MID), \
        read_radio_byte_from_register(FREQUENCY_REGISTER_LSB));

    return baud;
}
