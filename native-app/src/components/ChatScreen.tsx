import React, { useState, useEffect } from 'react';
import {
  View,
  Text,
  FlatList,
  TextInput,
  TouchableOpacity,
  StyleSheet,
  Alert,
} from 'react-native';
import { BleManager, Device } from 'react-native-ble-plx';
import Geolocation from '@react-native-community/geolocation';
import PushNotification from 'react-native-push-notification';

interface MeshMessage {
  id: string;
  senderId: string;
  senderName: string;
  text: string;
  timestamp: number;
  location?: {
    latitude: number;
    longitude: number;
    accuracy: number;
  };
}

interface ConnectedDevice {
  id: string;
  name: string;
  rssi: number;
  distance: number;
  location?: {
    latitude: number;
    longitude: number;
  };
}

const ChatScreen: React.FC = () => {
  const [messages, setMessages] = useState<MeshMessage[]>([]);
  const [devices, setDevices] = useState<ConnectedDevice[]>([]);
  const [inputText, setInputText] = useState('');
  const [bleManager] = useState(new BleManager());
  const [connectedDevice, setConnectedDevice] = useState<Device | null>(null);
  const [currentLocation, setCurrentLocation] = useState<{latitude: number; longitude: number} | null>(null);

  useEffect(() => {
    initializeBluetooth();
    startLocationTracking();
    configurePushNotifications();
    
    return () => {
      bleManager.destroy();
    };
  }, []);

  const initializeBluetooth = () => {
    bleManager.onStateChange((state) => {
      if (state === 'PoweredOn') {
        scanForDevices();
      }
    }, true);
  };

  const scanForDevices = () => {
    bleManager.startDeviceScan(
      ['6E400001-B5A3-F393-E0A9-E50E24DCCA9E'], // MeshChat service UUID
      { allowDuplicates: false },
      (error, device) => {
        if (error) {
          console.log('Scan error:', error);
          return;
        }

        if (device && device.name === 'MeshChat-Device') {
          bleManager.stopDeviceScan();
          connectToDevice(device);
        }
      }
    );
  };

  const connectToDevice = async (device: Device) => {
    try {
      const connectedDevice = await device.connect();
      await connectedDevice.discoverAllServicesAndCharacteristics();
      setConnectedDevice(connectedDevice);
      
      // Start listening for messages
      startMessageListener(connectedDevice);
      
      Alert.alert('Connected', `Connected to ${device.name}`);
    } catch (error) {
      console.log('Connection error:', error);
      Alert.alert('Connection Failed', 'Could not connect to device');
    }
  };

  const startMessageListener = (device: Device) => {
    // Listen for incoming messages from ESP32
    device.monitorCharacteristicForService(
      '6E400001-B5A3-F393-E0A9-E50E24DCCA9E',
      '6E400003-B5A3-F393-E0A9-E50E24DCCA9E',
      (error, characteristic) => {
        if (error) {
          console.log('Monitor error:', error);
          return;
        }

        if (characteristic?.value) {
          const messageData = JSON.parse(atob(characteristic.value));
          const newMessage: MeshMessage = {
            id: messageData.id,
            senderId: messageData.senderId,
            senderName: messageData.senderName,
            text: messageData.text,
            timestamp: messageData.timestamp,
            location: messageData.location,
          };

          setMessages(prev => [...prev, newMessage]);
          
          // Show push notification if app is backgrounded
          PushNotification.localNotification({
            title: `Message from ${newMessage.senderName}`,
            message: newMessage.text,
            playSound: true,
            soundName: 'default',
          });
        }
      }
    );
  };

  const startLocationTracking = () => {
    Geolocation.watchPosition(
      (position) => {
        const { latitude, longitude } = position.coords;
        setCurrentLocation({ latitude, longitude });
        
        // Send location to device if connected
        if (connectedDevice) {
          sendLocationToDevice({ latitude, longitude, accuracy: position.coords.accuracy });
        }
      },
      (error) => console.log('GPS error:', error),
      {
        enableHighAccuracy: true,
        distanceFilter: 5, // Only update if moved 5+ meters
        interval: 30000,   // Check every 30 seconds
      }
    );
  };

  const sendLocationToDevice = async (location: {latitude: number; longitude: number; accuracy: number}) => {
    if (!connectedDevice) return;

    try {
      const locationData = {
        type: 'gps_update',
        latitude: location.latitude,
        longitude: location.longitude,
        accuracy: location.accuracy,
        timestamp: Date.now(),
      };

      const base64Data = btoa(JSON.stringify(locationData));
      
      await connectedDevice.writeCharacteristicWithResponseForService(
        '6E400001-B5A3-F393-E0A9-E50E24DCCA9E',
        '6E400002-B5A3-F393-E0A9-E50E24DCCA9E',
        base64Data
      );
    } catch (error) {
      console.log('Failed to send location:', error);
    }
  };

  const sendMessage = async () => {
    if (!inputText.trim() || !connectedDevice) return;

    try {
      const message: MeshMessage = {
        id: Date.now().toString(),
        senderId: 'user',
        senderName: 'You',
        text: inputText.trim(),
        timestamp: Date.now(),
        location: currentLocation || undefined,
      };

      const base64Data = btoa(JSON.stringify({
        type: 'message',
        ...message,
      }));

      await connectedDevice.writeCharacteristicWithResponseForService(
        '6E400001-B5A3-F393-E0A9-E50E24DCCA9E',
        '6E400002-B5A3-F393-E0A9-E50E24DCCA9E',
        base64Data
      );

      setMessages(prev => [...prev, message]);
      setInputText('');
    } catch (error) {
      console.log('Failed to send message:', error);
      Alert.alert('Send Failed', 'Could not send message');
    }
  };

  const configurePushNotifications = () => {
    PushNotification.configure({
      onNotification: (notification) => {
        console.log('Notification received:', notification);
      },
      permissions: {
        alert: true,
        badge: true,
        sound: true,
      },
      popInitialNotification: true,
      requestPermissions: true,
    });
  };

  const renderMessage = ({ item }: { item: MeshMessage }) => (
    <View style={[
      styles.messageBubble,
      item.senderId === 'user' ? styles.myMessage : styles.otherMessage
    ]}>
      <Text style={styles.senderName}>{item.senderName}</Text>
      <Text style={styles.messageText}>{item.text}</Text>
      <Text style={styles.timestamp}>
        {new Date(item.timestamp).toLocaleTimeString()}
      </Text>
      {item.location && (
        <Text style={styles.location}>
          📍 Location shared ({item.location.latitude.toFixed(6)}, {item.location.longitude.toFixed(6)})
        </Text>
      )}
    </View>
  );

  return (
    <View style={styles.container}>
      <View style={styles.header}>
        <Text style={styles.title}>MeshChat</Text>
        <Text style={styles.status}>
          {connectedDevice ? '🟢 Connected' : '🔴 Disconnected'}
        </Text>
      </View>

      <FlatList
        data={messages}
        renderItem={renderMessage}
        keyExtractor={(item) => item.id}
        style={styles.messageList}
      />

      <View style={styles.inputContainer}>
        <TextInput
          style={styles.textInput}
          value={inputText}
          onChangeText={setInputText}
          placeholder="Type a message..."
          multiline
        />
        <TouchableOpacity
          style={styles.sendButton}
          onPress={sendMessage}
          disabled={!connectedDevice}
        >
          <Text style={styles.sendButtonText}>Send</Text>
        </TouchableOpacity>
      </View>
    </View>
  );
};

