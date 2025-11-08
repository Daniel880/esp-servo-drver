idf_component_register(SRCS "main.cpp"
                            "adapters/Esp32GpioAdapter.cpp"
                            "usecases/BlinkLedUseCase.cpp"
                       PRIV_REQUIRES esp_driver_gpio)
