#ifndef BLE_SERVER_H
#define BLE_SERVER_H

#include "esp_err.h"
#include "esp_gatts_api.h"

// BLE server functions
esp_err_t ble_server_init(void);
esp_err_t ble_server_send_message(const char *message);
esp_err_t ble_server_notify_clients(const char *data, size_t length);

// Callback for received data from client
typedef void (*ble_data_callback_t)(const char *data, size_t length);
void ble_server_set_data_callback(ble_data_callback_t callback);

#endif // BLE_SERVER_H
