#ifndef GATT_SRV_H
#define GATT_SRV_H

#include "esp_err.h"
#include "esp_gatts_api.h"
#include "ble_server.h"

// GATT service functions
esp_err_t gatt_server_init(void);
esp_err_t gatt_server_create_service(esp_gatt_if_t gatts_if);
esp_err_t gatt_server_send_notification(esp_gatt_if_t gatts_if, uint16_t conn_id, const char *data, size_t length);
void gatt_server_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void gatt_server_set_data_callback(ble_data_callback_t callback);

#endif // GATT_SRV_H
