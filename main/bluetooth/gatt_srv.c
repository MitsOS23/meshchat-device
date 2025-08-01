#include "gatt_srv.h"
#include "device_config.h"
#include "esp_log.h"
#include "esp_gatt_common_api.h"
#include <string.h>

static const char *TAG = "GATT_SRV";

// Service and characteristic handles
static uint16_t meshchat_service_handle;
static uint16_t meshchat_char_tx_handle;
static uint16_t meshchat_char_rx_handle;
static uint16_t meshchat_char_tx_val_handle;
static uint16_t meshchat_char_rx_val_handle;

// UUIDs converted from strings
static uint8_t service_uuid[16] = {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x01, 0x00, 0x40, 0x6E};
static uint8_t char_tx_uuid[16] = {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x02, 0x00, 0x40, 0x6E};
static uint8_t char_rx_uuid[16] = {0x9E, 0xCA, 0xDC, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x03, 0x00, 0x40, 0x6E};

static ble_data_callback_t data_rx_callback = NULL;

// Service definition
static const uint16_t GATTS_SERVICE_UUID_MESHCHAT = 0x00FF;
static const uint16_t GATTS_CHAR_UUID_TX = 0xFF01;
static const uint16_t GATTS_CHAR_UUID_RX = 0xFF02;

static const uint16_t primary_service_uuid = ESP_GATT_UUID_PRI_SERVICE;
static const uint16_t character_declaration_uuid = ESP_GATT_UUID_CHAR_DECLARE;
static const uint16_t character_client_config_uuid = ESP_GATT_UUID_CHAR_CLIENT_CONFIG;

static const uint8_t char_prop_read_write_notify = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE | ESP_GATT_CHAR_PROP_BIT_NOTIFY;
static const uint8_t char_prop_read_write = ESP_GATT_CHAR_PROP_BIT_READ | ESP_GATT_CHAR_PROP_BIT_WRITE;

// Attribute database
static const esp_gatts_attr_db_t gatt_db[7] = {
    // Service Declaration
    [0] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&primary_service_uuid, ESP_GATT_PERM_READ,
         sizeof(service_uuid), sizeof(service_uuid), (uint8_t *)service_uuid}
    },

    // TX Characteristic Declaration
    [1] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
         sizeof(char_prop_read_write_notify), sizeof(char_prop_read_write_notify), (uint8_t *)&char_prop_read_write_notify}
    },

    // TX Characteristic Value
    [2] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)char_tx_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         256, 0, NULL}
    },

    // TX Client Characteristic Configuration Descriptor
    [3] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_client_config_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         sizeof(uint16_t), 0, NULL}
    },

    // RX Characteristic Declaration  
    [4] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)&character_declaration_uuid, ESP_GATT_PERM_READ,
         sizeof(char_prop_read_write), sizeof(char_prop_read_write), (uint8_t *)&char_prop_read_write}
    },

    // RX Characteristic Value
    [5] = {
        {ESP_GATT_AUTO_RSP},
        {ESP_UUID_LEN_16, (uint8_t *)char_rx_uuid, ESP_GATT_PERM_READ | ESP_GATT_PERM_WRITE,
         256, 0, NULL}
    },
};

esp_err_t gatt_server_init(void)
{
    esp_err_t ret = esp_ble_gatts_app_register(0);
    if (ret) {
        ESP_LOGE(TAG, "gatts app register error, error code = %x", ret);
        return ret;
    }

    ret = esp_ble_gatt_set_local_mtu(500);
    if (ret) {
        ESP_LOGE(TAG, "set local MTU failed, error code = %x", ret);
    }

    ESP_LOGI(TAG, "GATT server initialized");
    return ESP_OK;
}

esp_err_t gatt_server_create_service(esp_gatt_if_t gatts_if)
{
    esp_err_t ret = esp_ble_gatts_create_attr_tab(gatt_db, gatts_if, 
                                                  sizeof(gatt_db) / sizeof(gatt_db[0]), 0);
    if (ret) {
        ESP_LOGE(TAG, "create attr table failed, error code = %x", ret);
        return ret;
    }

    ESP_LOGI(TAG, "GATT service creation initiated");
    return ESP_OK;
}

