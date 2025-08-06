# MeshChat - Off-Grid Messaging Device

A battery-powered mesh networking device that enables messaging without cellular towers using LoRa radio and Bluetooth Low Energy for phone interface.

## Interface Options

### Option A: Native iPhone App (React Native) - **RECOMMENDED**

#### **Why Native App is Better:**
- **📱 App Store Distribution**: Easy installation via official App Store
- **🔔 Real Push Notifications**: Background notifications even when app is closed
- **⚡ Better Performance**: Native components, smoother animations
- **🔋 Battery Optimization**: iOS-optimized background processing
- **📍 Enhanced GPS Access**: More reliable location services integration
- **🔐 Secure Storage**: iOS Keychain for message encryption keys
- **📱 Native UI Components**: Feels like built-in iOS app
- **🔄 Auto-Updates**: Seamless updates via App Store

#### **Native App Setup:**
1. **📲 Download from App Store**: "MeshChat" app (one-time install)
2. **🔵 Enable Bluetooth**: App automatically discovers nearby devices
3. **📡 Connect to Device**: Tap device in list to pair
4. **💬 Start Messaging**: Instant mesh communication
5. **📍 Share Location**: Native GPS integration with privacy controls

#### **React Native Architecture:**
```
MeshChat iOS App/
├── src/
│   ├── components/
│   │   ├── ChatScreen.tsx         # Main chat interface
│   │   ├── MapView.tsx           # Native map with device positions
│   │   ├── DeviceList.tsx        # Bluetooth device discovery
│   │   └── SettingsScreen.tsx    # App configuration
│   ├── services/
│   │   ├── BluetoothManager.ts   # iOS Bluetooth Low Energy
│   │   ├── LocationService.ts    # CoreLocation integration
│   │   ├── NotificationService.ts # Push notifications
│   │   └── MeshProtocol.ts       # Message handling
│   ├── utils/
│   │   ├── Encryption.ts         # Message encryption
│   │   └── Storage.ts            # AsyncStorage + Keychain
│   └── navigation/
│       └── AppNavigator.tsx      # React Navigation
├── ios/                          # Native iOS modules
│   ├── BluetoothModule.m         # Custom Bluetooth handling
│   └── LocationModule.m          # Enhanced GPS features
└── package.json                  # Dependencies
```

### Option B: Web-Based PWA (Current Implementation)

#### **Web App Setup:**
1. **📱 Connect to ESP32 WiFi**: "MeshChat-XXXX" (one-time setup)
2. **💾 Open web interface**: ESP32 serves at 192.168.4.1
3. **🔖 Bookmark the page**: Save for easy access
4. **📶 Disconnect WiFi**: Return phone to 4G mode
5. **🔵 Use Bluetooth**: Open bookmark, connect via Bluetooth
6. **💬 Pure mesh messaging**: Works forever after setup

#### **Why This Works Perfectly#### **User Interface Examples:**
```
┌─────────────────────────┐
│    📱 MeshChat Group    │
├─────────────────────────┤
│ 💬 Group Chat 📍 Users  │  ← Tab to switch views
├─────────────────────────┤
│                         │
│ 👥 4 people online      │
│                         │
│ Alice (150m ↗): "Hey    │  ← Messages from everyone
│ everyone, where should  │     in the group
│ we meet?"               │
│ 2 min ago              │
│                         │
│ Bob (300m ↙): "I found  │
│ the campsite! 📍"       │
│ 5 min ago              │
│                         │
│ You: "On my way!"       │
│ 8 min ago              │
│                         │
│ Dave (75m ↘): "I can    │
│ see Alice from here"    │
│ 1 min ago              │
│                         │
│ [Type message...]       │
├─────────────────────────┤
│ 🧭 Tap Users to see map │  ← Hint text
└─────────────────────────┘

📍 Users/Map View:
┌─────────────────────────┐
│  👥 Group Members       │
│                         │
│        ↑ N              │
│    W ← ⊕ → E           │  ← Your position  
│        ↓ S              │
│                         │
│ Alice 📍  150m ↗        │  ← Tap to navigate to Alice
│ Bob   📍  300m ↙        │  ← All group members shown
│ Dave  📍  75m  ↘        │     with distances
│                         │
│ [� Refresh Positions]   │
│ [� Back to Chat]       │
└─────────────────────────┘
``` the web app**: Interface stored in ESP32's own flash memory
- **No internet dependency**: ESP32 serves web app from its own storage
- **Survives phone restarts**: Bookmark always works, no cache issues
- **Phone stays on 4G**: Only connect to ESP32 WiFi when needed
- **Battery efficient**: Use Bluetooth for messaging, WiFi only for setupour Use Case):**
1. **📱 Use ESP32 Built-in Web Server**: No internet dependency ever
2. **💾 ESP32 serves web app**: Interface stored in ESP32's own flash memory
3. **🔵 Pure Bluetooth**: Phone stays on 4G, connects to ESP32 via Bluetooth only
4. **💬 Start messaging**: Direct device-to-phone communication, no servers needed

#### **Primary Mode: ESP32 Self-Contained (Recommended for Off-Grid)**
1. **Connect to ESP32 WiFi**: "MeshChat-XXXX" network (one-time setup)
2. **Browser opens web app**: ESP32 serves interface from its own memory at 192.168.4.1
3. **Save as bookmark**: Add to home screen for easy access
4. **Disconnect WiFi**: Return phone to 4G mode
5. **Open bookmarked page**: Works offline, connects via Bluetooth only
6. **Pure mesh messaging**: ESP32 ↔ Bluetooth ↔ Phone (survives phone restarts)g without cellular towers using LoRa radio and Bluetooth Low Energy for phone interface.

## Project Overview

**Range**: 2km line-of-sight  
**Interface**: Native iOS App (React Native) OR Web-based PWA via Bluetooth  
**Platform**: ESP32 + LoRa + C firmware (ESP-IDF)  
**Compatibility**: iOS native app OR iOS/Android via Web Bluetooth API  

### 🎯 **RECOMMENDATION: Native iOS App**

**For the best user experience, we recommend developing a native iOS app using React Native instead of the web-based PWA approach.**

#### **Why Native App is Superior:**
- **📱 App Store Distribution**: Professional installation and updates
- **🔔 Real Push Notifications**: Messages arrive even when app is closed
- **⚡ Superior Performance**: 60fps native UI, instant startup
- **🔋 Better Battery Life**: iOS-optimized background processing
- **📍 Enhanced GPS**: More reliable and accurate location services
- **🔐 Secure Storage**: iOS Keychain for encryption keys
- **🎨 Native UI**: Feels like built-in iOS app
- **🔄 Seamless Updates**: Auto-updates via App Store

