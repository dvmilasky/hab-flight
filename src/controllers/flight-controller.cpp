#include <cstdio>
#include "board/board.h"
#include "drivers/gps.h"
#include "drivers/radio.h"
#include "pico/stdlib.h"



int main() {
    board_init();

    while (true) {
        status_led_toggle();
        RadioStatus status;
        
        uint8_t mode;
        status = read_radio_mode(mode);
        assert(status == RadioStatus::Ok);
        std::printf("Operating Mode: 0x%02X\n", mode);

        uint32_t freq;
        status = read_radio_frequency(freq);
        assert(status == RadioStatus::Ok);
        std::printf("Radio Frequency Initialized to %luhz\n", freq);

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