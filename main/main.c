#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "device_config.h"
#include "lora.h"
#include "mesh.h"
#include "ble_server.h"
#include "power_mgmt.h"
#include "nvs_storage.h"

static const char *TAG = "MESHCHAT_MAIN";

void app_main(void)
{
    esp_err_t ret;

    // Initialize NVS
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_LOGI(TAG, "MeshChat Device Starting...");

    // Initialize power management
    power_mgmt_init();

    // Initialize NVS storage for messages
    nvs_storage_init();

    // Initialize LoRa radio
    if (lora_init() != ESP_OK) {
        ESP_LOGE(TAG, "LoRa initialization failed");
        return;
    }

    // Initialize mesh networking
    mesh_init();

    // Initialize Bluetooth LE
    ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));
    
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    ret = esp_bt_controller_init(&bt_cfg);
    if (ret) {
        ESP_LOGE(TAG, "initialize controller failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
    if (ret) {
        ESP_LOGE(TAG, "enable controller failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_init();
    if (ret) {
        ESP_LOGE(TAG, "init bluetooth failed: %s", esp_err_to_name(ret));
        return;
    }

    ret = esp_bluedroid_enable();
    if (ret) {
        ESP_LOGE(TAG, "enable bluetooth failed: %s", esp_err_to_name(ret));
        return;
    }

    // Initialize BLE GATT server
    ble_server_init();

    ESP_LOGI(TAG, "MeshChat Device Ready!");
    ESP_LOGI(TAG, "Device ID: %s", DEVICE_NAME);
    ESP_LOGI(TAG, "LoRa Frequency: %.1f MHz", LORA_FREQUENCY / 1000000.0);

    // Main application loop
    while (1) {
        // Process mesh network messages
        mesh_process();
        
        // Handle power management
        power_mgmt_update();
        
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
