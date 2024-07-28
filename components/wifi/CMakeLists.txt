idf_component_register(SRCS "src/wifi_ap.c" "src/wifi_sta.c"
                INCLUDE_DIRS "include"
                PRIV_INCLUDE_DIRS ""
                PRIV_REQUIRES "driver" "esp_wifi" "nvs_flash"
                REQUIRES "misc")