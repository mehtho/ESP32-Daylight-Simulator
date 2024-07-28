#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include "esp_sntp.h"
#include "esp_netif_sntp.h"
#include "esp_event.h"
#include "freertos/event_groups.h"
#include "misc_functions.h"

#define WIFI_CONNECTED_BIT      BIT0
#define WIFI_FAIL_BIT           BIT1

static const char* TAG = "WiFi_STA";
static EventGroupHandle_t s_wifi_event_group;
static int wifi_retry_count = 0;
static bool is_wifi_shutting_down = false;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED && !is_wifi_shutting_down) {
        if (wifi_retry_count < CONFIG_WIFI_MAX_RETRIES) {
            esp_wifi_connect();
            wifi_retry_count++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        wifi_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

void start_wifi_sta(void) {
    ESP_LOGI(TAG, "Beginning WiFi configuration");
    ESP_ERROR_CHECK(nvs_init());
    ESP_ERROR_CHECK(esp_netif_init());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t init_cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_cfg));

    wifi_config_t cfg = {
        .sta = {
            .ssid           = CONFIG_OUTBOUND_WIFI_SSID,
            .password       = CONFIG_OUTBOUND_WIFI_PASSWORD
        }
    };

    s_wifi_event_group = xEventGroupCreate();
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &cfg));
    ESP_ERROR_CHECK(esp_wifi_start());


    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
        WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE,
        pdFALSE,
        portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "WiFi STA connected to SSID %s with Password %s", 
        CONFIG_OUTBOUND_WIFI_SSID, CONFIG_OUTBOUND_WIFI_PASSWORD);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 CONFIG_OUTBOUND_WIFI_SSID, CONFIG_OUTBOUND_WIFI_PASSWORD);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
}

void sync_time_with_sntp(void) {
    ESP_LOGI(TAG, "Initializing SNTP");
    esp_sntp_config_t time_cfg = ESP_NETIF_SNTP_DEFAULT_CONFIG(CONFIG_SNTP_SERVER_HOSTNAME);
    ESP_ERROR_CHECK(esp_netif_sntp_init(&time_cfg));

    ESP_LOGI(TAG, "Starting SNTP");
    ESP_ERROR_CHECK(esp_netif_sntp_start());

    int retry = 0;

    while (esp_netif_sntp_sync_wait(2000 / portTICK_PERIOD_MS) == ESP_ERR_TIMEOUT && retry++ < CONFIG_SNTP_MAX_RETRIES) {
        ESP_LOGI(TAG, "Waiting for system time to be set... (%d/%d)", retry, CONFIG_SNTP_MAX_RETRIES);
    }

    if (retry >= CONFIG_SNTP_MAX_RETRIES) {
        ESP_LOGE(TAG, "SNTP failed");
    }
    else {
        ESP_LOGI(TAG, "SNTP ok!");

        time_t now;
        struct tm timeinfo;
        char strftime_buf[64];
        setenv("TZ", "GMT-8", 1);
        tzset();

        time(&now);
        localtime_r(&now, &timeinfo);
        strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
        ESP_LOGI(TAG, "The current date/time is: %s", strftime_buf);
    }

    esp_netif_sntp_deinit();
}

void stop_wifi_sta(void) {
    is_wifi_shutting_down = true;

    ESP_ERROR_CHECK(esp_wifi_disconnect());
    ESP_ERROR_CHECK(esp_wifi_stop());
    ESP_ERROR_CHECK(esp_wifi_deinit());
}
