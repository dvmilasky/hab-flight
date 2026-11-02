#pragma once
#include "pico/types.h"
#include "hardware/uart.h"


// GPS Comms
constexpr uint GPS_BAUD_RATE = 9600;
constexpr uint BUFFER_SIZE = 85;

// How the GPS is wired to the Pico. The board fills this.
struct GpsConfig {
    uart_inst_t* uart;
    uint tx_pin;
    uint rx_pin;
};

// Initializes the UART the GPS is connected to
void init_gps_uart(const GpsConfig& config);
