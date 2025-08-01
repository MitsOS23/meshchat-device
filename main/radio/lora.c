#include "lora.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "LORA";

// SX127x Registers
#define REG_FIFO                 0x00
#define REG_OP_MODE              0x01
#define REG_FRF_MSB              0x06
#define REG_FRF_MID              0x07
#define REG_FRF_LSB              0x08
#define REG_PA_CONFIG            0x09
#define REG_LR_OCP               0x0B
#define REG_LNA                  0x0C
#define REG_FIFO_ADDR_PTR        0x0D
#define REG_FIFO_TX_BASE_ADDR    0x0E
#define REG_FIFO_RX_BASE_ADDR    0x0F
#define REG_FIFO_RX_CURRENT_ADDR 0x10
#define REG_IRQ_FLAGS_MASK       0x11
#define REG_IRQ_FLAGS            0x12
#define REG_RX_NB_BYTES          0x13
#define REG_PKT_RSSI_VALUE       0x1A
#define REG_PKT_SNR_VALUE        0x1B
#define REG_MODEM_CONFIG_1       0x1D
#define REG_MODEM_CONFIG_2       0x1E
#define REG_PREAMBLE_MSB         0x20
#define REG_PREAMBLE_LSB         0x21
#define REG_PAYLOAD_LENGTH       0x22
#define REG_MODEM_CONFIG_3       0x26
#define REG_RSSI_VALUE           0x1C
#define REG_DIO_MAPPING_1        0x40
#define REG_DIO_MAPPING_2        0x41
#define REG_VERSION              0x42

// Operating modes
#define MODE_LONG_RANGE_MODE     0x80
#define MODE_SLEEP               0x00
#define MODE_STDBY               0x01
#define MODE_TX                  0x03
#define MODE_RX_CONTINUOUS       0x05
#define MODE_RX_SINGLE           0x06

// IRQ flags
#define IRQ_TX_DONE_MASK         0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK 0x20
#define IRQ_RX_DONE_MASK         0x40

static spi_device_handle_t spi_handle;
static bool lora_initialized = false;

// SPI communication functions
static esp_err_t lora_write_register(uint8_t reg, uint8_t value)
{
    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = reg | 0x80,  // Write bit
        .length = 8,
        .rxlength = 0,
        .tx_buffer = &value,
        .rx_buffer = NULL
    };
    return spi_device_transmit(spi_handle, &trans);
}

static esp_err_t lora_read_register(uint8_t reg, uint8_t *value)
{
    spi_transaction_t trans = {
        .flags = 0,
        .cmd = 0,
        .addr = reg & 0x7F,  // Clear write bit
        .length = 8,
        .rxlength = 8,
        .tx_buffer = NULL,
        .rx_buffer = value
    };
    return spi_device_transmit(spi_handle, &trans);
}

esp_err_t lora_init(void)
{
    esp_err_t ret;

    // Configure GPIO pins
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << LORA_RST_PIN),
        .pull_down_en = 0,
        .pull_up_en = 0,
    };
    gpio_config(&io_conf);

    // Configure DIO pins as input
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << LORA_DIO0_PIN) | (1ULL << LORA_DIO1_PIN);
    gpio_config(&io_conf);

    // Configure SPI
    spi_bus_config_t buscfg = {
        .miso_io_num = LORA_MISO_PIN,
        .mosi_io_num = LORA_MOSI_PIN,
        .sclk_io_num = LORA_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = 256,
    };

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 1000000,  // 1 MHz
        .mode = 0,
        .spics_io_num = LORA_CS_PIN,
        .queue_size = 7,
        .address_bits = 8,
    };

    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI bus initialization failed");
        return ret;
    }

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SPI device add failed");
        return ret;
    }

    // Reset LoRa module
    gpio_set_level(LORA_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(LORA_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    // Check version
    uint8_t version;
    ret = lora_read_register(REG_VERSION, &version);
    if (ret != ESP_OK || version != 0x12) {
        ESP_LOGE(TAG, "LoRa module not found (version: 0x%02X)", version);
        return ESP_ERR_NOT_FOUND;
    }

    // Put in sleep mode
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);

    // Set frequency
    uint64_t frf = ((uint64_t)LORA_FREQUENCY << 19) / 32000000;
    lora_write_register(REG_FRF_MSB, (uint8_t)(frf >> 16));
    lora_write_register(REG_FRF_MID, (uint8_t)(frf >> 8));
    lora_write_register(REG_FRF_LSB, (uint8_t)(frf >> 0));

    // Set TX power
    lora_set_power(LORA_TX_POWER);

    // Set spreading factor, bandwidth, and coding rate
    lora_write_register(REG_MODEM_CONFIG_1, 
        ((LORA_BANDWIDTH == 125000) ? 0x70 : 0x80) | 
        ((LORA_CODING_RATE - 4) << 1));
    
    lora_write_register(REG_MODEM_CONFIG_2, 
        (LORA_SPREADING_FACTOR << 4) | 0x04);

    // Set preamble length
    lora_write_register(REG_PREAMBLE_MSB, (LORA_PREAMBLE_LENGTH >> 8) & 0xFF);
    lora_write_register(REG_PREAMBLE_LSB, LORA_PREAMBLE_LENGTH & 0xFF);

    // Set sync word
    lora_write_register(0x39, LORA_SYNC_WORD);

    // Set FIFO base addresses
    lora_write_register(REG_FIFO_TX_BASE_ADDR, 0);
    lora_write_register(REG_FIFO_RX_BASE_ADDR, 0);

    // Put in standby mode
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

    lora_initialized = true;
    ESP_LOGI(TAG, "LoRa initialized successfully (version: 0x%02X)", version);
    
    return ESP_OK;
}

