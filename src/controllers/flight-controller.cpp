#include <cstdio>
#include "board/board.h"
#include "drivers/gps.h"
#include "drivers/radio.h"
#include "pico/stdlib.h"

int main() {
    board_init();

    while (true) {
        status_led_toggle();
        uint8_t op_mode;

        op_mode = read_radio_byte_from_register(OP_MODE_REGISTER);
        std::printf("Operating Mode: 0x%02X\n", op_mode);

        std::printf("Radio Frequency Initialized to 0x%02X,  0x%02X,  0x%02X\n", read_radio_byte_from_register(FREQUENCY_REGISTER_MSB), \
        read_radio_byte_from_register(FREQUENCY_REGISTER_MID), \
        read_radio_byte_from_register(FREQUENCY_REGISTER_LSB));

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