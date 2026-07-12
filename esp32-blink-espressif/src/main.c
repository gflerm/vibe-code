#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "led_strip.h"

#ifndef RGB_LED_GPIO
#define RGB_LED_GPIO 48
#endif

#ifndef BLINK_INTERVAL_MS
#define BLINK_INTERVAL_MS 500
#endif

static led_strip_handle_t create_rgb_led(void)
{
    const led_strip_config_t strip_config = {
        .strip_gpio_num = RGB_LED_GPIO,
        .max_leds = 1,
        .led_model = LED_MODEL_WS2812,
        .flags.invert_out = false,
    };

    const led_strip_rmt_config_t rmt_config = {
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = 10 * 1000 * 1000,
        .mem_block_symbols = 0,
        .flags.with_dma = false,
    };

    led_strip_handle_t led = NULL;
    led_strip_new_rmt_device(&strip_config, &rmt_config, &led);
    return led;
}

static void set_led(led_strip_handle_t led, bool on)
{
    if (on) {
        led_strip_set_pixel(led, 0, 51, 0, 0);
        led_strip_refresh(led);
    } else {
        led_strip_clear(led);
    }
}

void app_main(void)
{
    const led_strip_handle_t led = create_rgb_led();

    while (true) {
        set_led(led, true);
        vTaskDelay(pdMS_TO_TICKS(BLINK_INTERVAL_MS));

        set_led(led, false);
        vTaskDelay(pdMS_TO_TICKS(BLINK_INTERVAL_MS));
    }
}
