// Web Bluetooth API integration for MeshChat
class BluetoothManager {
    constructor() {
        this.device = null;
        this.server = null;
        this.service = null;
        this.txCharacteristic = null;
        this.rxCharacteristic = null;
        this.isConnected = false;
        this.messageCallback = null;
        
        // UUIDs must match the ESP32 GATT service
        this.serviceUUID = '6e400001-b5a3-f393-e0a9-e50e24dcca9e';
        this.txCharacteristicUUID = '6e400002-b5a3-f393-e0a9-e50e24dcca9e'; // Phone -> Device
        this.rxCharacteristicUUID = '6e400003-b5a3-f393-e0a9-e50e24dcca9e'; // Device -> Phone
    }

    async connect() {
        try {
            console.log('Requesting Bluetooth Device...');
            
            this.device = await navigator.bluetooth.requestDevice({
                filters: [
                    { name: 'MeshChat' },
                    { namePrefix: 'MeshChat-' }
                ],
                optionalServices: [this.serviceUUID]
            });

            console.log('Connecting to GATT Server...');
            this.server = await this.device.gatt.connect();

            console.log('Getting Service...');
            this.service = await this.server.getPrimaryService(this.serviceUUID);

            console.log('Getting Characteristics...');
            this.txCharacteristic = await this.service.getCharacteristic(this.txCharacteristicUUID);
            this.rxCharacteristic = await this.service.getCharacteristic(this.rxCharacteristicUUID);

            // Subscribe to notifications from device
            await this.rxCharacteristic.startNotifications();
            this.rxCharacteristic.addEventListener('characteristicvaluechanged', 
                this.handleNotification.bind(this));

            // Listen for disconnection
            this.device.addEventListener('gattserverdisconnected', 
                this.onDisconnected.bind(this));

            this.isConnected = true;
            this.updateConnectionStatus(true);
            
            console.log('Connected to MeshChat device');
            
            // Get device info
            await this.requestDeviceInfo();
            
            return true;
        } catch (error) {
            console.error('Bluetooth connection failed:', error);
            this.updateConnectionStatus(false);
            throw error;
        }
    }

    async disconnect() {
        if (this.device && this.device.gatt.connected) {
            await this.device.gatt.disconnect();
        }
        this.cleanup();
    }

    cleanup() {
        this.device = null;
        this.server = null;
        this.service = null;
        this.txCharacteristic = null;
        this.rxCharacteristic = null;
        this.isConnected = false;
        this.updateConnectionStatus(false);
    }

    onDisconnected() {
        console.log('Device disconnected');
        this.cleanup();
    }

    handleNotification(event) {
        const value = event.target.value;
        const message = new TextDecoder().decode(value);
        
        console.log('Received from device:', message);
        
        try {
            const data = JSON.parse(message);
            if (this.messageCallback) {
                this.messageCallback(data);
            }
        } catch (error) {
            console.error('Failed to parse message:', error);
            // Handle as plain text message
            if (this.messageCallback) {
                this.messageCallback({
                    type: 'text',
                    text: message,
                    timestamp: Date.now()
                });
            }
        }
    }

    async sendMessage(message) {
        if (!this.isConnected || !this.txCharacteristic) {
            throw new Error('Not connected to device');
        }

        try {
            const data = JSON.stringify(message);
            const encoder = new TextEncoder();
            const encodedData = encoder.encode(data);
            
            // Split large messages if needed (BLE has MTU limits)
            const maxChunkSize = 200; // Conservative size
            
            if (encodedData.length <= maxChunkSize) {
                await this.txCharacteristic.writeValue(encodedData);
            } else {
                // Send in chunks
                for (let i = 0; i < encodedData.length; i += maxChunkSize) {
                    const chunk = encodedData.slice(i, i + maxChunkSize);
                    await this.txCharacteristic.writeValue(chunk);
                    await this.delay(50); // Small delay between chunks
                }
            }
            
            console.log('Sent to device:', data);
        } catch (error) {
            console.error('Failed to send message:', error);
            throw error;
        }
    }

    async requestDeviceInfo() {
        try {
            await this.sendMessage({
                type: 'request',
                action: 'device_info'
            });
        } catch (error) {
            console.error('Failed to request device info:', error);
        }
    }

