#pragma once

// Brings up stdio, the status LED, and all drivers. Call once at startup.
void board_init();

void status_led_toggle();
