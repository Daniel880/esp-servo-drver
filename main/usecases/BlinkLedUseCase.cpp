#include "BlinkLedUseCase.h"

namespace usecases
{
    BlinkLedUseCase::BlinkLedUseCase(ports::IGpioPort &gpioPort)
        : m_gpioPort(gpioPort), m_taskHandle(nullptr), m_isOn(false)
    {
    }

    void BlinkLedUseCase::initialize()
    {
        m_gpioPort.configureAsOutput(LED_PIN);
    }

    void BlinkLedUseCase::startThread()
    {
        xTaskCreate(
            run,
            "BlinkTask",
            2048,
            this,
            5,
            &m_taskHandle);
    }

    void BlinkLedUseCase::run(void *parameter)
    {
        BlinkLedUseCase *useCase = static_cast<BlinkLedUseCase *>(parameter);

        while (true)
        {
            useCase->m_isOn = !useCase->m_isOn;
            useCase->m_gpioPort.write(LED_PIN, useCase->m_isOn);
            vTaskDelay(pdMS_TO_TICKS(BLINK_INTERVAL_MS));
        }
    }

} // namespace usecases
