#include "drivers/gps.h"

#include "pico/stdlib.h"
#include "hardware/uart.h"

void init_gps_uart(const GpsConfig& config) {
    uart_init(config.uart, GPS_BAUD_RATE);
    gpio_set_function(config.tx_pin, GPIO_FUNC_UART);
    gpio_set_function(config.rx_pin, GPIO_FUNC_UART);
}
