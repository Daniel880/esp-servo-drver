#include "WiFiAdapter.h"
#include <cstring>

namespace adapters
{
    // Static event handler callback
    void WiFiAdapter::eventHandler(void *arg, esp_event_base_t event_base,
                                   int32_t event_id, void *event_data)
    {
        auto *adapter = static_cast<WiFiAdapter *>(arg);
        adapter->handleEvent(event_base, event_id, event_data);
    }

    // Instance event handler
    void WiFiAdapter::handleEvent(esp_event_base_t event_base, int32_t event_id, void *event_data)
    {
        if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
        {
            ESP_LOGI(TAG, "WiFi station started, connecting...");
            esp_wifi_connect();
        }
        else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
        {
            if (retry_count_ < MAX_RETRY_ATTEMPTS)
            {
                esp_wifi_connect();
                retry_count_++;
                ESP_LOGI(TAG, "Retry to connect to AP (attempt %d/%d)",
                         retry_count_, MAX_RETRY_ATTEMPTS);
            }
            else
            {
                xEventGroupSetBits(event_group_, WIFI_FAIL_BIT);
                ESP_LOGE(TAG, "Failed to connect to AP after %d attempts", MAX_RETRY_ATTEMPTS);
            }
        }
        else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
        {
            auto *event = static_cast<ip_event_got_ip_t *>(event_data);
            ESP_LOGI(TAG, "Got IP address: " IPSTR, IP2STR(&event->ip_info.ip));
            retry_count_ = 0;
            xEventGroupSetBits(event_group_, WIFI_CONNECTED_BIT);
        }
    }

    void WiFiAdapter::initializeNetworkInterface()
    {
        ESP_ERROR_CHECK(esp_netif_init());
        ESP_ERROR_CHECK(esp_event_loop_create_default());
        esp_netif_create_default_wifi_sta();
    }

    void WiFiAdapter::registerEventHandlers()
    {
        ESP_ERROR_CHECK(esp_event_handler_instance_register(
            WIFI_EVENT,
            ESP_EVENT_ANY_ID,
            &WiFiAdapter::eventHandler,
            this,
            &wifi_event_instance_));

        ESP_ERROR_CHECK(esp_event_handler_instance_register(
            IP_EVENT,
            IP_EVENT_STA_GOT_IP,
            &WiFiAdapter::eventHandler,
            this,
            &ip_event_instance_));
    }

    void WiFiAdapter::configureWiFi()
    {
        wifi_config_t wifi_config = {};
        strncpy(reinterpret_cast<char *>(wifi_config.sta.ssid),
                ssid_.c_str(), sizeof(wifi_config.sta.ssid) - 1);
        strncpy(reinterpret_cast<char *>(wifi_config.sta.password),
                password_.c_str(), sizeof(wifi_config.sta.password) - 1);
        wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
        wifi_config.sta.sae_pwe_h2e = WPA3_SAE_PWE_UNSPECIFIED;

        ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    }

    bool WiFiAdapter::waitForConnection()
    {
        ESP_LOGI(TAG, "Waiting for WiFi connection...");

        EventBits_t bits = xEventGroupWaitBits(
            event_group_,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

        if (bits & WIFI_CONNECTED_BIT)
        {
            ESP_LOGI(TAG, "Successfully connected to SSID: %s", ssid_.c_str());
            return true;
        }
        else if (bits & WIFI_FAIL_BIT)
        {
            ESP_LOGE(TAG, "Failed to connect to SSID: %s", ssid_.c_str());
            return false;
        }
        else
        {
            ESP_LOGE(TAG, "Unexpected WiFi event");
            return false;
        }
    }

    // Constructor
    WiFiAdapter::WiFiAdapter(std::string ssid, std::string password)
        : ssid_(std::move(ssid)), password_(std::move(password)), event_group_(nullptr), retry_count_(0), wifi_event_instance_(nullptr), ip_event_instance_(nullptr)
    {
    }

    // Destructor
    WiFiAdapter::~WiFiAdapter()
    {
        if (event_group_)
        {
            vEventGroupDelete(event_group_);
        }
    }

    // Move constructor
    WiFiAdapter::WiFiAdapter(WiFiAdapter &&other) noexcept
        : ssid_(std::move(other.ssid_)), password_(std::move(other.password_)), event_group_(other.event_group_), retry_count_(other.retry_count_), wifi_event_instance_(other.wifi_event_instance_), ip_event_instance_(other.ip_event_instance_)
    {
        other.event_group_ = nullptr;
    }

    // Move assignment operator
    WiFiAdapter &WiFiAdapter::operator=(WiFiAdapter &&other) noexcept
    {
        if (this != &other)
        {
            if (event_group_)
            {
                vEventGroupDelete(event_group_);
            }

            ssid_ = std::move(other.ssid_);
            password_ = std::move(other.password_);
            event_group_ = other.event_group_;
            retry_count_ = other.retry_count_;
            wifi_event_instance_ = other.wifi_event_instance_;
            ip_event_instance_ = other.ip_event_instance_;

            other.event_group_ = nullptr;
        }
        return *this;
    }

    // Public methods
    bool WiFiAdapter::initialize()
    {
        event_group_ = xEventGroupCreate();
        if (!event_group_)
        {
            ESP_LOGE(TAG, "Failed to create event group");
            return false;
        }

        initializeNetworkInterface();

        wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
        ESP_ERROR_CHECK(esp_wifi_init(&cfg));

        registerEventHandlers();
        configureWiFi();

        ESP_ERROR_CHECK(esp_wifi_start());

        return waitForConnection();
    }

    bool WiFiAdapter::isConnected() const
    {
        if (!event_group_)
        {
            return false;
        }

        EventBits_t bits = xEventGroupGetBits(event_group_);
        return (bits & WIFI_CONNECTED_BIT) != 0;
    }

    const std::string &WiFiAdapter::getSsid() const
    {
        return ssid_;
    }

} // namespace adapters
