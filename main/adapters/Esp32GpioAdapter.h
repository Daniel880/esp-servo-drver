#pragma once

#include "../ports/IGpioPort.h"
#include <cstdint>

namespace adapters
{
    class Esp32GpioAdapter : public ports::IGpioPort
    {
    public:
        void configureAsOutput(uint8_t pin) override;
        void write(uint8_t pin, bool state) override;
    };

} // namespace adapters
