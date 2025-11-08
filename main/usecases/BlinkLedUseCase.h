#pragma once

#include "../ports/IGpioPort.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstdint>

namespace usecases
{
    class BlinkLedUseCase
    {
    public:
        static constexpr uint8_t LED_PIN = 2;
        static constexpr uint32_t BLINK_INTERVAL_MS = 200;

        explicit BlinkLedUseCase(ports::IGpioPort &gpioPort);

        void initialize();
        void startThread();

    private:
        static void run(void *parameter);

        ports::IGpioPort &m_gpioPort;
        TaskHandle_t m_taskHandle;
        bool m_isOn;
    };

} // namespace usecases
