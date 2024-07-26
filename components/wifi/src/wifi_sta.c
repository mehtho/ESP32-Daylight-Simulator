#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"

static const char* TAG = "WiFi_STA";

void init_wifi_sta(void) {
    ESP_LOGI(TAG, "Beginning WiFi configuration");
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_LOGI(TAG, "NVS FLASH INIT DONE");

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    wifi_config_t cfg = {
        .sta = {
            .ssid           = CONFIG_OUTBOUND_WIFI_SSID,
            .password       = CONFIG_OUTBOUND_WIFI_PASSWORD
        }
    };

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "WiFi STA connected to SSID %s with Password %s", 
        CONFIG_OUTBOUND_WIFI_SSID, CONFIG_OUTBOUND_WIFI_PASSWORD);
}

void deinit_wifi_sta() {
    
}