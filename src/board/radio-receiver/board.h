#pragma once

enum class BoardStatus {
    Ok,
    FailedInitialization
};

// Brings up stdio, the status LED, and all drivers. Call once at startup.
BoardStatus board_init();

void status_led_toggle();
