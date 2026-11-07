#include <cstdio>
#include "pico/stdlib.h"
#include "board/radio-receiver/board.h"
#include "board/radio-receiver/pins.h"
#include "drivers/radio.h"

static void init_status_led() {
    gpio_init(STATUS_LED_PIN);
    gpio_set_dir(STATUS_LED_PIN, GPIO_OUT);
}

BoardStatus board_init() {
    stdio_init_all();
    init_status_led();

    const RadioConfig radio_config = {
        spi1,                   // spi
        SPI_CLOCK_PIN,          // clock_pin
        SPI_TX_PIN,             // tx_pin
        SPI_RX_PIN,             // rx_pin
        SPI_CHIP_SELECT_PIN,    // chip_select_pin
        SPI_RST_PIN,            // reset_pin
    };
    RadioStatus status = init_radio(radio_config);
    if (status != RadioStatus::Ok) {
        const char* err = get_error_msg(status);
        std::printf("Error initializing Radio: %s\n", err);
        return BoardStatus::FailedInitialization;
    }
    return BoardStatus::Ok;
}

void status_led_toggle() {
    gpio_put(STATUS_LED_PIN, !gpio_get(STATUS_LED_PIN));
}
