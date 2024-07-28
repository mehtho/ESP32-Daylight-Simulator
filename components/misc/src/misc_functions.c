#include "misc_functions.h"
#include "esp_system.h"
#include "nvs_flash.h"

esp_err_t nvs_init(void) {
    esp_err_t flash_init_err = nvs_flash_init();

    if (flash_init_err == ESP_ERR_NVS_NO_FREE_PAGES || flash_init_err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        esp_err_t flash_erase_err = nvs_flash_erase();
        if (flash_erase_err != ESP_OK) {
            return flash_erase_err;
        }
        flash_init_err = nvs_flash_init();
    }

   return flash_init_err;
}