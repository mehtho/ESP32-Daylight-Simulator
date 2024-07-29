#include "persistence.h"
#include "stdint.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "misc_functions.h"

#define NVS_LIGHT_TAG_R "R"
#define NVS_LIGHT_TAG_G "G"
#define NVS_LIGHT_TAG_B "B"

#define READ_ERR_CHECK(err) do { \
    if ((err) == ESP_OK) { \
        /* Do nothing */ \
    } else if ((err) == ESP_ERR_NVS_NOT_FOUND) { \
        return false; \
    } else { \
        ESP_ERROR_CHECK(err); \
    } \
} while (0)

static const char* TAG = "Persistence";

esp_err_t open_nvs_handle(nvs_handle_t *nvs_handle_ptr, const char *light_tag) {
    ESP_LOGI(TAG, "Opening NVS handle");

    esp_err_t err = nvs_init();
    if(err != ESP_OK) {
        return err;
    }

    // TODO: Make read only if possible
    err = nvs_open(light_tag, NVS_READWRITE, nvs_handle_ptr);
    if(err != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS Handle");
        return err;
    }
    else {
        ESP_LOGI(TAG, "Opened NVS Handle");
        return ESP_OK;
    }
}

esp_err_t write_one_light(const char *light_tag, const uint32_t val) {
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(open_nvs_handle(&nvs_handle, light_tag));

    esp_err_t nvs_write_err = nvs_set_u32(nvs_handle, light_tag, val);

    if (nvs_write_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write to %s", light_tag);
        return nvs_write_err;
    }
    else {
        ESP_LOGI(TAG, "Wrote to %s, attempting to commit", light_tag);
    }

    nvs_write_err = nvs_commit(nvs_handle);

    if (nvs_write_err != ESP_OK) {
        ESP_LOGE(TAG, "Failed commit"); 
        return nvs_write_err;
    }
    else {
        ESP_LOGI(TAG, "Commit successful");
    }

    nvs_close(nvs_handle);
    return nvs_write_err;
}

void persistence_write_lights(const uint32_t r, const uint32_t g, const uint32_t b) {
    ESP_LOGI(TAG, "Attempting to write to NVS");

    ESP_ERROR_CHECK(write_one_light(NVS_LIGHT_TAG_R, r));   
    ESP_ERROR_CHECK(write_one_light(NVS_LIGHT_TAG_G, g));
    ESP_ERROR_CHECK(write_one_light(NVS_LIGHT_TAG_B, b));

    ESP_LOGI(TAG, "Wrote to NVS!");
}

esp_err_t read_one_light(const char *light_tag, uint32_t *val) {
    nvs_handle_t nvs_handle;
    ESP_ERROR_CHECK(open_nvs_handle(&nvs_handle, light_tag));

    esp_err_t nvs_read_err = nvs_get_u32(nvs_handle, light_tag, val);
    if (nvs_read_err != ESP_OK) {
        ESP_LOGI(TAG, "Failed to read %s", light_tag);
        return nvs_read_err;
    }
    else {
        ESP_LOGI(TAG, "Read value %s", light_tag);
    }

    switch (nvs_read_err) {
        case ESP_OK:
            ESP_LOGI(TAG, "Read %s\n", light_tag);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            ESP_LOGW(TAG, "%s not initialized", light_tag);
            break;
        default :
            ESP_LOGE(TAG, "Error reading %s (%s)", light_tag, esp_err_to_name(nvs_read_err));
    }
    return nvs_read_err;
}

bool persistence_read_lights(uint32_t *r, uint32_t *g, uint32_t *b) {
    ESP_LOGI(TAG, "Attempting to read from NVS");

    esp_err_t err_r = read_one_light(NVS_LIGHT_TAG_R, r);
    READ_ERR_CHECK(err_r);

    esp_err_t err_g = read_one_light(NVS_LIGHT_TAG_G, g);
    READ_ERR_CHECK(err_g); 

    esp_err_t err_b = read_one_light(NVS_LIGHT_TAG_B, b);
    READ_ERR_CHECK(err_b); 

    ESP_LOGI(TAG, "Read light colours from NVS");
    return true;
}