#### **Web App Still Available:**
The web-based PWA remains as a backup option for cross-platform compatibility and immediate deployment without App Store approval.  

## Hardware Specifications

### Core Components
- **MCU**: ESP32-S3 (WiFi/Bluetooth + dual-core)
- **Radio**: SX1276/SX1278 LoRa module (868MHz/915MHz)
- **Battery**: 18650 Li-ion 3000mAh + solar charging
- **Antenna**: 868/915MHz with 3-5dBi gain
- **Display Options**:
  - **Phone Screen Mode**: Use phone as display when aligned on top
  - **Optional OLED**: 128x64 for standalone operation
- **Sensors**: 
  - LSM9DS1 (9-DOF: accelerometer, gyroscope, magnetometer)
  - **GPS via Phone**: Uses phone's built-in GPS for absolute positioning
  - Optional barometric pressure sensor for altitude
- **Enclosure**: Designed for phone mounting with alignment guides

### Display Modes
- **Phone-as-Display**: Device compass shown on phone screen via web app
- **Physical Alignment**: Device designed with phone cradle/mount
- **Sensor Fusion**: Phone + device sensors combined for better accuracy
- **Standalone Mode**: Optional OLED for operation without phone

### Power Management
- **Active Power**: ~100mA (transmitting)
- **Sleep Power**: ~10µA (deep sleep)
- **Solar Panel**: 5W for continuous operation
- **Battery Life**: 7+ days without solar

#### **Sleep/Wake Behavior:**
- **Auto Sleep**: Device enters deep sleep after 30 seconds of inactivity
- **Wake Triggers**: 
  - LoRa message received (DIO0 interrupt)
  - Bluetooth connection from phone
  - Timer wake-up every 30 seconds for mesh beacons
  - Physical button press (optional)
- **Sleep States**:
  - **Light Sleep**: CPU paused, radios active (~20mA)
  - **Deep Sleep**: Only RTC active, radios off (~10µA)
  - **Hibernation**: Minimal power, requires reset to wake (~5µA)

## Software Architecture

### Firmware (C/ESP-IDF) - Same for Both Interfaces
```
main/
├── main.c                # Main application entry
├── radio/
│   ├── lora.c/.h        # LoRa driver (SX127x)
│   └── mesh.c/.h        # Mesh networking logic
├── bluetooth/
│   ├── ble_server.c/.h  # Bluetooth Low Energy service
│   └── gatt_srv.c/.h    # GATT server for messaging
├── gps/
│   └── gps_handler.c/.h # Phone GPS coordinate processing
├── power/
│   └── power_mgmt.c/.h  # Sleep modes and power optimization
├── storage/
│   └── nvs_storage.c/.h # NVS message storage and routing
├── wifi/
│   └── wifi_ap.c/.h     # WiFi hotspot (backup interface - PWA only)
└── config/
    └── device_config.h  # Device configuration and settings
```

### Option A: Native iOS App (React Native)
```
MeshChat-iOS/
├── package.json              # Dependencies and scripts
├── metro.config.js           # React Native bundler config
├── ios/
│   ├── MeshChat.xcodeproj   # Xcode project
│   ├── MeshChat/
│   │   ├── Info.plist       # iOS app configuration
│   │   ├── AppDelegate.m    # iOS app lifecycle
│   │   └── main.m           # iOS entry point
│   └── Modules/
│       ├── BluetoothModule.m # Native Bluetooth LE integration
│       └── LocationModule.m  # Enhanced GPS services
├── src/
│   ├── App.tsx              # Main React Native component
│   ├── components/
│   │   ├── ChatScreen.tsx   # Main messaging interface
│   │   ├── MapScreen.tsx    # Native map with real GPS
│   │   ├── DeviceList.tsx   # Bluetooth device discovery
│   │   ├── SettingsScreen.tsx # Configuration & preferences
│   │   └── common/
│   │       ├── Button.tsx   # Reusable UI components
│   │       └── TextInput.tsx
│   ├── services/
│   │   ├── BluetoothManager.ts # iOS BLE central manager
│   │   ├── LocationService.ts  # CoreLocation wrapper
│   │   ├── NotificationService.ts # Push notifications
│   │   ├── MeshProtocol.ts     # Message encoding/decoding
│   │   └── EncryptionService.ts # End-to-end encryption
│   ├── store/
│   │   ├── messagesSlice.ts # Redux state management
│   │   ├── devicesSlice.ts  # Connected devices state
│   │   └── settingsSlice.ts # App preferences
│   ├── utils/
│   │   ├── Storage.ts       # AsyncStorage + Keychain
│   │   ├── Encryption.ts    # Message encryption utils
│   │   └── Positioning.ts   # RSSI triangulation algorithms
│   └── navigation/
│       └── AppNavigator.tsx # React Navigation stack
└── assets/
    ├── icons/              # App icons and UI elements
    └── sounds/             # Notification sounds
```

### Option B: Web Frontend (PWA)
```
web/
├── index.html           # Main chat interface with integrated positioning
├── manifest.json        # PWA configuration
├── service-worker.js    # Offline functionality
├── css/
│   └── style.css       # Mobile-optimized styling
└── js/
    ├── bluetooth.js     # Web Bluetooth API integration
    ├── gps.js          # Phone GPS integration for absolute positioning
    ├── maps.js         # Real map display with device locations
    ├── chat.js         # Chat UI with position indicators
    ├── positioning.js   # RSSI triangulation and compass logic
    └── mesh.js         # Mesh network visualization
```

## Power Management & Sleep Modes

### **How Sleep/Wake Works:**

#### **Automatic Sleep Logic:**
```c
// Device automatically enters sleep when:
// 1. No Bluetooth connection for 30 seconds
// 2. No LoRa activity for 30 seconds  
// 3. No user interaction for 30 seconds
```

#### **Wake-Up Sources:**
1. **LoRa Message Received**: 
   - **SX1276 DIO0 pin triggers GPIO interrupt** - LoRa chip wakes ESP32 instantly
   - **LoRa chip stays listening** even when ESP32 sleeps
   - ESP32 wakes instantly to process message
   - Handles mesh forwarding or delivers to user