    async sendTextMessage(recipientId, text, isEmergency = false) {
        const message = {
            type: 'text_message',
            recipient_id: recipientId,
            text: text,
            emergency: isEmergency,
            timestamp: Date.now()
        };

        return this.sendMessage(message);
    }

    async sendBroadcast(text, isEmergency = false) {
        return this.sendTextMessage('broadcast', text, isEmergency);
    }

    updateConnectionStatus(connected) {
        const statusIndicator = document.getElementById('statusIndicator');
        const statusText = document.getElementById('statusText');
        
        if (connected) {
            statusIndicator.classList.add('connected');
            statusText.textContent = 'Connected';
        } else {
            statusIndicator.classList.remove('connected');
            statusText.textContent = 'Disconnected';
        }
    }

    setMessageCallback(callback) {
        this.messageCallback = callback;
    }

    delay(ms) {
        return new Promise(resolve => setTimeout(resolve, ms));
    }

    // Check if Web Bluetooth is supported
    static isSupported() {
        return navigator.bluetooth !== undefined;
    }

    // Get signal strength (if available)
    async getSignalStrength() {
        if (!this.device || !this.device.gatt.connected) {
            return null;
        }

        try {
            // This is experimental and may not work on all devices
            return await this.device.gatt.readRSSI?.();
        } catch (error) {
            console.log('RSSI not available');
            return null;
        }
    }
}

// Global instance
let bluetoothManager = null;

function initializeBluetooth() {
    bluetoothManager = new BluetoothManager();
    
    // Check if Web Bluetooth is supported
    if (!BluetoothManager.isSupported()) {
        console.error('Web Bluetooth is not supported');
        document.getElementById('connectBtn').disabled = true;
        document.getElementById('connectBtn').textContent = 'Bluetooth Not Supported';
        return;
    }

    // Setup connection button
    const connectBtn = document.getElementById('connectBtn');
    connectBtn.addEventListener('click', async () => {
        if (bluetoothManager.isConnected) {
            await bluetoothManager.disconnect();
            connectBtn.textContent = 'Connect to MeshChat Device';
        } else {
            try {
                connectBtn.disabled = true;
                connectBtn.textContent = 'Connecting...';
                
                await bluetoothManager.connect();
                connectBtn.textContent = 'Disconnect';
                
                // Show chat interface
                document.getElementById('connectionPanel').style.display = 'none';
                document.getElementById('chatContainer').style.display = 'flex';
                
            } catch (error) {
                alert('Failed to connect: ' + error.message);
                connectBtn.textContent = 'Connect to MeshChat Device';
            } finally {
                connectBtn.disabled = false;
            }
        }
    });

    // Set up message callback
    bluetoothManager.setMessageCallback((data) => {
        handleIncomingMessage(data);
    });
}

function handleIncomingMessage(data) {
    console.log('Incoming message:', data);
    
    switch (data.type) {
        case 'text_message':
            addMessageToChat(data.text, false, data.emergency, data.timestamp);
            break;
            
        case 'device_info':
            updateDeviceInfo(data);
            break;
            
        case 'network_status':
            updateNetworkStatus(data);
            break;
            
        case 'ack':
            updateMessageStatus(data.message_id, 'delivered');
            break;
            
        default:
            console.log('Unknown message type:', data.type);
    }
}

function updateDeviceInfo(info) {
    const deviceInfo = document.getElementById('deviceInfo');
    const deviceName = document.getElementById('deviceName');
    const batteryLevel = document.getElementById('batteryLevel');
    
    if (info.device_name) {
        deviceName.textContent = info.device_name;
    }
    
    if (info.battery_voltage) {
        const percentage = Math.round(((info.battery_voltage - 3200) / (4200 - 3200)) * 100);
        batteryLevel.textContent = `${percentage}% (${info.battery_voltage}mV)`;
    }
    
    deviceInfo.style.display = 'block';
}

function updateNetworkStatus(status) {
    const networkStatus = document.getElementById('networkStatus');
    if (networkStatus) {
        networkStatus.textContent = `${status.connected_devices} devices, ${status.routes} routes`;
    }
}

// Export for use in other modules
window.bluetoothManager = bluetoothManager;
