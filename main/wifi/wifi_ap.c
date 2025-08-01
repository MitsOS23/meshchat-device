#include "wifi_ap.h"
#include "device_config.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include <string.h>

static const char *TAG = "WIFI_AP";

#define WIFI_AP_SSID      "MeshChat-AP"
#define WIFI_AP_PASS      "meshchat123"
#define WIFI_AP_CHANNEL   1
#define WIFI_AP_MAX_CONN  4

static bool wifi_ap_initialized = false;
static bool wifi_ap_running = false;
static int connected_clients = 0;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                              int32_t event_id, void* event_data)
{
    if (event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" joined, AID=%d",
                 MAC2STR(event->mac), event->aid);
        connected_clients++;
    } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "Station "MACSTR" left, AID=%d",
                 MAC2STR(event->mac), event->aid);
        connected_clients--;
        if (connected_clients < 0) connected_clients = 0;
    }
}

esp_err_t wifi_ap_init(void)
{
    if (wifi_ap_initialized) {
        return ESP_OK;
    }
    
    esp_err_t ret;
    
    // Initialize networking stack
    ret = esp_netif_init();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize netif: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Create default event loop
    ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Failed to create event loop: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Create default WiFi AP netif
    esp_netif_create_default_wifi_ap();
    
    // Initialize WiFi with default config
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ret = esp_wifi_init(&cfg);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize WiFi: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Register event handlers
    ret = esp_event_handler_instance_register(WIFI_EVENT,
                                            ESP_EVENT_ANY_ID,
                                            &wifi_event_handler,
                                            NULL,
                                            NULL);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register WiFi event handler: %s", esp_err_to_name(ret));
        return ret;
    }
    
    wifi_ap_initialized = true;
    ESP_LOGI(TAG, "WiFi AP initialized");
    
    return ESP_OK;
}

esp_err_t wifi_ap_start(void)
{
    if (!wifi_ap_initialized) {
        esp_err_t ret = wifi_ap_init();
        if (ret != ESP_OK) {
            return ret;
        }
    }
    
    if (wifi_ap_running) {
        ESP_LOGW(TAG, "WiFi AP already running");
        return ESP_OK;
    }
    
    // Configure WiFi as AP
    wifi_config_t wifi_config = {
        .ap = {
            .ssid = WIFI_AP_SSID,
            .ssid_len = strlen(WIFI_AP_SSID),
            .channel = WIFI_AP_CHANNEL,
            .password = WIFI_AP_PASS,
            .max_connection = WIFI_AP_MAX_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    
    if (strlen(WIFI_AP_PASS) == 0) {
        wifi_config.ap.authmode = WIFI_AUTH_OPEN;
    }
    
    esp_err_t ret = esp_wifi_set_mode(WIFI_MODE_AP);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi mode: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set WiFi config: %s", esp_err_to_name(ret));
        return ret;
    }
    
    ret = esp_wifi_start();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start WiFi: %s", esp_err_to_name(ret));
        return ret;
    }
    
    wifi_ap_running = true;
    connected_clients = 0;
    
    ESP_LOGI(TAG, "WiFi AP started. SSID: %s, Password: %s, Channel: %d",
             WIFI_AP_SSID, WIFI_AP_PASS, WIFI_AP_CHANNEL);
    
    return ESP_OK;
}

esp_err_t wifi_ap_stop(void)
{
    if (!wifi_ap_running) {
        ESP_LOGW(TAG, "WiFi AP not running");
        return ESP_OK;
    }
    
    esp_err_t ret = esp_wifi_stop();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop WiFi: %s", esp_err_to_name(ret));
        return ret;
    }
    
    wifi_ap_running = false;
    connected_clients = 0;
    
    ESP_LOGI(TAG, "WiFi AP stopped");
    return ESP_OK;
}

bool wifi_ap_is_running(void)
{
    return wifi_ap_running;
}

int wifi_ap_get_client_count(void)
{
    return connected_clients;
}
