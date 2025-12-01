idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_rmaker_json esp_rmaker_factory fs spi_flash app_update nvs_flash
              wifi_provisioning esp_rmaker_core esp_rmaker_standard_types
              esp_rmaker_standard_params esp_rmaker_wifi esp_rmaker_schedule
              esp_rmaker_provisioning esp_rmaker_ota
)
