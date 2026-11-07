#include <cstdio>
#include <iterator>
#include "board/radio-receiver/board.h"
#include "drivers/gps.h"
#include "drivers/radio.h"
#include "pico/stdlib.h"



int main() {
    while (board_init() != BoardStatus::Ok) {
        sleep_ms(10000);
    }

    RadioStatus status;
    status = enable_rx_continuous_mode();
    if (status != RadioStatus::Ok) {
        std::printf("Radio Error during setting continuous RX mode: %s\n", get_error_msg(status));
    }

    while (true) {
        status_led_toggle();

        uint32_t freq;
        status = read_radio_frequency(freq);
        if (status != RadioStatus::Ok) {
            std::printf("Radio Error during read Freq: %s\n", get_error_msg(status));
        }
        std::printf("Radio Frequency Initialized to %luhz\n", freq);

        uint8_t data[RADIO_MAX_PAYLOAD];
        uint8_t output_packet_length;

        uint8_t mode;
        read_radio_mode(mode);
        std::printf("Radio RX Mode: 0x%2X\n", mode);
        status = poll_for_packet(data, output_packet_length);
        if (status == RadioStatus::Ok) {
            for (int i = 0; i < output_packet_length; i++) {
                std::printf("Received Byte: 0x%02X\n", data[i]);
            }
        } else {
            std::printf("Radio Error during polling for packet: %s\n", get_error_msg(status));
        }
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