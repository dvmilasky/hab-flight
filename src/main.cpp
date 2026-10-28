#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/uart.h"


#define BAUD_RATE 9600
#define UART_TX_PIN 16
#define UART_RX_PIN 17
#define BUFFER_SIZE 85



int main() {
    stdio_init_all();


    uart_init(uart0, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    char databuffer[85];
    uint8_t i = 0;
    while (true) {
        if (uart_is_readable(uart0)) {
            char in_byte = uart_getc(uart0);
            if (i < BUFFER_SIZE - 1) {
                if (in_byte == '\n') {
                    databuffer[i] = '\0';
                    std::printf("GPS Line: %s\n", databuffer);
                    i = 0;
                } else {
                    databuffer[i] = in_byte;
                    i++;
                }
            } else {
                databuffer[i] = '\0';
                std::printf("Missing termination byte. Throwing line away: %s", databuffer);
                i = 0;
            }
        }
    }
}