2. **Bluetooth Connection**:
   - Phone attempts to connect via Web Bluetooth
   - ESP32 wakes to establish connection
   - Stays awake while phone is connected

3. **Periodic Timer**: 
   - Wakes every 30 seconds for mesh beacon
   - Announces presence to nearby devices
   - Updates routing table, then returns to sleep

4. **External Trigger** (optional):
   - Physical button press
   - Emergency wake-up capability
   - Manual status check

#### **Key Point: LoRa Chip Never Sleeps**
- **ESP32 sleeps, SX1276 stays awake**: LoRa radio continues listening for messages
- **Hardware interrupt wake-up**: DIO0 pin from SX1276 instantly wakes ESP32
- **< 1ms wake-up time**: Messages are never missed
- **Automatic message handling**: Device wakes, processes, forwards, then sleeps again
- **Mesh reliability**: All devices stay "reachable" even when sleeping

#### **How Message Reception Works:**
```c
// Power states during message reception:
1. ESP32 DEEP_SLEEP + SX1276 RX_MODE (listening)
2. Message arrives → SX1276 triggers DIO0 interrupt
3. ESP32 wakes in <1ms → reads message from SX1276
4. Process: decode, route, forward, or deliver to user
5. ESP32 returns to DEEP_SLEEP + SX1276 back to RX_MODE
```
```c
// Power management state machine:
ACTIVE → (30s timeout) → LIGHT_SLEEP → (no activity) → DEEP_SLEEP
  ↑                                                        ↓
  └─── (wake trigger) ←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←←┘
```

#### **Power Consumption by State:**
- **Active/Transmitting**: ~100mA (LoRa TX + ESP32 + Bluetooth)
- **Active/Receiving**: ~50mA (LoRa RX + ESP32 + Bluetooth)
- **Light Sleep**: ~20mA (radios on, CPU paused)
- **Deep Sleep**: ~10µA (only RTC timer active)
- **Hibernation**: ~5µA (minimal, requires manual reset)

#### **Smart Power Features:**
- **Activity Detection**: Monitors message frequency to adjust sleep timing
- **Solar Charging**: Wakes more frequently when solar power available
- **Low Battery Mode**: Increases sleep duration when battery < 20%
- **Emergency Mode**: Reduces power to essentials, extends range

### **Battery Optimization:**

#### **Mesh Network Efficiency:**
- **Beacon Timing**: Coordinates with neighbors to minimize overlapping transmissions
- **Route Caching**: Stores frequently used routes to reduce discovery overhead
- **Message Batching**: Combines multiple small messages into single transmission
- **Adaptive Power**: Reduces TX power for nearby devices, increases for distant ones

#### **Real-World Power Profile:**
```
Typical Day (Mixed Usage):
├── 95% Deep Sleep: ~10µA average
├── 4% Light Sleep/Processing: ~50mA average  
├── 1% Active Transmission: ~100mA average
└── Result: ~7-10 days battery life
```

## GPS Integration & Real Map Display

### **Phone GPS for Absolute Positioning**

#### **Why Phone GPS is Perfect:**
- **Always Available**: Every smartphone has GPS built-in
- **High Accuracy**: Modern phones get 3-5m accuracy
- **No Extra Hardware**: Saves $15+ vs dedicated GPS module
- **Better Antenna**: Phone GPS antennas are optimized
- **No Power Drain**: GPS runs on phone battery, not device battery
- **Works Offline**: GPS doesn't need internet, just satellites

#### **How GPS Integration Works:**

##### **Step 1: Phone Shares GPS with Device**
```javascript
// Web app requests GPS permission
navigator.geolocation.getCurrentPosition((position) => {
    const coords = {
        latitude: position.coords.latitude,
        longitude: position.coords.longitude,
        accuracy: position.coords.accuracy,
        timestamp: Date.now()
    };
    
    // Send GPS data to ESP32 via Bluetooth
    sendToDevice({
        type: 'gps_update',
        coordinates: coords
    });
});
```

##### **Step 2: Device Stores and Shares GPS Data**
```c
// ESP32 receives GPS from phone and shares with mesh
typedef struct {
    double latitude;           // GPS latitude
    double longitude;          // GPS longitude  
    float accuracy;            // GPS accuracy in meters
    uint64_t timestamp;        // When GPS was taken
    uint8_t device_id[8];     // Which device this belongs to
} gps_position_t;

// Share GPS position with mesh network
void share_gps_position(gps_position_t *pos) {
    mesh_message_t msg = {0};
    msg.message_type = MSG_TYPE_GPS_POSITION;
    memcpy(msg.payload, pos, sizeof(gps_position_t));
    msg.payload_length = sizeof(gps_position_t);
    mesh_send_broadcast(&msg);
}
```

##### **Step 3: Real Map Display**
```javascript
// Display all devices on real world map
function updateRealMap() {
    // Use Leaflet.js for offline maps
    const map = L.map('mapContainer').setView([lat, lng], 15);
    
    // Add offline map tiles (stored in PWA cache)
    L.tileLayer('offline-tiles/{z}/{x}/{y}.png').addTo(map);
    
    // Add device markers with real GPS coordinates
    devices.forEach(device => {
        if (device.gps) {
            L.marker([device.gps.latitude, device.gps.longitude])
             .bindPopup(`${device.name}<br>Accuracy: ${device.gps.accuracy}m`)
             .addTo(map);
        }
    });
}
```

### **Hybrid Positioning System**

#### **Three Positioning Modes:**

##### **Mode 1: GPS + RSSI (Best Accuracy)**
- **Primary**: Phone GPS for absolute position
- **Secondary**: RSSI for relative positioning when GPS unavailable
- **Use Case**: Outdoor activities, hiking, emergency response
- **Accuracy**: 3-5m absolute, real-time updates

##### **Mode 2: RSSI Only (Indoor/Underground)**
- **Primary**: RSSI triangulation between devices
- **Fallback**: When GPS signal is blocked
- **Use Case**: Indoor events, underground, dense urban areas
- **Accuracy**: 10-20m relative, improves over time

##### **Mode 3: Mixed GPS + RSSI (Maximum Coverage)**
- **Smart Switching**: GPS when available, RSSI when not
- **Data Fusion**: Combine both for best accuracy
- **Use Case**: Mixed indoor/outdoor environments
- **Accuracy**: Adaptive based on GPS availability

#### **Smart GPS Features:**