esp_err_t lora_send_message(const mesh_message_t *message)
{
    if (!lora_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    // Calculate total message size
    size_t msg_size = sizeof(mesh_message_t);
    
    // Put in standby mode
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

    // Set FIFO address pointer
    lora_write_register(REG_FIFO_ADDR_PTR, 0);

    // Write message to FIFO
    const uint8_t *data = (const uint8_t *)message;
    for (size_t i = 0; i < msg_size; i++) {
        lora_write_register(REG_FIFO, data[i]);
    }

    // Set payload length
    lora_write_register(REG_PAYLOAD_LENGTH, msg_size);

    // Start transmission
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

    // Wait for TX done (with timeout)
    uint8_t irq_flags;
    int timeout = 10000;  // 10 seconds
    do {
        lora_read_register(REG_IRQ_FLAGS, &irq_flags);
        vTaskDelay(pdMS_TO_TICKS(1));
        timeout--;
    } while (!(irq_flags & IRQ_TX_DONE_MASK) && timeout > 0);

    // Clear IRQ flags
    lora_write_register(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);

    // Put back in standby
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

    if (timeout <= 0) {
        ESP_LOGE(TAG, "TX timeout");
        return ESP_ERR_TIMEOUT;
    }

    ESP_LOGD(TAG, "Message sent successfully");
    return ESP_OK;
}

esp_err_t lora_receive_message(mesh_message_t *message, uint32_t timeout_ms)
{
    if (!lora_initialized) {
        return ESP_ERR_INVALID_STATE;
    }

    // Put in continuous RX mode
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_CONTINUOUS);

    uint8_t irq_flags;
    uint32_t start_time = xTaskGetTickCount();
    
    // Wait for RX done or timeout
    do {
        lora_read_register(REG_IRQ_FLAGS, &irq_flags);
        vTaskDelay(pdMS_TO_TICKS(10));
    } while (!(irq_flags & IRQ_RX_DONE_MASK) && 
             (xTaskGetTickCount() - start_time) < pdMS_TO_TICKS(timeout_ms));

    if (!(irq_flags & IRQ_RX_DONE_MASK)) {
        lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
        return ESP_ERR_TIMEOUT;
    }

    // Check for CRC error
    if (irq_flags & IRQ_PAYLOAD_CRC_ERROR_MASK) {
        lora_write_register(REG_IRQ_FLAGS, IRQ_RX_DONE_MASK | IRQ_PAYLOAD_CRC_ERROR_MASK);
        lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
        ESP_LOGW(TAG, "CRC error in received message");
        return ESP_ERR_INVALID_CRC;
    }

    // Get received bytes count
    uint8_t rx_bytes;
    lora_read_register(REG_RX_NB_BYTES, &rx_bytes);

    if (rx_bytes != sizeof(mesh_message_t)) {
        lora_write_register(REG_IRQ_FLAGS, IRQ_RX_DONE_MASK);
        lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
        ESP_LOGW(TAG, "Received message size mismatch: %d", rx_bytes);
        return ESP_ERR_INVALID_SIZE;
    }

    // Set FIFO pointer to received message
    uint8_t rx_addr;
    lora_read_register(REG_FIFO_RX_CURRENT_ADDR, &rx_addr);
    lora_write_register(REG_FIFO_ADDR_PTR, rx_addr);

    // Read message from FIFO
    uint8_t *data = (uint8_t *)message;
    for (int i = 0; i < rx_bytes; i++) {
        lora_read_register(REG_FIFO, &data[i]);
    }

    // Clear IRQ flags
    lora_write_register(REG_IRQ_FLAGS, IRQ_RX_DONE_MASK);

    // Put back in standby
    lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);

    ESP_LOGD(TAG, "Message received successfully (%d bytes)", rx_bytes);
    return ESP_OK;
}

esp_err_t lora_set_power(int8_t power)
{
    if (power < 2) power = 2;
    if (power > 17) power = 17;

    lora_write_register(REG_PA_CONFIG, 0x80 | (power - 2));
    return ESP_OK;
}

esp_err_t lora_sleep(void)
{
    return lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

esp_err_t lora_wake(void)
{
    return lora_write_register(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

int lora_get_rssi(void)
{
    uint8_t rssi;
    lora_read_register(REG_PKT_RSSI_VALUE, &rssi);
    return -164 + rssi;  // Adjust for frequency > 525 MHz
}

float lora_get_snr(void)
{
    uint8_t snr;
    lora_read_register(REG_PKT_SNR_VALUE, &snr);
    return ((int8_t)snr) * 0.25;
}
