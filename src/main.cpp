#include "pico/stdlib.h"

#define BLINK_INTERVAL 100

int main() {
    gpio_init(PICO_DEFAULT_LED_PIN);

    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (true) {
        gpio_put(PICO_DEFAULT_LED_PIN, 1);
        sleep_ms(BLINK_INTERVAL);

        gpio_put(PICO_DEFAULT_LED_PIN, 0);
        sleep_ms(BLINK_INTERVAL);
    }
}