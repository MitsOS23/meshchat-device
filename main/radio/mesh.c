#include "mesh.h"
#include "lora.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <string.h>
#include <time.h>
#include <sys/time.h>

static const char *TAG = "MESH";

// Global variables
static route_entry_t route_table[MAX_ROUTES];
static mesh_message_t message_queue[MAX_MESSAGES];
static uint8_t device_id[8];
static uint32_t message_counter = 0;
static mesh_message_callback_t message_callback = NULL;
static TaskHandle_t mesh_task_handle = NULL;
static QueueHandle_t tx_queue;

// Forward declarations
static void mesh_task(void *parameters);
static void mesh_handle_received_message(const mesh_message_t *message);
static void mesh_send_beacon(void);
static bool mesh_is_message_duplicate(uint32_t msg_id, const uint8_t *sender_id);

esp_err_t mesh_init(void)
{
    // Get device MAC address as unique ID
    esp_wifi_get_mac(WIFI_IF_STA, device_id);
    
    // Initialize route table
    memset(route_table, 0, sizeof(route_table));
    
    // Initialize message queue
    memset(message_queue, 0, sizeof(message_queue));
    
    // Create TX queue for outgoing messages
    tx_queue = xQueueCreate(10, sizeof(mesh_message_t));
    if (tx_queue == NULL) {
        ESP_LOGE(TAG, "Failed to create TX queue");
        return ESP_ERR_NO_MEM;
    }
    
    // Create mesh processing task
    if (xTaskCreate(mesh_task, "mesh_task", 4096, NULL, 5, &mesh_task_handle) != pdPASS) {
        ESP_LOGE(TAG, "Failed to create mesh task");
        return ESP_ERR_NO_MEM;
    }
    
    ESP_LOGI(TAG, "Mesh network initialized");
    ESP_LOGI(TAG, "Device ID: %02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
             device_id[0], device_id[1], device_id[2], device_id[3],
             device_id[4], device_id[5], device_id[6], device_id[7]);
    
    return ESP_OK;
}

