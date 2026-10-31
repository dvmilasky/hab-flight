#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "hardware/spi.h"



// GPS Comms
constexpr uint GPS_BAUD_RATE = 9600;
constexpr uint UART_TX_PIN = 16;
constexpr uint UART_RX_PIN = 17;
constexpr uint BUFFER_SIZE = 85;

// Radio Comms

// 1MHz - keep some margin (10MHz max on radio's SX1276 chip)
constexpr uint RADIO_CLOCK_RATE = 1000 * 1000;
constexpr uint RADIO_WRITE_FLAG = 0x80;
constexpr uint RADIO_READ_MASK = 0x7F;

constexpr uint SPI_CLOCK_PIN = 10;
constexpr uint SPI_TX_PIN = 11;
constexpr uint SPI_RX_PIN = 12;
constexpr uint SPI_CHIP_SELECT_PIN = 13;
constexpr uint SPI_RST_PIN = 14;


// Radio Registers

constexpr uint8_t VERSION_REGISTER = 0x42;
constexpr uint8_t RX_DELAY_REGISTER = 0x23;

void init_gps_uart() {
    uart_init(uart0, GPS_BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}

void reset_radio_state() {
    gpio_put(SPI_RST_PIN, 0);
    sleep_ms(1);
    gpio_put(SPI_RST_PIN, 1);
    sleep_ms(10);
}

uint init_radio() {
    gpio_set_function(SPI_CLOCK_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_TX_PIN, GPIO_FUNC_SPI);
    gpio_set_function(SPI_RX_PIN, GPIO_FUNC_SPI);
    
    gpio_init(SPI_CHIP_SELECT_PIN);
    gpio_init(SPI_RST_PIN);
    // Set direction to output
    gpio_set_dir(SPI_CHIP_SELECT_PIN, GPIO_OUT);
    // Initially set chip select high
    gpio_put(SPI_CHIP_SELECT_PIN, 1);

    const uint baud = spi_init(spi1,  RADIO_CLOCK_RATE);

    // Clear radio state
    gpio_set_dir(SPI_RST_PIN, GPIO_OUT);
    reset_radio_state();
    return baud;
}


void write_byte_to_radio_register(uint8_t reg, uint8_t data) {
    // Set chip select low to start SPI convo
    gpio_put(SPI_CHIP_SELECT_PIN, 0);

    reg |= RADIO_WRITE_FLAG;
    spi_write_blocking(spi1, &reg, sizeof(reg));
    spi_write_blocking(spi1, &data, sizeof(data));

    // Set chip select back to high
    gpio_put(SPI_CHIP_SELECT_PIN, 1);
}

uint8_t read_radio_byte_from_register(uint8_t reg) {
    // Set chip select low to start SPI convo
    gpio_put(SPI_CHIP_SELECT_PIN, 0);

    reg &= RADIO_READ_MASK;
    spi_write_blocking(spi1, &reg, sizeof(reg));

    uint8_t read_byte;
    spi_read_blocking(spi1, 0x00, &read_byte, sizeof(read_byte));

    // Set chip select back to high
    gpio_put(SPI_CHIP_SELECT_PIN, 1);

    return read_byte;
}

int main() {
    stdio_init_all();
    init_gps_uart();
    const uint actual_baud = init_radio();
    std::printf("Radio Baud Set: %u\n", actual_baud);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
        reset_radio_state();
        uint8_t rx_delay;

        rx_delay = read_radio_byte_from_register(RX_DELAY_REGISTER);
        std::printf("Radio Delay (Before Set): 0x%02X\n", rx_delay);

        write_byte_to_radio_register(RX_DELAY_REGISTER, 0xA5);
        rx_delay = read_radio_byte_from_register(RX_DELAY_REGISTER);
        std::printf("Radio Delay (After Set): 0x%02X\n", rx_delay);

        sleep_ms(5000);
    }

    // char gps_data_buffer[85];
    // uint8_t i = 0;
    // while (true) {
    //     if (uart_is_readable(uart0)) {
    //         char in_byte = uart_getc(uart0);
    //         if (i < BUFFER_SIZE - 1) {
    //             if (in_byte == '\n') {
    //                 gps_data_buffer[i] = '\0';
    //                 std::printf("GPS Line: %s\n", gps_data_buffer);
    //                 i = 0;
    //             } else {
    //                 gps_data_buffer[i] = in_byte;
    //                 i++;
    //             }
    //         } else {
    //             gps_data_buffer[i] = '\0';
    //             std::printf("Missing termination byte. Throwing line away: %s", gps_data_buffer);
    //             i = 0;
    //         }
    //     }
    // }
}