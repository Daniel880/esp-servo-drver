#ifndef MQTTADAPTER_H
#define MQTTADAPTER_H

#include "esp_event.h"
#include "esp_log.h"
#include <string>

#include "mqtt_client.h"

namespace adapters
{

    class MqttAdapter
    {
    public:
        MqttAdapter(std::string uri = "mqtt://192.168.0.10",
                    std::string client_id = "esp32_mqtt_client",
                    std::string username = "",
                    std::string password = "");
        ~MqttAdapter();

        // non-copyable
        MqttAdapter(const MqttAdapter &) = delete;
        MqttAdapter &operator=(const MqttAdapter &) = delete;

        bool initialize();
        void publish(const std::string &topic, const std::string &payload, int qos = 0, bool retain = false);
        bool isConnected() const;

    private:
        std::string uri_;
        std::string client_id_;
        std::string username_;
        std::string password_;
        esp_mqtt_client_handle_t client_;
        bool connected_;

        static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
        void handleEvent(int32_t event_id, void *event_data);
    };

} // namespace adapters

#endif // MQTTADAPTER_H
