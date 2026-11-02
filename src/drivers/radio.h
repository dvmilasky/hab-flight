#pragma once

#include <cstdint>
#include "pico/types.h"
#include "hardware/spi.h"


// How the radio is wired to the Pico. Board fills this
struct RadioConfig {
    spi_inst_t* spi;
    uint clock_pin;
    uint tx_pin;
    uint rx_pin;
    uint chip_select_pin;
    uint reset_pin;
};

enum class RadioStatus {
    Ok,
    NotInitialized
};
// Sets up SPI and the radio. Must be called before any other radio function.
// Returns the actual SPI baud rate.
[[nodiscard]] RadioStatus init_radio(const RadioConfig& config);

void reset_radio_state();


// Reads operation mode byte
RadioStatus read_radio_mode(uint8_t& mode_out);

// Reads radio configured frequency in Hz
RadioStatus read_radio_frequency(uint32_t& frequency_out);

// Transmits buffer of data (Blocking)
[[nodiscard]] RadioStatus transmit_data_blocking(const uint8_t *data, size_t len);
