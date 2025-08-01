#ifndef POWER_MGMT_H
#define POWER_MGMT_H

#include "esp_err.h"

// Power management functions
esp_err_t power_mgmt_init(void);
esp_err_t power_mgmt_update(void);
esp_err_t power_mgmt_sleep(uint32_t duration_ms);
esp_err_t power_mgmt_deep_sleep(uint32_t duration_ms);
uint16_t power_mgmt_get_battery_voltage(void);
uint8_t power_mgmt_get_battery_percentage(void);
bool power_mgmt_is_battery_low(void);
void power_mgmt_activity_notify(void);

#endif // POWER_MGMT_H
