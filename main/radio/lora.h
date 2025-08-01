#ifndef LORA_H
#define LORA_H

#include "esp_err.h"
#include "device_config.h"

// Function prototypes
esp_err_t lora_init(void);
esp_err_t lora_send_message(const mesh_message_t *message);
esp_err_t lora_receive_message(mesh_message_t *message, uint32_t timeout_ms);
esp_err_t lora_set_power(int8_t power);
esp_err_t lora_sleep(void);
esp_err_t lora_wake(void);
int lora_get_rssi(void);
float lora_get_snr(void);

#endif // LORA_H
