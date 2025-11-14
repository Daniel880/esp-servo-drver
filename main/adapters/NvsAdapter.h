#ifndef NVSADAPTER_H
#define NVSADAPTER_H

#include "nvs_flash.h"

namespace adapters
{
    class NvsAdapter
    {
    private:
        static constexpr const char *TAG = "NvsAdapter";

    public:
        NvsAdapter() = default;
        void initialize();
    };

} // namespace adapters

#endif // NVSADAPTER_H