##### **Battery-Efficient GPS Usage**
```javascript
// Intelligent GPS collection - only when needed
class SmartGPS {
    constructor() {
        this.lastGPS = null;
        this.watchId = null;
        this.updateInterval = 30000; // 30 seconds default
    }
    
    startTracking() {
        // Adaptive update rate based on movement
        const options = {
            enableHighAccuracy: true,
            timeout: 10000,
            maximumAge: 30000
        };
        
        this.watchId = navigator.geolocation.watchPosition(
            (position) => this.handleGPSUpdate(position),
            (error) => this.handleGPSError(error),
            options
        );
    }
    
    handleGPSUpdate(position) {
        const coords = position.coords;
        
        // Only send update if position changed significantly
        if (this.hasMovedSignificantly(coords)) {
            this.sendGPSToDevice(coords);
            this.updateMapPosition(coords);
        }
        
        // Adjust update frequency based on speed
        this.adjustUpdateRate(coords);
    }
    
    hasMovedSignificantly(coords) {
        if (!this.lastGPS) return true;
        
        const distance = this.calculateDistance(
            this.lastGPS.latitude, this.lastGPS.longitude,
            coords.latitude, coords.longitude
        );
        
        return distance > 5; // 5 meter minimum change
    }
}
```

##### **Offline Map Support**
```javascript
// Cache map tiles for offline use
class OfflineMapManager {
    constructor() {
        this.mapCache = new Map();
        this.tileUrlTemplate = 'https://tile.openstreetmap.org/{z}/{x}/{y}.png';
    }
    
    async preloadMapArea(centerLat, centerLng, radiusKm) {
        const zoomLevels = [12, 13, 14, 15, 16]; // Different detail levels
        
        for (let zoom of zoomLevels) {
            const tiles = this.getTilesInRadius(centerLat, centerLng, radiusKm, zoom);
            
            for (let tile of tiles) {
                await this.cacheTile(tile.x, tile.y, tile.z);
            }
        }
    }
    
    async cacheTile(x, y, z) {
        const url = this.tileUrlTemplate.replace('{x}', x).replace('{y}', y).replace('{z}', z);
        const response = await fetch(url);
        const blob = await response.blob();
        
        // Store in browser cache for offline use
        const cache = await caches.open('map-tiles');
        await cache.put(url, new Response(blob));
    }
}
```

### **Real-World Map Interface**

#### **Map View Features:**
- **Satellite/Street Toggle**: Switch between map types
- **Device Clustering**: Group nearby devices for cleaner display
- **Trail History**: Show movement paths over time
- **Geofencing**: Set boundaries and alerts
- **Waypoint Sharing**: Mark and share points of interest

#### **Enhanced User Interface:**
```
┌─────────────────────────────────────┐
│  🗺️ Real World Map View            │
├─────────────────────────────────────┤
│                                     │
│    📍 You (GPS: 3m accuracy)       │
│    ┌─────────────────────────────┐  │
│    │     🛰️ Satellite View      │  │
│    │                            │  │
│    │  Alice 📱                  │  │ ← Real GPS positions
│    │   ↗ 150m                   │  │   on actual map
│    │                            │  │
│    │      📍 You               │  │
│    │                            │  │
│    │           Bob 📱           │  │
│    │            ↙ 300m         │  │
│    │                            │  │
│    │  Charlie 📱                │  │
│    │   ↘ 75m                   │  │
│    └─────────────────────────────┘  │
│                                     │
│ 🎯 GPS: Active (4 satellites)      │
│ 📡 Mesh: 3 devices connected       │
│ 🔋 Battery: 85% (6h remaining)     │
│                                     │
│ [📍] [💬] [⚙️] [🔍]              │ ← Navigation tabs
└─────────────────────────────────────┘
```

### **GPS Message Protocol Enhancement**

#### **Extended Message Structure:**
```c
typedef struct {
    uint32_t id;                    // Unique message ID
    uint64_t timestamp;             // Unix timestamp
    uint8_t sender_id[8];          // Device MAC address
    uint8_t recipient_id[8];       // Target device (broadcast if all 0xFF)
    message_type_t message_type;    // TEXT, GPS_POSITION, WAYPOINT, etc.
    uint8_t hop_count;             // TTL for mesh routing
    uint8_t payload[256];          // Message content or GPS data
    uint16_t checksum;             // CRC16 error detection
} __attribute__((packed)) mesh_message_t;

// GPS-specific message types
typedef enum {
    MSG_TYPE_TEXT = 1,
    MSG_TYPE_ACK = 2,
    MSG_TYPE_BEACON = 3,
    MSG_TYPE_GPS_POSITION = 4,     // GPS coordinates + accuracy
    MSG_TYPE_WAYPOINT = 5,         // Shared points of interest
    MSG_TYPE_EMERGENCY_GPS = 6,    // High-priority location sharing
    MSG_TYPE_GPS_REQUEST = 7       // Request GPS from specific device
} message_type_t;
```

### **Cost and Complexity Benefits**

#### **Using Phone GPS vs Dedicated Module:**
| Feature | Phone GPS | Dedicated GPS Module |
|---------|-----------|---------------------|
| **Cost** | $0 (free) | $15-25 |
| **Accuracy** | 3-5m | 3-10m |
| **Power Usage** | 0mA (device) | 25-50mA |
| **Antenna** | Optimized | Basic patch antenna |
| **AGPS Support** | Yes (faster locks) | No |
| **Indoor Performance** | Better | Poor |
| **Maintenance** | Auto-updates | Manual firmware |

#### **Phone GPS Advantages:**
- **Zero Power Drain**: GPS runs on phone, not device battery
- **Better Accuracy**: Phone GPS often has AGPS and multiple satellite systems
- **Always Updated**: Phone OS keeps GPS firmware current
- **Instant Lock**: Phones cache satellite data for faster GPS acquisition
- **Indoor Assistance**: Phones use WiFi/cell towers to improve GPS indoors

## Communication Protocol

### Message Structure
```c
typedef struct {
    uint32_t id;                    // Unique message ID
    uint64_t timestamp;             // Unix timestamp
    uint8_t sender_id[8];          // Device MAC address
    uint8_t recipient_id[8];       // Target device (broadcast if all 0xFF)
    message_type_t message_type;    // TEXT, ACK, ROUTE_REQUEST, etc.
    uint8_t hop_count;             // TTL for mesh routing
    uint8_t payload[256];          // Message content
    uint16_t checksum;             // CRC16 error detection
} __attribute__((packed)) mesh_message_t;
```

