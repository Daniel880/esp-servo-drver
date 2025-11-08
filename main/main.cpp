#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "usecases/BlinkLedUseCase.h"
#include "adapters/Esp32GpioAdapter.h"

extern "C" void app_main(void)
{
    adapters::Esp32GpioAdapter gpioAdapter;

    usecases::BlinkLedUseCase blinkUseCase(gpioAdapter);

    blinkUseCase.initialize();
    blinkUseCase.startThread();

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}