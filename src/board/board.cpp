#include "board/board.h"

#include <cstdio>
#include "pico/stdlib.h"
#include "board/pins.h"
#include "drivers/gps.h"
#include "drivers/radio.h"

static void init_status_led() {
    gpio_init(STATUS_LED_PIN);
    gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);
}

void board_init() {
    stdio_init_all();
    init_status_led();

    const GpsConfig gps_config = {
        uart0,          // uart
        UART_TX_PIN,    // tx_pin
        UART_RX_PIN,    // rx_pin
    };
    init_gps_uart(gps_config);

    const RadioConfig radio_config = {
        spi1,                   // spi
        SPI_CLOCK_PIN,          // clock_pin
        SPI_TX_PIN,             // tx_pin
        SPI_RX_PIN,             // rx_pin
        SPI_CHIP_SELECT_PIN,    // chip_select_pin
        SPI_RST_PIN,            // reset_pin
    };
    uint actual_baud = init_radio(radio_config);
    std::printf("Radio Baud Set: %u\n", actual_baud);
}

void status_led_toggle() {
    gpio_put(STATUS_LED_PIN, !gpio_get(STATUS_LED_PIN));
}
