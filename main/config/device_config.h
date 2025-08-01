#ifndef DEVICE_CONFIG_H
#define DEVICE_CONFIG_H

// Device Information
#define DEVICE_NAME "MeshChat-ESP32"
#define DEVICE_VERSION "1.0.0"
#define MANUFACTURER_NAME "OpenMesh"

// LoRa Configuration
#define LORA_FREQUENCY          915000000  // 915 MHz (US) / 868000000 (EU)
#define LORA_TX_POWER           14         // dBm (max 14 for unlicensed)
#define LORA_SPREADING_FACTOR   7          // SF7-SF12 (higher = longer range, slower)
#define LORA_BANDWIDTH          125000     // 125 kHz
#define LORA_CODING_RATE        5          // 4/5
#define LORA_PREAMBLE_LENGTH    8          // symbols
#define LORA_SYNC_WORD          0x12       // Private network

// LoRa Pin Configuration (ESP32-S3)
#define LORA_SCK_PIN            18
#define LORA_MISO_PIN           19
#define LORA_MOSI_PIN           23
#define LORA_CS_PIN             5
#define LORA_RST_PIN            14
#define LORA_DIO0_PIN           27
#define LORA_DIO1_PIN           26

// Mesh Network Configuration
#define MAX_HOP_COUNT           10
#define MESH_BEACON_INTERVAL    30000      // ms
#define MESSAGE_TIMEOUT         300000     // 5 minutes
#define MAX_ROUTES              50
#define MAX_MESSAGES            100

// Bluetooth Configuration
#define BLE_DEVICE_NAME         "MeshChat"
#define BLE_SERVICE_UUID        "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define BLE_CHAR_TX_UUID        "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"  // Phone -> Device
#define BLE_CHAR_RX_UUID        "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"  // Device -> Phone

// Power Management
#define BATTERY_LOW_VOLTAGE     3200       // mV
#define SLEEP_TIMEOUT           300000     // 5 minutes of inactivity
#define DEEP_SLEEP_TIME         3600       // 1 hour deep sleep

// Message Types
typedef enum {
    MSG_TYPE_TEXT = 0x01,
    MSG_TYPE_ACK = 0x02,
    MSG_TYPE_ROUTE_REQUEST = 0x03,
    MSG_TYPE_ROUTE_REPLY = 0x04,
    MSG_TYPE_ROUTE_ERROR = 0x05,
    MSG_TYPE_BEACON = 0x06,
    MSG_TYPE_BROADCAST = 0x07
} message_type_t;

// Message Structure
typedef struct {
    uint32_t id;                    // Unique message ID
    uint64_t timestamp;             // Unix timestamp
    uint8_t sender_id[8];          // Device MAC address
    uint8_t recipient_id[8];       // Target device (broadcast if all 0xFF)
    message_type_t message_type;    // Message type
    uint8_t hop_count;             // TTL for mesh routing
    uint8_t payload_length;        // Actual payload length
    uint8_t payload[256];          // Message content
    uint16_t checksum;             // CRC16 error detection
} __attribute__((packed)) mesh_message_t;

// Route Table Entry
typedef struct {
    uint8_t destination[8];        // Destination device ID
    uint8_t next_hop[8];          // Next hop device ID
    uint8_t hop_count;            // Number of hops to destination
    uint64_t timestamp;           // Last updated
    bool active;                  // Route is active
} route_entry_t;

#endif // DEVICE_CONFIG_H
