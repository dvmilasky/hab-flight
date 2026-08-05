#include <cstdio>
#include "pico/stdlib.h"
#include "hardware/uart.h"


#define BAUD_RATE 9600
#define UART_TX_PIN 16
#define UART_RX_PIN 17



int main() {
    stdio_init_all();


    uart_init(uart0, BAUD_RATE);

    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

    while (true) {
        if (uart_is_readable(uart0)) {
            char in_byte = uart_getc(uart0);
            std::printf("GPS Line: %c\n", in_byte);
        }
    }
}