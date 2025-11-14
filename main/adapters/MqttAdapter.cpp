#include "MqttAdapter.h"
#include <cstring>

namespace adapters
{

    MqttAdapter::MqttAdapter(std::string uri, std::string client_id, std::string username, std::string password)
        : uri_(std::move(uri)),
          client_id_(std::move(client_id)),
          username_(std::move(username)),
          password_(std::move(password)),
          client_(nullptr),
          connected_(false) {}

    MqttAdapter::~MqttAdapter()
    {
        if (client_)
        {
            esp_mqtt_client_stop(client_);
            esp_mqtt_client_destroy(client_);
        }
    }

    bool MqttAdapter::initialize()
    {
        esp_mqtt_client_config_t cfg = {};
        cfg.broker.address.uri = uri_.c_str();
        cfg.credentials.client_id = client_id_.c_str();

        // Set username and password if provided
        if (!username_.empty())
        {
            cfg.credentials.username = username_.c_str();
        }
        if (!password_.empty())
        {
            cfg.credentials.authentication.password = password_.c_str();
        }

        client_ = esp_mqtt_client_init(&cfg);
        if (!client_)
        {
            ESP_LOGE("MqttAdapter", "Failed to init MQTT client");
            return false;
        }

        esp_mqtt_client_register_event(client_, MQTT_EVENT_ANY, mqtt_event_handler, this);

        esp_err_t err = esp_mqtt_client_start(client_);
        if (err != ESP_OK)
        {
            ESP_LOGE("MqttAdapter", "Failed to start MQTT client: %d", err);
            return false;
        }

        ESP_LOGI("MqttAdapter", "MQTT client initialized, connecting to %s", uri_.c_str());
        return true;
    }

    void MqttAdapter::publish(const std::string &topic, const std::string &payload, int qos, bool retain)
    {
        if (!client_)
            return;
        esp_mqtt_client_publish(client_, topic.c_str(), payload.c_str(), payload.size(), qos, retain);
    }

    bool MqttAdapter::isConnected() const
    {
        return connected_;
    }

    void MqttAdapter::mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
    {
        auto *adapter = static_cast<MqttAdapter *>(handler_args);
        if (adapter)
        {
            adapter->handleEvent(event_id, event_data);
        }
    }

    void MqttAdapter::handleEvent(int32_t event_id, void *event_data)
    {
        esp_mqtt_event_handle_t event = static_cast<esp_mqtt_event_handle_t>(event_data);
        switch (event_id)
        {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_CONNECTED");
            connected_ = true;
            // subscribe to all topics using wildcard
            if (client_)
            {
                esp_mqtt_client_subscribe(client_, "#", 0);
            }
            break;
        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_DISCONNECTED");
            connected_ = false;
            break;
        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_DATA");
            if (event->topic_len && event->data_len)
            {
                std::string topic(event->topic, event->topic_len);
                std::string data(event->data, event->data_len);
                ESP_LOGI("MqttAdapter", "Topic=%s, Data=%s", topic.c_str(), data.c_str());
            }
            break;
        case MQTT_EVENT_BEFORE_CONNECT:
            ESP_LOGI("MqttAdapter", "MQTT_EVENT_BEFORE_CONNECT");
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGE("MqttAdapter", "MQTT_EVENT_ERROR");
            if (event->error_handle)
            {
                ESP_LOGE("MqttAdapter", "Error type: %d", event->error_handle->error_type);
                if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
                {
                    ESP_LOGE("MqttAdapter", "Last ESP error: 0x%x", event->error_handle->esp_tls_last_esp_err);
                    ESP_LOGE("MqttAdapter", "Socket errno: %d", event->error_handle->esp_transport_sock_errno);
                }
            }
            break;
        default:
            ESP_LOGI("MqttAdapter", "Other MQTT event id=%d", event_id);
            break;
        }
    }

} // namespace adapters
