#ifndef NVS_STORAGE_H
#define NVS_STORAGE_H

#include "esp_err.h"
#include "device_config.h"

// NVS storage functions
esp_err_t nvs_storage_init(void);
esp_err_t nvs_storage_save_message(const mesh_message_t *message);
esp_err_t nvs_storage_load_messages(mesh_message_t *messages, size_t max_count, size_t *loaded_count);
esp_err_t nvs_storage_clear_messages(void);
esp_err_t nvs_storage_save_config(const char *key, const void *value, size_t length);
esp_err_t nvs_storage_load_config(const char *key, void *value, size_t *length);

#endif // NVS_STORAGE_H
