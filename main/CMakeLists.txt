idf_component_register(SRCS "main.cpp"
                            "adapters/Esp32GpioAdapter.cpp"
                            "usecases/BlinkLedUseCase.cpp"
                            "adapters/WiFiAdapter.cpp"
                            "adapters/NvsAdapter.cpp"
                            "adapters/MqttAdapter.cpp"
                       PRIV_REQUIRES esp_driver_gpio esp_wifi nvs_flash mqtt)