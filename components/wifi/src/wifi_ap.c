#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_mac.h"

#include "misc_functions.h"

static const char* TAG = "WiFi_AP";

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d",
                 MAC2STR(event->mac), event->aid);
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d, reason=%d",
                 MAC2STR(event->mac), event->aid, event->reason);
    }
}

void init_wifi_ap(void) {
    ESP_LOGI(TAG, "Beginning WiFi configuration");
    ESP_ERROR_CHECK(nvs_init());
    esp_netif_create_default_wifi_ap();

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                    ESP_EVENT_ANY_ID,
                                                    &wifi_event_handler,
                                                    NULL,
                                                    NULL));

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