void gatt_server_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param)
{
    switch (event) {
        case ESP_GATTS_CREAT_ATTR_TAB_EVT: {
            if (param->add_attr_tab.status != ESP_GATT_OK) {
                ESP_LOGE(TAG, "create attribute table failed, error code=0x%x", param->add_attr_tab.status);
                return;
            }
            
            if (param->add_attr_tab.num_handle != sizeof(gatt_db) / sizeof(gatt_db[0])) {
                ESP_LOGE(TAG, "create attribute table abnormally, num_handle (%d) doesn't equal to HRS_IDX_NB(%d)",
                         param->add_attr_tab.num_handle, sizeof(gatt_db) / sizeof(gatt_db[0]));
                return;
            }
            
            ESP_LOGI(TAG, "create attribute table successfully, the number handle = %d", param->add_attr_tab.num_handle);
            
            // Store handles
            meshchat_service_handle = param->add_attr_tab.handles[0];
            meshchat_char_tx_handle = param->add_attr_tab.handles[1];
            meshchat_char_tx_val_handle = param->add_attr_tab.handles[2];
            meshchat_char_rx_handle = param->add_attr_tab.handles[4];
            meshchat_char_rx_val_handle = param->add_attr_tab.handles[5];
            
            esp_ble_gatts_start_service(meshchat_service_handle);
            break;
        }
        
        case ESP_GATTS_START_EVT:
            ESP_LOGI(TAG, "SERVICE_START_EVT, status %d, service_handle %d", 
                     param->start.status, param->start.service_handle);
            break;
            
        case ESP_GATTS_WRITE_EVT:
            if (!param->write.is_prep) {
                ESP_LOGI(TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", 
                         param->write.handle, param->write.len);
                
                // Check if this is data written to RX characteristic
                if (param->write.handle == meshchat_char_rx_val_handle && data_rx_callback) {
                    // Null-terminate the received data
                    char received_data[257] = {0};
                    if (param->write.len < 256) {
                        memcpy(received_data, param->write.value, param->write.len);
                        received_data[param->write.len] = '\0';
                        
                        ESP_LOGI(TAG, "Received data: %s", received_data);
                        data_rx_callback(received_data, param->write.len);
                    }
                }
                
                if (param->write.need_rsp) {
                    esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
                }
            } else {
                ESP_LOGI(TAG, "GATT_WRITE_EVT, handle = %d, value len = %d, value :", 
                         param->write.handle, param->write.len);
            }
            break;
            
        case ESP_GATTS_EXEC_WRITE_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_EXEC_WRITE_EVT");
            esp_ble_gatts_send_response(gatts_if, param->write.conn_id, param->write.trans_id, ESP_GATT_OK, NULL);
            break;
            
        case ESP_GATTS_MTU_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_MTU_EVT, MTU %d", param->mtu.mtu);
            break;
            
        case ESP_GATTS_CONF_EVT:
            ESP_LOGI(TAG, "ESP_GATTS_CONF_EVT, status = %d, attr_handle %d", 
                     param->conf.status, param->conf.handle);
            break;
            
        default:
            break;
    }
}

esp_err_t gatt_server_send_notification(esp_gatt_if_t gatts_if, uint16_t conn_id, const char *data, size_t length)
{
    if (length > 256) {
        ESP_LOGW(TAG, "Data too long for notification: %d bytes", length);
        return ESP_ERR_INVALID_ARG;
    }
    
    esp_err_t ret = esp_ble_gatts_send_indicate(gatts_if, conn_id, meshchat_char_tx_val_handle,
                                                length, (uint8_t*)data, false);
    if (ret) {
        ESP_LOGE(TAG, "send notification failed, error code = %x", ret);
        return ret;
    }
    
    ESP_LOGD(TAG, "Sent notification: %.*s", (int)length, data);
    return ESP_OK;
}

void gatt_server_set_data_callback(ble_data_callback_t callback)
{
    data_rx_callback = callback;
}
