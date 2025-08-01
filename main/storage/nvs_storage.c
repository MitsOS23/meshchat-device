#include "nvs_storage.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include <string.h>

static const char *TAG = "NVS_STORAGE";

// NVS namespaces
#define NVS_NAMESPACE_MESSAGES "messages"
#define NVS_NAMESPACE_CONFIG   "config"

static nvs_handle_t nvs_messages_handle;
static nvs_handle_t nvs_config_handle;
static bool nvs_initialized = false;

esp_err_t nvs_storage_init(void)
{
    esp_err_t ret;
    
    // Open NVS namespace for messages
    ret = nvs_open(NVS_NAMESPACE_MESSAGES, NVS_READWRITE, &nvs_messages_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle for messages: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Open NVS namespace for configuration
    ret = nvs_open(NVS_NAMESPACE_CONFIG, NVS_READWRITE, &nvs_config_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error opening NVS handle for config: %s", esp_err_to_name(ret));
        nvs_close(nvs_messages_handle);
        return ret;
    }
    
    nvs_initialized = true;
    ESP_LOGI(TAG, "NVS storage initialized");
    
    return ESP_OK;
}

esp_err_t nvs_storage_save_message(const mesh_message_t *message)
{
    if (!nvs_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    // Create a key based on message ID and timestamp
    char key[32];
    snprintf(key, sizeof(key), "msg_%lu_%llu", message->id, message->timestamp);
    
    esp_err_t ret = nvs_set_blob(nvs_messages_handle, key, message, sizeof(mesh_message_t));
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error saving message: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Commit to ensure data is written to flash
    ret = nvs_commit(nvs_messages_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error committing message: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGD(TAG, "Message saved with key: %s", key);
    return ESP_OK;
}

esp_err_t nvs_storage_load_messages(mesh_message_t *messages, size_t max_count, size_t *loaded_count)
{
    if (!nvs_initialized || !messages || !loaded_count) {
        return ESP_ERR_INVALID_ARG;
    }
    
    *loaded_count = 0;
    
    // Iterate through all keys in the messages namespace
    nvs_iterator_t it = nvs_entry_find(NVS_DEFAULT_PART_NAME, NVS_NAMESPACE_MESSAGES, NVS_TYPE_BLOB);
    
    while (it != NULL && *loaded_count < max_count) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        
        size_t required_size = sizeof(mesh_message_t);
        esp_err_t ret = nvs_get_blob(nvs_messages_handle, info.key, &messages[*loaded_count], &required_size);
        
        if (ret == ESP_OK && required_size == sizeof(mesh_message_t)) {
            (*loaded_count)++;
            ESP_LOGD(TAG, "Loaded message with key: %s", info.key);
        } else {
            ESP_LOGW(TAG, "Failed to load message with key: %s", info.key);
        }
        
        it = nvs_entry_next(it);
    }
    
    nvs_release_iterator(it);
    
    ESP_LOGI(TAG, "Loaded %d messages from NVS", *loaded_count);
    return ESP_OK;
}

esp_err_t nvs_storage_clear_messages(void)
{
    if (!nvs_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    esp_err_t ret = nvs_erase_all(nvs_messages_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error clearing messages: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = nvs_commit(nvs_messages_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error committing message clear: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGI(TAG, "All messages cleared from NVS");
    return ESP_OK;
}

esp_err_t nvs_storage_save_config(const char *key, const void *value, size_t length)
{
    if (!nvs_initialized || !key || !value) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = nvs_set_blob(nvs_config_handle, key, value, length);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error saving config %s: %s", key, esp_err_to_name(ret));
        return ret;
    }
    
    ret = nvs_commit(nvs_config_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error committing config %s: %s", key, esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGD(TAG, "Config saved: %s", key);
    return ESP_OK;
}

esp_err_t nvs_storage_load_config(const char *key, void *value, size_t *length)
{
    if (!nvs_initialized || !key || !value || !length) {
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = nvs_get_blob(nvs_config_handle, key, value, length);
    if (ret == ESP_ERR_NVS_NOT_FOUND) {
        ESP_LOGD(TAG, "Config not found: %s", key);
        return ret;
    } else if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error loading config %s: %s", key, esp_err_to_name(ret));
        return ret;
    }
    
    ESP_LOGD(TAG, "Config loaded: %s", key);
    return ESP_OK;
}