### Mesh Routing
- **Protocol**: AODV (Ad-hoc On-Demand Distance Vector)
- **Discovery**: Broadcast beacon every 30 seconds
- **Routing**: Dynamic route discovery and maintenance
- **Redundancy**: Multiple path support for reliability

### Network Capacity
- **Direct LoRa connections**: 50+ devices per node (limited by routing table)
- **Bluetooth connections**: 1 phone per device (Web Bluetooth limitation)
- **Maximum network size**: 200+ devices (theoretical mesh limit)
- **Practical network size**: 20-50 devices for optimal performance
- **Hop limit**: 10 hops maximum (configurable in device_config.h)

## User Experience & Setup

### **How It Actually Works**

#### **Off-Grid Setup (Your Use Case):**
1. **📱 Save web app locally**: Visit MeshChat web page once with internet
2. **� Browser caches everything**: PWA saves locally, works offline forever
3. **🔵 Pure Bluetooth**: Phone stays on 4G, connects to ESP32 via Bluetooth only
4. **💬 Start messaging**: Direct device-to-phone communication, no servers needed

#### **Primary Mode: Offline Bluetooth (Recommended for Off-Grid)**
1. **One-time setup**: Visit web app with internet to cache it locally
2. **Phone bookmark**: Save as PWA to home screen (works like native app)
3. **Go offline**: Turn off WiFi, keep 4G for other apps
4. **Open cached web app**: Works completely offline
5. **Click "Connect to MeshChat Device"** - connects via Bluetooth only
### **Deployment Comparison**

#### **Option A: Native iOS App (React Native) - RECOMMENDED**
```bash
# App Store deployment:
# 1. Download "MeshChat" from App Store
# 2. Enable Bluetooth permissions
# 3. App auto-discovers devices
# 4. Tap device to connect
# 5. Start messaging immediately
```

**Advantages:**
- ✅ **Easy Installation**: One-tap App Store download
- ✅ **Background Notifications**: Messages arrive even when app closed
- ✅ **Native Performance**: Smooth 60fps animations
- ✅ **Better GPS**: Enhanced location services integration
- ✅ **Secure Storage**: iOS Keychain for encryption keys
- ✅ **Auto-Updates**: Seamless updates via App Store
- ✅ **Professional UX**: Native iOS interface components

**Development Requirements:**
- React Native CLI
- Xcode for iOS compilation
- Apple Developer Account ($99/year)
- App Store approval process

#### **Option B: Web-Based PWA (Current Implementation)**
```bash
# ESP32-hosted web interface:
# 1. Connect phone to ESP32 WiFi: "MeshChat-XXXX"
# 2. Navigate to: http://192.168.4.1
# 3. Bookmark the page for easy access
# 4. Disconnect WiFi, return phone to 4G
# 5. Open bookmark anytime - ESP32 serves web app
# 6. Switch to Bluetooth mode for messaging
```

**Advantages:**
- ✅ **Zero Dependencies**: No app store, works on any phone
- ✅ **Self-Contained**: ESP32 hosts everything
- ✅ **Cross-Platform**: Works on iOS, Android, any browser
- ✅ **No Approval Process**: Deploy immediately
- ✅ **Offline-First**: Cached in browser, works without internet

**Limitations:**
- ⚠️ **Web Bluetooth Limitations**: iOS Safari restrictions
- ⚠️ **Background Processing**: Limited notifications when app not active
- ⚠️ **Performance**: Slower than native apps
- ⚠️ **Setup Complexity**: Initial WiFi connection required

#### **Option C: Local Development/Testing**
```bash
# For firmware development only
cd native-app && npx react-native run-ios
# For web testing: cd web && python -m http.server 8000
```

## User Interface Features

### Chat Interface
- **Group chat for all nearby devices** (like FB Messenger group)
- **Real-time messaging** via broadcast to all devices in range
- **Message history** stored locally and synced across devices
- **Online user list** with distance/direction indicators
- **GPS position sharing** with real-world map display
- **Emergency mode** for priority messages with GPS broadcast
- **Integrated Device Finder**: Tap any user to see their location on real map
- **Live Position Updates**: See where everyone is in real-time on satellite/street map
- **Compass Mode**: Navigate to any user using phone compass + GPS
- **Offline Maps**: Cached map tiles work without internet connection

### PWA Features
- **Install to home screen** like native app
- **Offline functionality** with service workers
- **Push notifications** for new messages
- **Dark/light theme** for battery saving

## Development Phases

### Phase 1: Core Firmware ✅
- [x] Project structure setup
- [x] LoRa driver implementation
- [ ] Basic point-to-point messaging
- [ ] BLE service setup
- [ ] Power management

### Phase 2: Mesh Networking 🔄
- [ ] AODV routing protocol
- [ ] Message flooding and relay
- [ ] Route discovery and maintenance
- [ ] Network topology mapping

### Phase 3A: Native iOS App (RECOMMENDED) 📱
- [ ] React Native project setup
- [ ] Native iOS BLE integration
- [ ] Enhanced location services (CoreLocation)
- [ ] Push notification system
- [ ] Native UI components
- [ ] App Store preparation
- [ ] Background processing optimization
- [ ] iOS system integration (Siri, Control Center)
- [ ] Haptic feedback and sound
- [ ] TestFlight beta distribution

### Phase 3B: Web Interface (Alternative) 🌐
- [ ] Web Bluetooth API integration
- [ ] Chat UI development
- [ ] PWA implementation
- [ ] Message synchronization
- [ ] Service worker for offline functionality

### Phase 4: Advanced Features (Both Interfaces) 🚀
- [ ] Group chat interface (primary feature)
- [ ] User discovery and online status
- [ ] Broadcast message distribution
- [ ] **GPS integration** - phone GPS for absolute positioning
- [ ] **Real map display** - satellite/street view with device positions
- [ ] **Offline map caching** - works without internet connection
- [ ] **Hybrid positioning** - GPS + RSSI for maximum accuracy
- [ ] Position sharing in group with real coordinates
- [ ] **RSSI triangulation** - backup positioning when GPS unavailable
- [ ] **Time-of-flight ranging** - precise distance measurement
- [ ] **Movement-based direction finding** - warmer/colder feedback
- [ ] **Multi-device positioning** - relative positions on map
- [ ] **Waypoint sharing** - mark and share points of interest
- [ ] **Emergency GPS broadcast** - high-priority location sharing