static void mesh_task(void *parameters)
{
    mesh_message_t message;
    TickType_t last_beacon_time = 0;
    const TickType_t beacon_interval = pdMS_TO_TICKS(MESH_BEACON_INTERVAL);
    
    while (1) {
        // Send beacon periodically
        if (xTaskGetTickCount() - last_beacon_time >= beacon_interval) {
            mesh_send_beacon();
            last_beacon_time = xTaskGetTickCount();
        }
        
        // Process outgoing messages
        if (xQueueReceive(tx_queue, &message, pdMS_TO_TICKS(100)) == pdPASS) {
            lora_send_message(&message);
            ESP_LOGD(TAG, "Sent message ID: %lu", message.id);
        }
        
        // Listen for incoming messages
        if (lora_receive_message(&message, 100) == ESP_OK) {
            mesh_handle_received_message(&message);
        }
        
        // Cleanup old routes
        mesh_cleanup_old_routes();
        
        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

esp_err_t mesh_send_text_message(const uint8_t *recipient_id, const char *text)
{
    if (strlen(text) > 255) {
        return ESP_ERR_INVALID_ARG;
    }
    
    mesh_message_t message = {0};
    message.id = mesh_generate_message_id();
    
    // Get current timestamp
    struct timeval tv;
    gettimeofday(&tv, NULL);
    message.timestamp = tv.tv_sec;
    
    memcpy(message.sender_id, device_id, 8);
    memcpy(message.recipient_id, recipient_id, 8);
    message.message_type = MSG_TYPE_TEXT;
    message.hop_count = MAX_HOP_COUNT;
    message.payload_length = strlen(text);
    strcpy((char*)message.payload, text);
    message.checksum = mesh_calculate_checksum(&message);
    
    // Add to TX queue
    if (xQueueSend(tx_queue, &message, pdMS_TO_TICKS(1000)) != pdPASS) {
        ESP_LOGE(TAG, "Failed to queue message");
        return ESP_ERR_TIMEOUT;
    }
    
    ESP_LOGI(TAG, "Queued text message: %s", text);
    return ESP_OK;
}

esp_err_t mesh_send_broadcast(const char *text)
{
    uint8_t broadcast_id[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    return mesh_send_text_message(broadcast_id, text);
}

static void mesh_handle_received_message(const mesh_message_t *message)
{
    // Verify checksum
    if (!mesh_verify_checksum(message)) {
        ESP_LOGW(TAG, "Message checksum verification failed");
        return;
    }
    
    // Check if it's a duplicate
    if (mesh_is_message_duplicate(message->id, message->sender_id)) {
        ESP_LOGD(TAG, "Duplicate message ignored");
        return;
    }
    
    // Check if message is for us or broadcast
    bool is_for_us = (memcmp(message->recipient_id, device_id, 8) == 0);
    bool is_broadcast = true;
    for (int i = 0; i < 8; i++) {
        if (message->recipient_id[i] != 0xFF) {
            is_broadcast = false;
            break;
        }
    }
    
    switch (message->message_type) {
        case MSG_TYPE_TEXT:
            if (is_for_us || is_broadcast) {
                ESP_LOGI(TAG, "Received message: %s", (char*)message->payload);
                if (message_callback) {
                    message_callback(message);
                }
                
                // Send ACK if not broadcast
                if (!is_broadcast) {
                    mesh_message_t ack = {0};
                    ack.id = mesh_generate_message_id();
                    struct timeval tv;
                    gettimeofday(&tv, NULL);
                    ack.timestamp = tv.tv_sec;
                    memcpy(ack.sender_id, device_id, 8);
                    memcpy(ack.recipient_id, message->sender_id, 8);
                    ack.message_type = MSG_TYPE_ACK;
                    ack.hop_count = MAX_HOP_COUNT;
                    memcpy(ack.payload, &message->id, sizeof(uint32_t));
                    ack.payload_length = sizeof(uint32_t);
                    ack.checksum = mesh_calculate_checksum(&ack);
                    
                    xQueueSend(tx_queue, &ack, 0);
                }
            } else if (message->hop_count > 0) {
                // Forward message
                mesh_message_t forward_msg = *message;
                forward_msg.hop_count--;
                xQueueSend(tx_queue, &forward_msg, 0);
                ESP_LOGD(TAG, "Forwarded message");
            }
            break;
            
        case MSG_TYPE_ACK:
            if (is_for_us) {
                uint32_t ack_msg_id;
                memcpy(&ack_msg_id, message->payload, sizeof(uint32_t));
                ESP_LOGI(TAG, "Received ACK for message %lu", ack_msg_id);
                if (message_callback) {
                    message_callback(message);
                }
            }
            break;
            
        case MSG_TYPE_BEACON:
            // Update route to sender
            mesh_add_route(message->sender_id, message->sender_id, 1);
            ESP_LOGD(TAG, "Updated route from beacon");
            break;
            
        default:
            ESP_LOGW(TAG, "Unknown message type: %d", message->message_type);
            break;
    }
}

static void mesh_send_beacon(void)
{
    mesh_message_t beacon = {0};
    beacon.id = mesh_generate_message_id();
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    beacon.timestamp = tv.tv_sec;
    
    memcpy(beacon.sender_id, device_id, 8);
    memset(beacon.recipient_id, 0xFF, 8);  // Broadcast
    beacon.message_type = MSG_TYPE_BEACON;
    beacon.hop_count = 1;  // Only direct neighbors
    beacon.payload_length = 0;
    beacon.checksum = mesh_calculate_checksum(&beacon);
    
    xQueueSend(tx_queue, &beacon, 0);
    ESP_LOGD(TAG, "Sent beacon");
}

static bool mesh_is_message_duplicate(uint32_t msg_id, const uint8_t *sender_id)
{
    // Simple duplicate detection - check last few messages
    for (int i = 0; i < MAX_MESSAGES; i++) {
        if (message_queue[i].id == msg_id && 
            memcmp(message_queue[i].sender_id, sender_id, 8) == 0) {
            return true;
        }
    }
    
    // Add to message history (circular buffer)
    static int msg_index = 0;
    message_queue[msg_index].id = msg_id;
    memcpy(message_queue[msg_index].sender_id, sender_id, 8);
    msg_index = (msg_index + 1) % MAX_MESSAGES;
    
    return false;
}

esp_err_t mesh_process(void)
{
    // This function is called from main loop
    // Most processing is done in the mesh_task
    return ESP_OK;
}

esp_err_t mesh_add_route(const uint8_t *destination, const uint8_t *next_hop, uint8_t hop_count)
{
    // Find existing route or empty slot
    route_entry_t *route = NULL;
    for (int i = 0; i < MAX_ROUTES; i++) {
        if (!route_table[i].active) {
            route = &route_table[i];
            break;
        } else if (memcmp(route_table[i].destination, destination, 8) == 0) {
            route = &route_table[i];
            break;
        }
    }
    
    if (route == NULL) {
        ESP_LOGW(TAG, "Route table full");
        return ESP_ERR_NO_MEM;
    }
    
    memcpy(route->destination, destination, 8);
    memcpy(route->next_hop, next_hop, 8);
    route->hop_count = hop_count;
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    route->timestamp = tv.tv_sec;
    route->active = true;
    
    ESP_LOGD(TAG, "Added/updated route to device");
    return ESP_OK;
}

esp_err_t mesh_remove_route(const uint8_t *destination)
{
    for (int i = 0; i < MAX_ROUTES; i++) {
        if (route_table[i].active && 
            memcmp(route_table[i].destination, destination, 8) == 0) {
            route_table[i].active = false;
            ESP_LOGD(TAG, "Removed route");
            return ESP_OK;
        }
    }
    return ESP_ERR_NOT_FOUND;
}

route_entry_t* mesh_find_route(const uint8_t *destination)
{
    for (int i = 0; i < MAX_ROUTES; i++) {
        if (route_table[i].active && 
            memcmp(route_table[i].destination, destination, 8) == 0) {
            return &route_table[i];
        }
    }
    return NULL;
}

void mesh_cleanup_old_routes(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t current_time = tv.tv_sec;
    
    for (int i = 0; i < MAX_ROUTES; i++) {
        if (route_table[i].active && 
            (current_time - route_table[i].timestamp) > MESSAGE_TIMEOUT / 1000) {
            route_table[i].active = false;
            ESP_LOGD(TAG, "Cleaned up old route");
        }
    }
}

uint32_t mesh_generate_message_id(void)
{
    return ++message_counter;
}

uint16_t mesh_calculate_checksum(const mesh_message_t *message)
{
    uint16_t checksum = 0;
    const uint8_t *data = (const uint8_t *)message;
    size_t len = sizeof(mesh_message_t) - sizeof(uint16_t);  // Exclude checksum field
    
    for (size_t i = 0; i < len; i++) {
        checksum += data[i];
    }
    
    return checksum;
}

bool mesh_verify_checksum(const mesh_message_t *message)
{
    mesh_message_t temp = *message;
    temp.checksum = 0;
    uint16_t calculated = mesh_calculate_checksum(&temp);
    return calculated == message->checksum;
}

void mesh_get_device_id(uint8_t *device_id_out)
{
    memcpy(device_id_out, device_id, 8);
}

void mesh_set_message_callback(mesh_message_callback_t callback)
{
    message_callback = callback;
}