const styles = StyleSheet.create({
  container: {
    flex: 1,
    backgroundColor: '#f5f5f5',
  },
  header: {
    padding: 16,
    backgroundColor: '#007AFF',
    flexDirection: 'row',
    justifyContent: 'space-between',
    alignItems: 'center',
  },
  title: {
    fontSize: 20,
    fontWeight: 'bold',
    color: 'white',
  },
  status: {
    fontSize: 14,
    color: 'white',
  },
  messageList: {
    flex: 1,
    padding: 16,
  },
  messageBubble: {
    padding: 12,
    marginVertical: 4,
    borderRadius: 12,
    maxWidth: '80%',
  },
  myMessage: {
    backgroundColor: '#007AFF',
    alignSelf: 'flex-end',
  },
  otherMessage: {
    backgroundColor: '#E9E9EB',
    alignSelf: 'flex-start',
  },
  senderName: {
    fontSize: 12,
    fontWeight: 'bold',
    marginBottom: 4,
    color: '#666',
  },
  messageText: {
    fontSize: 16,
    color: '#000',
  },
  timestamp: {
    fontSize: 10,
    color: '#666',
    marginTop: 4,
  },
  location: {
    fontSize: 10,
    color: '#007AFF',
    marginTop: 2,
  },
  inputContainer: {
    flexDirection: 'row',
    padding: 16,
    backgroundColor: 'white',
  },
  textInput: {
    flex: 1,
    borderWidth: 1,
    borderColor: '#ddd',
    borderRadius: 20,
    paddingHorizontal: 16,
    paddingVertical: 8,
    marginRight: 8,
    maxHeight: 100,
  },
  sendButton: {
    backgroundColor: '#007AFF',
    borderRadius: 20,
    paddingHorizontal: 20,
    paddingVertical: 10,
    justifyContent: 'center',
  },
  sendButtonText: {
    color: 'white',
    fontWeight: 'bold',
  },
});

export default ChatScreen;
