#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "usecases/BlinkLedUseCase.h"
#include "adapters/Esp32GpioAdapter.h"
#include "adapters/WiFiAdapter.h"
#include "adapters/MqttAdapter.h"
#include "adapters/NvsAdapter.h"
#include <string>

static const char *TAG = "main";

extern "C" void app_main(void)
{
    adapters::NvsAdapter nvsAdapter;
    nvsAdapter.initialize();

    adapters::Esp32GpioAdapter gpioAdapter;

    adapters::WiFiAdapter wifiAdapter("a4ffc2", "password123");
    wifiAdapter.initialize();

    adapters::MqttAdapter mqttAdapter("mqtt://192.168.0.10", "esp_servo_client");
    mqttAdapter.initialize();

    usecases::BlinkLedUseCase blinkUseCase(gpioAdapter);

    blinkUseCase.initialize();
    blinkUseCase.startThread();

    int counter = 0;
    while (true)
    {
        std::string payload = "hello from esp32: " + std::to_string(counter++);
        mqttAdapter.publish("esp/servo/heartbeat", payload);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}