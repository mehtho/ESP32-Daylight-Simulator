#include "light.h"
#include "esp_log.h"
#include "driver/ledc.h"

#define LEDC_TIMER        LEDC_TIMER_0
#define LEDC_MODE         LEDC_LOW_SPEED_MODE
#define LEDC_DUTY_RES     LEDC_TIMER_13_BIT 
#define LEDC_DUTY         (4096)
#define LEDC_FREQUENCY    (4096)

#define LEDC_CHANNEL_R    LEDC_CHANNEL_0
#define LEDC_CHANNEL_G    LEDC_CHANNEL_1
#define LEDC_CHANNEL_B    LEDC_CHANNEL_2

#define PWM_RED     (21)
#define PWM_GREEN   (19)
#define PWM_BLUE    (18)

static const char* TAG = "Lights";

void init_gpio_pwm(void){
    ledc_timer_config_t ledc_timer = {
        .duty_resolution    = LEDC_DUTY_RES,
        .freq_hz            = LEDC_FREQUENCY,
        .speed_mode         = LEDC_MODE,
        .timer_num          = LEDC_TIMER,
        .clk_cfg            = LEDC_AUTO_CLK
    };
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel_r = {
        .channel    = LEDC_CHANNEL_R,
        .duty       = 0,
        .gpio_num   = PWM_RED,
        .speed_mode = LEDC_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE
    };
    ledc_channel_config(&ledc_channel_r);

    ledc_channel_config_t ledc_channel_g = {
        .channel    = LEDC_CHANNEL_G,
        .duty       = 0,
        .gpio_num   = PWM_GREEN,
        .speed_mode = LEDC_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE
    };
    ledc_channel_config(&ledc_channel_g);

    ledc_channel_config_t ledc_channel_b = {
        .channel    = LEDC_CHANNEL_B,
        .duty       = 0,
        .gpio_num   = PWM_BLUE,
        .speed_mode = LEDC_MODE,
        .hpoint     = 0,
        .timer_sel  = LEDC_TIMER,
        .intr_type  = LEDC_INTR_DISABLE
    };
    ledc_channel_config(&ledc_channel_b);
    ESP_LOGI(TAG, "Initialised LEDs");
}

void set_lights(int r, int g, int b) {
    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, r);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_1, g);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_1);

    ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_2, b);
    ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_2);

    ESP_LOGI(TAG, "Set R%d G%d B%d", r, g, b);
}
