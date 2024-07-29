#include <stdio.h>
#include "esp_event.h"

#include "light.h"
#include "persistence.h"
#include "wifi_ap.h"
#include "wifi_sta.h"

/*
    Initialization flow:

    Set up the GPIO pins

    Connect to the internet to pull time data
    Close the Wifi station

    Read from persistent data to find stored config info
    Open the Wifi AP for configuration

    Configure the light colours
*/
void app_main(void)
{
    esp_event_loop_create_default();

    init_gpio_pwm();

    start_wifi_sta();
    sync_time_with_sntp();
    stop_wifi_sta();

    init_wifi_ap();

    uint32_t r, g, b;

    persistence_read_lights(&r, &g, &b);
    persistence_write_lights(8192u, 8192u, 8192u);
}