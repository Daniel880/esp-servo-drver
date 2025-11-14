#include "NvsAdapter.h"
#include "esp_log.h"

namespace adapters
{
    void NvsAdapter::initialize()
    {
        // Initialize NVS
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            // NVS partition was truncated and needs to be erased
            ESP_LOGI(TAG, "NVS partition needs to be erased, erasing...");
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
        ESP_LOGI(TAG, "NVS initialized successfully");
    }

} // namespace adapters