### Phase 5: Production & Distribution 📦
- [ ] **Native App**: App Store submission and approval
- [ ] **Web App**: CDN deployment for global access
- [ ] Hardware certification (FCC, CE)
- [ ] Manufacturing partnerships
- [ ] User documentation and support
- [ ] Community building and open source ecosystem

## Network Capacity & Limitations

### Connection Types

#### 1. **LoRa Mesh Network**
- **Direct neighbors**: 50+ devices (limited by route table size)
- **Total network**: 200+ devices theoretically
- **Practical limit**: 20-50 devices for best performance
- **Range**: 2km line-of-sight per hop
- **Extended range**: 20km+ through multi-hop routing

#### 2. **Bluetooth Phone Interface**
- **Connections per device**: 1 phone at a time
- **Range**: 10-30 meters
- **Multiple phones**: Each needs its own MeshChat device
- **Simultaneous users**: Unlimited (each phone connects to its own device)

### Scaling Considerations

#### **Network Performance vs Size**
- **1-10 devices**: Excellent performance, instant messaging
- **10-25 devices**: Good performance, <2 second latency  
- **25-50 devices**: Acceptable performance, 2-5 second latency
- **50+ devices**: Degraded performance, potential message loss

#### **Bandwidth Limitations**
- **LoRa data rate**: 0.3-50 kbps (depending on spread factor)
- **Message throughput**: ~10-50 messages/minute per device
- **Network saturation**: Occurs with >100 messages/minute total

#### **Memory Constraints (ESP32-S3)**
- **Flash Memory**: 16MB total available
  - **Firmware**: ~2MB (ESP-IDF + application code)
  - **Web Interface**: ~500KB (HTML/CSS/JS files)
  - **Configuration**: ~64KB (device settings, certificates)
  - **Available for messages**: ~13MB (message history storage)
- **RAM**: 512KB total
  - **Route table**: 50 entries (~10KB configurable)
  - **Message buffer**: 100 messages (~50KB configurable)  
  - **RAM usage**: ~200KB for mesh networking
  - **Available RAM**: ~250KB for other functions

#### **Flash Storage Breakdown:**
```c
ESP32-S3 Flash Layout (16MB):
├── 0x000000 - 0x010000: Bootloader (64KB)
├── 0x010000 - 0x020000: Partition Table (64KB)  
├── 0x020000 - 0x220000: Firmware (2MB)
├── 0x220000 - 0x2A0000: Web Interface (500KB)
├── 0x2A0000 - 0x2B0000: NVS Config (64KB)
├── 0x2B0000 - 0x1000000: Message Storage (13MB+)
└── Messages can store ~50,000 text messages
```

#### **Storage Capacity:**
- **Text Messages**: ~260 bytes each → ~50,000 messages in 13MB
- **Route Table**: Stores up to 50 device routes (expandable to 200)
- **Web Interface**: Complete PWA fits in 500KB
- **Firmware Updates**: OTA updates possible with dual partition scheme

### Real-World Deployment Examples

#### **Small Group (5-10 devices)**
- **Use case**: Family/team communication
- **Performance**: Instant messaging, full features
- **Range**: 10km+ with line-of-sight positioning

#### **Medium Community (20-30 devices)**  
- **Use case**: Neighborhood emergency network
- **Performance**: Good responsiveness, reliable delivery
- **Range**: City district coverage with strategic placement

#### **Large Network (50+ devices)**
- **Use case**: Disaster response, festival communications
- **Performance**: Slower but functional
- **Range**: Entire city coverage possible
- **Recommendation**: Use multiple smaller sub-networks

### Optimizing Network Size

#### **Configuration Adjustments**
```c
// In device_config.h - increase for larger networks
#define MAX_ROUTES              100  // Default: 50
#define MAX_MESSAGES            200  // Default: 100
#define MESH_BEACON_INTERVAL    60000  // Slower beacons for large networks
```

#### **Network Topology Tips**
- **Strategic placement**: High elevation devices as backbone
- **Hub devices**: Central nodes with more memory/power
- **Sub-networks**: Divide large areas into smaller meshes
- **Gateway devices**: Bridge between sub-networks

## Native App vs Web Interface Technical Comparison

### Bluetooth Low Energy Implementation

#### **Native iOS App Advantages:**
```typescript
// React Native with native iOS BLE modules
import { BleManager } from 'react-native-ble-plx';

class MeshChatBLE {
  manager: BleManager;
  
  constructor() {
    this.manager = new BleManager();
    
    // iOS-specific optimizations:
    this.manager.onStateChange((state) => {
      if (state === 'PoweredOn') {
        this.startScanning();
      }
    });
  }
  
  // Background scanning even when app is backgrounded
  async startScanning() {
    this.manager.startDeviceScan(
      ['6E400001-B5A3-F393-E0A9-E50E24DCCA9E'], // MeshChat service UUID
      { allowDuplicates: false },
      (error, device) => {
        if (device) {
          this.connectToDevice(device);
        }
      }
    );
  }
  
  // Native iOS notifications work in background
  async sendMessage(message: string) {
    const result = await this.writeCharacteristic(message);
    
    // Show iOS notification if app is backgrounded
    PushNotification.localNotification({
      title: "Message Sent",
      message: `Sent via mesh network`,
      playSound: true,
      soundName: 'default'
    });
  }
}
```

#### **Web App Limitations:**
```javascript
// Web Bluetooth API - limited by browser restrictions
navigator.bluetooth.requestDevice({
  filters: [{ services: ['6E400001-B5A3-F393-E0A9-E50E24DCCA9E'] }]
}).then(device => {
  // ⚠️ Connection lost when browser tab loses focus
  // ⚠️ No background processing on iOS Safari
  // ⚠️ Limited notification support
  // ⚠️ Must manually reconnect after phone sleep
});
```

### GPS and Location Services

#### **Native App Benefits:**
```typescript
// Enhanced iOS location services
import Geolocation from '@react-native-community/geolocation';

class LocationService {
  watchId: number;
  
  startTracking() {
    // Request always-location permission for background tracking
    this.watchId = Geolocation.watchPosition(
      (position) => {
        const { latitude, longitude, accuracy } = position.coords;
        
        // ✅ Works in background
        // ✅ More accurate (assisted GPS)
        // ✅ Better power management
        // ✅ Automatic fallback to WiFi/cellular positioning
        
        this.updateDevicePosition({ latitude, longitude, accuracy });
      },
      (error) => console.log(error),
      {
        enableHighAccuracy: true,
        distanceFilter: 5, // Only update if moved 5+ meters
        interval: 30000,   // Check every 30 seconds
        fastestInterval: 10000 // But not more than every 10 seconds
      }
    );
  }
}
```

