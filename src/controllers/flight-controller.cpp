#include <cstdio>
#include <iterator>
#include "board/flight-controller/board.h"
#include "drivers/gps.h"
#include "drivers/radio.h"
#include "pico/stdlib.h"



int main() {
    board_init();

    while (true) {
        status_led_toggle();
        RadioStatus status;

        uint32_t freq;
        status = read_radio_frequency(freq);
        assert(status == RadioStatus::Ok);
        std::printf("Radio Frequency Initialized to %luhz\n", freq);

        const uint8_t data[] = {0x23, 0x53, 0x43, 0x45};

        status = transmit_data_blocking(data, std::size(data));
        assert(status == RadioStatus::Ok);

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