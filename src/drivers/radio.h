#pragma once

#include <cstdint>
#include "pico/types.h"
#include "hardware/spi.h"

// 1MHz - keep some margin (10MHz max on radio's SX1276 chip)
constexpr uint RADIO_CLOCK_RATE = 1000 * 1000;
constexpr uint RADIO_WRITE_FLAG = 0x80;
constexpr uint RADIO_READ_MASK = 0x7F;

// Radio Registers

constexpr uint8_t VERSION_REGISTER = 0x42;
constexpr uint8_t OP_MODE_REGISTER = 0x01;
constexpr uint8_t FREQUENCY_REGISTER_MSB = 0x06;
constexpr uint8_t FREQUENCY_REGISTER_MID = 0x07;
constexpr uint8_t FREQUENCY_REGISTER_LSB = 0x08;

// Radio Settings
constexpr uint8_t SLEEP_MODE = 0x00;
constexpr uint8_t LORA_MODE = 0x80;
constexpr uint32_t FREQUENCY_SETTING = 14'991'360;

// How the radio is wired to the Pico. Board fills this
struct RadioConfig {
    spi_inst_t* spi;
    uint clock_pin;
    uint tx_pin;
    uint rx_pin;
    uint chip_select_pin;
    uint reset_pin;
};

// Sets up SPI and the radio. Must be called before any other radio function.
// Returns the actual SPI baud rate.
uint init_radio(const RadioConfig& config);

void reset_radio_state();

void write_byte_to_radio_register(uint8_t reg, uint8_t data);

uint8_t read_radio_byte_from_register(uint8_t reg);
