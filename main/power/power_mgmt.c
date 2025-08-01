#include "power_mgmt.h"
#include "device_config.h"
#include "esp_log.h"
#include "esp_sleep.h"
#include "esp_pm.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <sys/time.h>

static const char *TAG = "POWER_MGMT";

static esp_adc_cal_characteristics_t *adc_chars;
static uint64_t last_activity_time = 0;
static bool power_mgmt_initialized = false;

// ADC configuration for battery monitoring
#define DEFAULT_VREF    1100        // Use adc2_vref_to_gpio() to obtain a better estimate
#define NO_OF_SAMPLES   64          // Multisampling

esp_err_t power_mgmt_init(void)
{
    esp_err_t ret;
    
    // Configure power management
    esp_pm_config_esp32s3_t pm_config = {
        .max_freq_mhz = 240,
        .min_freq_mhz = 10,
        .light_sleep_enable = true
    };
    
    ret = esp_pm_configure(&pm_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to configure power management: %s", esp_err_to_name(ret));
        return ret;
    }
    
    // Configure ADC for battery voltage monitoring
    adc1_config_width(ADC_WIDTH_BIT_12);
    adc1_config_channel_atten(ADC1_CHANNEL_0, ADC_ATTEN_DB_11);  // GPIO1 for battery monitoring
    
    // Characterize ADC
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, DEFAULT_VREF, adc_chars);
    
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
        ESP_LOGI(TAG, "Using eFuse Vref for ADC calibration");
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {
        ESP_LOGI(TAG, "Using Two Point Value for ADC calibration");
    } else {
        ESP_LOGI(TAG, "Using Default Vref for ADC calibration");
    }
    
    // Set initial activity time
    struct timeval tv;
    gettimeofday(&tv, NULL);
    last_activity_time = (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
    
    power_mgmt_initialized = true;
    ESP_LOGI(TAG, "Power management initialized");
    
    return ESP_OK;
}

esp_err_t power_mgmt_update(void)
{
    if (!power_mgmt_initialized) {
        return ESP_ERR_INVALID_STATE;
    }
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    uint64_t current_time = (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
    
    // Check if we should enter sleep mode due to inactivity
    if ((current_time - last_activity_time) > (SLEEP_TIMEOUT * 1000)) {
        ESP_LOGI(TAG, "Entering light sleep due to inactivity");
        power_mgmt_sleep(30000);  // Sleep for 30 seconds
    }
    
    // Check battery level
    if (power_mgmt_is_battery_low()) {
        ESP_LOGW(TAG, "Battery low! Consider deep sleep or charging");
        // Could implement automatic deep sleep here
    }
    
    return ESP_OK;
}

esp_err_t power_mgmt_sleep(uint32_t duration_ms)
{
    ESP_LOGI(TAG, "Entering light sleep for %lu ms", duration_ms);
    
    // Configure wake-up sources
    esp_sleep_enable_timer_wakeup(duration_ms * 1000);  // Convert to microseconds
    
    // Enter light sleep
    esp_err_t ret = esp_light_sleep_start();
    
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Woke up from light sleep");
        power_mgmt_activity_notify();  // Reset activity timer
    } else {
        ESP_LOGE(TAG, "Light sleep failed: %s", esp_err_to_name(ret));
    }
    
    return ret;
}

esp_err_t power_mgmt_deep_sleep(uint32_t duration_ms)
{
    ESP_LOGI(TAG, "Entering deep sleep for %lu ms", duration_ms);
    
    // Configure wake-up sources
    esp_sleep_enable_timer_wakeup(duration_ms * 1000);  // Convert to microseconds
    
    // Could add other wake-up sources like GPIO, touch, etc.
    // esp_sleep_enable_ext1_wakeup(BUTTON_PIN_BITMASK, ESP_EXT1_WAKEUP_ANY_HIGH);
    
    // Enter deep sleep (this function does not return)
    esp_deep_sleep_start();
    
    return ESP_OK;  // This line will never be reached
}

uint16_t power_mgmt_get_battery_voltage(void)
{
    if (!power_mgmt_initialized) {
        return 0;
    }
    
    uint32_t adc_reading = 0;
    
    // Multisampling
    for (int i = 0; i < NO_OF_SAMPLES; i++) {
        adc_reading += adc1_get_raw(ADC1_CHANNEL_0);
    }
    adc_reading /= NO_OF_SAMPLES;
    
    // Convert adc_reading to voltage in mV
    uint32_t voltage = esp_adc_cal_raw_to_voltage(adc_reading, adc_chars);
    
    // Assuming voltage divider (2:1) for battery monitoring
    // Adjust this based on your actual hardware
    voltage *= 2;
    
    return (uint16_t)voltage;
}

uint8_t power_mgmt_get_battery_percentage(void)
{
    uint16_t voltage = power_mgmt_get_battery_voltage();
    
    // Li-ion battery voltage range: 3200mV (0%) to 4200mV (100%)
    if (voltage >= 4200) return 100;
    if (voltage <= 3200) return 0;
    
    // Linear approximation
    return (uint8_t)((voltage - 3200) * 100 / (4200 - 3200));
}

bool power_mgmt_is_battery_low(void)
{
    return power_mgmt_get_battery_voltage() < BATTERY_LOW_VOLTAGE;
}

void power_mgmt_activity_notify(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    last_activity_time = (uint64_t)tv.tv_sec * 1000000 + tv.tv_usec;
}
