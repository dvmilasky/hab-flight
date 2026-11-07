#pragma once

#include <cstdint>
#include "pico/types.h"
#include "hardware/spi.h"

constexpr size_t RADIO_MAX_PAYLOAD = 255;


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
    NotInitialized,
    EmptyPayload,
    NullData,
    PayloadTooLarge,
    NotConnected,
    TxTimeout,
    RxTimeout,
    PayloadCorrupted,
    NoPacket
};
// Sets up SPI and the radio. Must be called before any other radio function.
// Radio set to standby mode upon return
[[nodiscard]] RadioStatus init_radio(const RadioConfig& config);

// Reads operation mode byte
RadioStatus read_radio_mode(uint8_t& mode_out);

// Reads radio configured frequency in Hz
RadioStatus read_radio_frequency(uint32_t& frequency_out);

// Transmits buffer of data (Blocking)
[[nodiscard]] RadioStatus transmit_data_blocking(const uint8_t *data, size_t len);

[[nodiscard]] RadioStatus enable_rx_continuous_mode();
[[nodiscard]] RadioStatus poll_for_packet(uint8_t (&buffer)[RADIO_MAX_PAYLOAD], uint8_t &len_out);

const char* get_error_msg(RadioStatus status);