#### **Web App GPS:**
```javascript
// Browser geolocation - more limited
navigator.geolocation.watchPosition(
  (position) => {
    // ⚠️ Stops working when browser backgrounded
    // ⚠️ Less accurate than native
    // ⚠️ No background location updates
    // ⚠️ iOS Safari requires user interaction to resume
  },
  { enableHighAccuracy: true }
);
```

### Push Notifications & Background Processing

#### **Native App Notifications:**
```typescript
// Full iOS notification support
import PushNotification from 'react-native-push-notification';

class NotificationService {
  configure() {
    PushNotification.configure({
      onNotification: (notification) => {
        // Handle notification tap - open to specific chat
        if (notification.userInteraction) {
          Navigation.navigate('ChatScreen', { 
            deviceId: notification.data.deviceId 
          });
        }
      },
      permissions: {
        alert: true,
        badge: true,
        sound: true,
      },
      popInitialNotification: true,
      requestPermissions: true,
    });
  }
  
  // Show notification even when app is closed
  showMessageNotification(message: MeshMessage) {
    PushNotification.localNotification({
      channelId: 'mesh-messages',
      title: `Message from ${message.senderName}`,
      message: message.text,
      bigText: message.text,
      playSound: true,
      soundName: 'default',
      badge: this.getUnreadCount(),
      userInfo: { deviceId: message.senderId },
      actions: ['Reply', 'View Location']
    });
  }
}
```

#### **Web App Notifications:**
```javascript
// Limited web notifications
if ('serviceWorker' in navigator) {
  navigator.serviceWorker.ready.then(registration => {
    // ⚠️ Only works when PWA is installed
    // ⚠️ iOS Safari very limited support
    // ⚠️ No background processing
    // ⚠️ Notifications often don't work reliably
    
    registration.showNotification('New Message', {
      body: message.text,
      icon: '/icon-192x192.png'
    });
  });
}
```

### Performance and User Experience

#### **Native App Advantages:**
- **Smooth Animations**: 60fps native UI components
- **Instant Startup**: App launches immediately from home screen
- **Native Navigation**: iOS-standard navigation patterns
- **System Integration**: Integrates with iOS Control Center, Siri, etc.
- **Offline Storage**: SQLite database for better performance
- **Memory Management**: iOS automatically manages memory

#### **Web App Limitations:**
- **Slower Performance**: JavaScript execution overhead
- **Browser Chrome**: URL bar and browser UI takes screen space
- **Limited Storage**: LocalStorage caps and clearing
- **No Deep Integration**: Can't integrate with iOS features
- **Connection Issues**: Must manually reconnect Bluetooth

## Building and Flashing

### Prerequisites

#### For Native iOS App:
- Xcode 14+ (macOS required)
- React Native CLI: `npm install -g @react-native-community/cli`
- CocoaPods: `sudo gem install cocoapods`
- Apple Developer Account ($99/year for App Store)
- iOS device for testing (Bluetooth required)

#### For Firmware (Both Options):
- ESP-IDF v5.0+ framework
- CMake build system
- Python 3.7+ (for ESP-IDF tools)

### Build Commands

#### Native iOS App:
```bash
# Setup React Native project
npx react-native init MeshChat --template react-native-template-typescript
cd MeshChat

# Install dependencies
npm install react-native-ble-plx @react-native-community/geolocation
npm install react-native-push-notification @reduxjs/toolkit
npm install react-native-maps react-navigation

# iOS-specific setup
cd ios && pod install && cd ..

# Run on iOS device/simulator
npx react-native run-ios --device "Your iPhone"

# Build for App Store
cd ios && xcodebuild -workspace MeshChat.xcworkspace -scheme MeshChat archive
```

#### Firmware (Same for Both):
```bash
# Setup ESP-IDF environment
. $HOME/esp/esp-idf/export.sh

# Configure for ESP32-S3
idf.py set-target esp32s3

# Build firmware
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash monitor
```

### Web Interface Deployment

#### **Production Use (No servers needed)**
```bash
# Option 1: Host on GitHub Pages/CDN
# - Upload web/ folder to any web hosting
# - Users access via any device with Bluetooth
# - Direct device connection, no backend needed

# Option 2: Use ESP32 built-in server
# - Web interface stored in ESP32 flash memory  
# - Connect to device WiFi: "MeshChat-XXXX"
# - Navigate to: http://192.168.4.1
```

#### **Development/Testing Only**
```bash
# Local testing server (not needed for production)
cd web && python -m http.server 8000
# Visit: http://localhost:8000
```

## Hardware Assembly

### Hardware Design (Pocket-Optimized)
```
📱 Phone (held in hand, upright)
│  ┌───────────────┐  
│  │  MeshChat App │  ← Phone compass works normally
│  │     ↗ ALICE   │  ← Phone shows direction using its compass
│  │   📶 Getting  │  ← RSSI feedback from pocket device
│  │     Warmer!   │  
│  └───────────────┘  
│
👤 Person walking
│
👖 Pocket (any orientation)
│  ┌─────────────┐
│  │ ESP32+LoRa  │  ← Device can be in any position
│  │ (no compass │  ← Magnetometer not used for direction
│  │  needed)    │  ← Only RSSI + radio communication
│  └─────────────┘
```

### Alignment System
- **Physical Cradle**: Device designed to hold phone in exact position
- **Magnetic Alignment**: Magnets ensure consistent phone placement
- **Sensor Calibration**: Software accounts for any small misalignments
- **Visual Guides**: Marks on device to show correct phone positioning

### Connections (Simplified - No Device Compass Needed)
```
ESP32-S3    SX1276      Power/Misc     Pocket Design
--------    ------      ----------     -------------
GPIO18  ->  SCK         18650 Battery  Compact enclosure
GPIO19  ->  MISO        Solar panel    Lanyard/clip
GPIO23  ->  MOSI        Charging port  LED status light
GPIO5   ->  NSS         Power switch   Button (optional)
GPIO27  ->  DIO0        Status LED     
GPIO26  ->  DIO1        
3.3V    ->  VCC         
GND     ->  GND         

Note: LSM9DS1 magnetometer optional - only needed for device stability/orientation logging
```

