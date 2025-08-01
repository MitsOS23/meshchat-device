#ifndef WIFI_AP_H
#define WIFI_AP_H

#include "esp_err.h"

// WiFi Access Point functions (backup interface)
esp_err_t wifi_ap_init(void);
esp_err_t wifi_ap_start(void);
esp_err_t wifi_ap_stop(void);
bool wifi_ap_is_running(void);
int wifi_ap_get_client_count(void);

#endif // WIFI_AP_H
