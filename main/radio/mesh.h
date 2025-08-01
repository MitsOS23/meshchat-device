#ifndef MESH_H
#define MESH_H

#include "esp_err.h"
#include "device_config.h"

// Mesh network functions
esp_err_t mesh_init(void);
esp_err_t mesh_send_text_message(const uint8_t *recipient_id, const char *text);
esp_err_t mesh_send_broadcast(const char *text);
esp_err_t mesh_process(void);
esp_err_t mesh_add_route(const uint8_t *destination, const uint8_t *next_hop, uint8_t hop_count);
esp_err_t mesh_remove_route(const uint8_t *destination);
route_entry_t* mesh_find_route(const uint8_t *destination);
void mesh_cleanup_old_routes(void);
uint32_t mesh_generate_message_id(void);
uint16_t mesh_calculate_checksum(const mesh_message_t *message);
bool mesh_verify_checksum(const mesh_message_t *message);
void mesh_get_device_id(uint8_t *device_id);

// Callback for received messages
typedef void (*mesh_message_callback_t)(const mesh_message_t *message);
void mesh_set_message_callback(mesh_message_callback_t callback);

#endif // MESH_H
