// Simple LED Blink (src component) for ESP32 DevKit with WROVER
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

// Built-in LED pin for ESP32 DevKit WROVER
#define LED_PIN GPIO_NUM_2
#define BLINK_DELAY_MS 100

static const char *TAG = "BLINK_SRC";

extern "C" void app_main(void)
{
    ESP_LOGI(TAG, "Starting LED Blink on GPIO %d", LED_PIN);

    // Configure LED pin
    gpio_reset_pin(LED_PIN);
    gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);

    bool led_state = false;

    while (true)
    {
        // Toggle LED
        led_state = !led_state;
        gpio_set_level(LED_PIN, led_state ? 1 : 0);

        ESP_LOGI(TAG, "LED: %s", led_state ? "ON" : "OFF");

        // Wait
        vTaskDelay(pdMS_TO_TICKS(BLINK_DELAY_MS));
    }
}