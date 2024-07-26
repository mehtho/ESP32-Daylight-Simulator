#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

static const char* TAG = "WiFi_AP";

void init_wifi_ap(void) {
    ESP_LOGI(TAG, "Beginning WiFi configuration");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "NVS FLASH INIT DONE");

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    wifi_config_t cfg = {
        .ap = {
            .ssid           = CONFIG_INBOUND_WIFI_SSID,
            .password       = CONFIG_INBOUND_WIFI_PASSWORD,
            .authmode       = WIFI_AUTH_WPA3_PSK,
            .max_connection = 15
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi AP Started on SSID %s with Password %s", 
        CONFIG_INBOUND_WIFI_SSID, CONFIG_INBOUND_WIFI_PASSWORD);
}
