#pragma once

#include "pico.h"
#include "pico/types.h"

// Pin assignments for the PICO controller.

// Status LED
constexpr uint STATUS_LED_PIN = PICO_DEFAULT_LED_PIN;

// SX1276 radio (SPI1)
constexpr uint SPI_CLOCK_PIN = 10;
constexpr uint SPI_TX_PIN = 11;
constexpr uint SPI_RX_PIN = 12;
constexpr uint SPI_CHIP_SELECT_PIN = 13;
constexpr uint SPI_RST_PIN = 14;

// GPS (UART0)
constexpr uint UART_TX_PIN = 16;
constexpr uint UART_RX_PIN = 17;