### Automated Positioning System
```c
// Automatic position tracking (no manual triangulation needed):
1. Continuous RSSI Monitoring: Collect measurements every 10 seconds
2. Natural Movement Tracking: Use normal walking patterns for positioning
3. Automatic Triangulation: Calculate positions from collected data points
4. Machine Learning: Improve accuracy over time with movement patterns
5. Real-Time Updates: Live position updates as people move naturally
```

#### **How Automated Positioning Works:**

##### **Method 1: Continuous RSSI Collection**
```c
// Automatic background positioning:
1. Device measures RSSI to all nearby devices every 10 seconds
2. Store RSSI + timestamp + rough phone GPS (if available)
3. Build database of signal strength over time and locations
4. No user action required - happens automatically
5. Calculate relative positions from accumulated data
```

##### **Method 2: Movement Pattern Recognition**
```c
// Smart positioning using natural movement:
1. Detect when RSSI values change (person is moving)
2. Track RSSI trends over 2-3 minutes of normal walking
3. Use multiple data points to triangulate automatically
4. Continuously refine position estimates as people move
5. Show live "distance getting closer/farther" updates
```

##### **Method 3: Multi-Device Auto-Triangulation**
```c
// Network-wide positioning without manual steps:
1. All devices share RSSI readings with each other
2. Each device knows signal strength to every other device
3. Combine all RSSI data for accurate position calculation
4. Update positions automatically every 30 seconds
5. Display real-time relative positions on map
```

##### **Method 4: Smart Direction Finding**
```c
// Intelligent "warmer/colder" with auto-learning:
1. Track RSSI changes as user walks naturally
2. Learn which directions increase/decrease signal strength
3. Predict best walking direction based on RSSI trends
4. Show arrow pointing toward predicted target location
5. Auto-update direction as new RSSI data comes in
```

### Automated Positioning Features:
- **Zero Manual Setup**: No "walk 50m" instructions needed
- **Continuous Tracking**: Position updates happen automatically
- **Natural Movement**: Uses normal walking patterns for triangulation
- **Real-Time Updates**: Live distance and direction as people move
- **Smart Learning**: Accuracy improves over time with more data
- **Background Operation**: Works while using other features
- **Multi-Device Sync**: Network-wide position sharing
- **Offline Operation**: Works without internet or GPS

#### **Automatic Positioning Algorithm:**
```c
// Background position tracking:
typedef struct {
    float rssi_history[10];      // Last 10 RSSI readings
    uint64_t timestamps[10];     // When readings were taken
    float estimated_distance;    // Current distance estimate
    float confidence_level;      // How accurate we think this is
    vector2d_t relative_pos;     // X,Y position relative to us
} device_position_t;

// Auto-triangulation process:
1. Every 10 seconds: measure RSSI to all devices
2. Store reading with timestamp in circular buffer
3. Detect movement when RSSI values change significantly
4. When 3+ different RSSI values exist, run triangulation
5. Update position estimate and confidence level
6. Share position data with other devices in network
7. Display live positions on phone interface
```

#### **User Interface Examples:**
```
┌─────────────────────────┐
│    Device Finder        │ 
│                         │
│        ↑ N              │
│    W ← ⊕ → E           │  ← Your position
│        ↓ S              │
│                         │
│  Alice    📍            │  ← Device at 150m, NE
│  ~150m    ↗             │
│  Signal: ████░          │
│                         │
│  Bob      📍            │  ← Device at 300m, SW  
│  ~300m    ↙             │
│  Signal: ██░░░          │
│                         │
│ 📊 Triangulation Mode  │  ← Tap for precise mode
└─────────────────────────┘
```

### Pocket-Mode Advantages:
- **No Alignment Needed**: Device works in any pocket orientation
- **Phone Compass**: Use phone's built-in compass (always held upright)
- **RSSI-Based**: Primary positioning uses signal strength + movement
- **Simpler Hardware**: No need for device magnetometer alignment
- **More Practical**: Just carry device in pocket like a phone
- **Cost Effective**: Eliminates complex mounting system ($8 savings)

### Antenna Considerations
- **Frequency**: Match regional ISM band (868MHz EU, 915MHz US)
- **Gain**: 3-5dBi for optimal range vs power trade-off
- **Placement**: External, vertical orientation for omnidirectional pattern

## Regulatory Compliance

### ISM Band Usage
- **Power limits**: 25mW (14dBm) maximum
- **Duty cycle**: <1% in EU, <100% in US
- **Bandwidth**: LoRa spread spectrum compliant

### Certification Requirements
- **FCC Part 15** (US)
- **CE marking** (EU)
- **IC certification** (Canada)

## Cost Estimation

### Per Unit (Pocket-Optimized Version)
- ESP32-S3 module: $8
- SX1276 LoRa module: $12
- 18650 battery + holder: $8
- Solar panel (5W): $15
- Antenna: $5
- PCB + components: $12
- Pocket enclosure: $7
- **Total**: ~$67 (saves $43 vs GPS-enabled phone-mount version)

### Optional Additions
- **LSM9DS1 sensor** (for advanced features): +$12
- **OLED display** (standalone operation): +$8
- **Phone mount** (if you want both modes): +$8

### GPS Integration Benefits (Using Phone GPS)
- **No GPS Module Cost**: Saves $15-25 vs dedicated GPS hardware
- **No GPS Power Drain**: Saves 25-50mA continuous power usage
- **Better Accuracy**: Phone GPS often more accurate than basic modules
- **Instant GPS Lock**: Phones have AGPS for faster satellite acquisition
- **Always Updated**: Phone OS keeps GPS firmware current automatically

### Phone-as-Display Benefits
- **Larger Screen**: 5-6" phone vs 1" OLED display
- **Better Resolution**: 1080p+ vs 128x64 pixels
- **Touch Interface**: Interactive vs button-only navigation  
- **Color Display**: Full color vs monochrome
- **Always Available**: Everyone carries a phone
- **Dual Mode**: Works with or without phone attached

## Competitive Analysis

### Advantages over alternatives:
- **GoTenna**: More open platform, web interface
- **Beartooth**: Lower cost, better range
- **Satellite communicators**: No subscription fees
- **Ham radio**: No license required, easier to use

## Future Roadmap

### Version 2.0 Features
- **Voice messages** (compressed audio)
- **Cryptocurrency wallet** integration
- **Sensor data** collection and sharing
- **Mesh network analytics** and optimization
- **Custom encryption** protocols

---

*This project is open source and designed for educational and emergency communication purposes.*
