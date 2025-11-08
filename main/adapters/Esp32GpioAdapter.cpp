#include "Esp32GpioAdapter.h"
#include "driver/gpio.h"

namespace adapters
{
    void Esp32GpioAdapter::configureAsOutput(uint8_t pin)
    {
        gpio_reset_pin(static_cast<gpio_num_t>(pin));
        gpio_set_direction(static_cast<gpio_num_t>(pin), GPIO_MODE_OUTPUT);
    }

    void Esp32GpioAdapter::write(uint8_t pin, bool state)
    {
        gpio_set_level(static_cast<gpio_num_t>(pin), state ? 1 : 0);
    }

} // namespace adapters
