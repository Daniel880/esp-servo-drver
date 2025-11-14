#ifndef WIFIADAPTER_H
#define WIFIADAPTER_H

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include <string>

namespace adapters
{
    class WiFiAdapter
    {
    private:
        static constexpr const char *TAG = "WiFiAdapter";
        static constexpr int WIFI_CONNECTED_BIT = BIT0;
        static constexpr int WIFI_FAIL_BIT = BIT1;
        static constexpr int MAX_RETRY_ATTEMPTS = 3;

        std::string ssid_;
        std::string password_;
        EventGroupHandle_t event_group_;
        int retry_count_;
        esp_event_handler_instance_t wifi_event_instance_;
        esp_event_handler_instance_t ip_event_instance_;

        static void eventHandler(void *arg, esp_event_base_t event_base,
                                 int32_t event_id, void *event_data);

        void handleEvent(esp_event_base_t event_base, int32_t event_id, void *event_data);
        void initializeNetworkInterface();
        void registerEventHandlers();
        void configureWiFi();
        bool waitForConnection();

    public:
        WiFiAdapter(std::string ssid = "a4ffc2", std::string password = "266957124");
        ~WiFiAdapter();

        // Delete copy constructor and assignment operator
        WiFiAdapter(const WiFiAdapter &) = delete;
        WiFiAdapter &operator=(const WiFiAdapter &) = delete;

        // Allow move semantics
        WiFiAdapter(WiFiAdapter &&other) noexcept;
        WiFiAdapter &operator=(WiFiAdapter &&other) noexcept;

        bool initialize();
        bool isConnected() const;
        const std::string &getSsid() const;
    };

} // namespace adapters

#endif // WIFIADAPTER_H
