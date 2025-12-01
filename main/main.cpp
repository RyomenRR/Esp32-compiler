#include <string.h>
#include "nvs_flash.h"
#include "esp_log.h"

#include "esp_rmaker_core.h"
#include "esp_rmaker_standard_types.h"
#include "esp_rmaker_standard_params.h"
#include "esp_rmaker_wifi.h"
#include "esp_rmaker_schedule.h"
#include "esp_rmaker_provisioning.h"
#include "esp_rmaker_ota.h"

#include "wifi_provisioning/manager.h"
#include "wifi_provisioning/scheme_ble.h"

static const char *TAG = "rainmaker";

// Relay GPIO pins
#define RELAY1 23
#define RELAY2 22
#define RELAY3 19
#define RELAY4 18

esp_err_t relay_cb(const esp_rmaker_device_t *device,
                   const esp_rmaker_param_t *param,
                   const esp_rmaker_param_val_t val,
                   void *ctx)
{
    bool state = val.val.b;
    ESP_LOGI(TAG, "Relay state changed to %d", state);
    return ESP_OK;
}

void app_main(void)
{
    nvs_flash_init();

    esp_rmaker_config_t config = {
        .enable_time_sync = true,
    };

    esp_rmaker_node_t *node =
        esp_rmaker_node_init(&config, "RelayBoard", "SwitchBoardType");

    esp_rmaker_device_t *relay1 =
        esp_rmaker_device_create("Relay1", ESP_RMAKER_DEVICE_SWITCH, NULL);
    esp_rmaker_param_t *param1 =
        esp_rmaker_param_create(ESP_RMAKER_DEF_POWER_NAME,
                                ESP_RMAKER_PARAM_POWER,
                                esp_rmaker_bool(false),
                                ESP_RMAKER_PARAM_PROP_DYNAMIC,
                                relay_cb, NULL);
    esp_rmaker_device_add_param(relay1, param1);
    esp_rmaker_node_add_device(node, relay1);

    ESP_LOGI(TAG, "Starting RainMaker...");
    esp_rmaker_wifi_init();
    esp_rmaker_start();

    ESP_LOGI(TAG, "Starting BLE provisioning...");
    wifi_prov_mgr_config_t prov_cfg = {
        .scheme = wifi_prov_scheme_ble,
        .scheme_event_handler = WIFI_PROV_EVENT_HANDLER_NONE
    };
    wifi_prov_mgr_init(prov_cfg);

    const char *pop = "12345678";
    wifi_prov_mgr_start_provisioning(WIFI_PROV_SECURITY_1, pop,
                                     "RelayBoard", NULL);
}
