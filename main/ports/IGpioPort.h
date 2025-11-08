#pragma once

#include <cstdint>

namespace ports
{
    class IGpioPort
    {
    public:
        virtual ~IGpioPort() = default;

        virtual void configureAsOutput(uint8_t pin) = 0;
        virtual void write(uint8_t pin, bool state) = 0;
    };

} // namespace ports
