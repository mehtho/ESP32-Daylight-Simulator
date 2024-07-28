#include <stdio.h>
#include "light.h"
#include "wifi_ap.h"
#include "wifi_sta.h"

/*
    Initialization flow:

    Set up the GPIO pins
    Connect to the internet to pull time data
    Close the Wifi station
    Read from persistent data to find stored config info
    Open the Wifi AP for configuration
*/
void app_main(void)
{
    init_gpio_pwm();

    start_wifi_sta();
    sync_time_with_sntp();
    stop_wifi_sta();

    init_wifi_ap();
}