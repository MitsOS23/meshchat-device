# MeshChat Native iOS App

React Native implementation of the MeshChat interface with enhanced iOS features.

## Features

### Core Functionality
- **Native Bluetooth LE**: Full iOS BLE Central Manager integration
- **Background Processing**: Receive messages even when app is closed
- **Enhanced GPS**: CoreLocation with background location updates
- **Push Notifications**: Local and remote notification support
- **Native UI**: iOS-standard interface components

### Technical Advantages
- **Better Performance**: Native components, 60fps animations
- **Reliable Connectivity**: Automatic BLE reconnection, background scanning
- **System Integration**: Siri, Control Center, iOS notification center
- **App Store Distribution**: Easy installation and updates
- **Security**: iOS Keychain for encryption key storage

## Project Structure

```
MeshChat-iOS/
├── package.json                    # React Native dependencies
├── metro.config.js                # Bundler configuration
├── react-native.config.js         # Native module linking
├── ios/
│   ├── MeshChat.xcodeproj         # Xcode project
│   ├── MeshChat/
│   │   ├── Info.plist             # iOS app configuration
│   │   ├── AppDelegate.m          # iOS app lifecycle
│   │   ├── main.m                 # iOS entry point
│   │   └── Images.xcassets/       # App icons and images
│   ├── MeshChatTests/             # Unit tests
│   └── Modules/                   # Custom native modules
│       ├── BluetoothModule.h/.m   # Enhanced BLE functionality
│       ├── LocationModule.h/.m    # Advanced GPS features
│       └── EncryptionModule.h/.m  # Hardware-accelerated crypto
├── src/
│   ├── App.tsx                    # Root React Native component
│   ├── components/                # Reusable UI components
│   │   ├── ChatScreen.tsx         # Main messaging interface
│   │   ├── MapScreen.tsx          # Real-time positioning map
│   │   ├── DeviceList.tsx         # BLE device discovery
│   │   ├── SettingsScreen.tsx     # App configuration
│   │   ├── EmergencyMode.tsx      # Emergency messaging mode
│   │   └── common/                # Shared components
│   │       ├── Button.tsx         # Styled button component
│   │       ├── TextInput.tsx      # Message input field
│   │       ├── DeviceCard.tsx     # Device info display
│   │       └── MessageBubble.tsx  # Chat message display
│   ├── navigation/                # App navigation structure
│   │   ├── AppNavigator.tsx       # Main navigation stack
│   │   ├── TabNavigator.tsx       # Bottom tab navigation
│   │   └── types.ts               # Navigation type definitions
│   ├── services/                  # Core business logic
│   │   ├── BluetoothManager.ts    # BLE connection management
│   │   ├── LocationService.ts     # GPS and positioning
│   │   ├── NotificationService.ts # Push notification handling
│   │   ├── MeshProtocol.ts        # Message protocol implementation
│   │   ├── EncryptionService.ts   # End-to-end encryption
│   │   ├── StorageService.ts      # Local data persistence
│   │   └── NetworkService.ts      # Mesh network management
│   ├── store/                     # Redux state management
│   │   ├── index.ts               # Store configuration
│   │   ├── slices/                # Redux slices
│   │   │   ├── messagesSlice.ts   # Message state
│   │   │   ├── devicesSlice.ts    # Connected devices
│   │   │   ├── locationSlice.ts   # GPS and positioning
│   │   │   ├── settingsSlice.ts   # App preferences
│   │   │   └── networkSlice.ts    # Mesh network state
│   │   └── selectors/             # Redux selectors
│   ├── types/                     # TypeScript type definitions
│   │   ├── bluetooth.ts           # BLE-related types
│   │   ├── location.ts            # GPS and positioning types
│   │   ├── mesh.ts                # Mesh protocol types
│   │   └── app.ts                 # General app types
│   ├── utils/                     # Utility functions
│   │   ├── encryption.ts          # Crypto utilities
│   │   ├── positioning.ts         # RSSI triangulation
│   │   ├── validation.ts          # Input validation
│   │   └── constants.ts           # App constants
│   └── hooks/                     # Custom React hooks
│       ├── useBluetooth.ts        # BLE connection hook
│       ├── useLocation.ts         # GPS tracking hook
│       ├── useMessages.ts         # Message handling hook
│       └── useNotifications.ts    # Notification hook
├── assets/                        # Static assets
│   ├── icons/                     # UI icons and graphics
│   ├── sounds/                    # Notification sounds
│   └── fonts/                     # Custom fonts
└── __tests__/                     # Test files
    ├── components/                # Component tests
    ├── services/                  # Service tests
    └── utils/                     # Utility tests
```

## Key Dependencies

### Core React Native
```json
{
  "react": "18.2.0",
  "react-native": "0.73.0",
  "@react-navigation/native": "^6.1.0",
  "@react-navigation/stack": "^6.3.0",
  "@react-navigation/bottom-tabs": "^6.5.0"
}
```

### Bluetooth & Hardware
```json
{
  "react-native-ble-plx": "^3.1.0",
  "@react-native-community/geolocation": "^3.2.0",
  "react-native-permissions": "^4.1.0",
  "react-native-background-job": "^1.1.0"
}
```

### UI & UX
```json
{
  "react-native-maps": "^1.8.0",
  "react-native-vector-icons": "^10.0.0",
  "react-native-push-notification": "^8.1.0",
  "react-native-haptic-feedback": "^2.2.0"
}
```

### State Management & Storage
```json
{
  "@reduxjs/toolkit": "^2.0.0",
  "react-redux": "^9.0.0",
  "@react-native-async-storage/async-storage": "^1.21.0",
  "react-native-keychain": "^8.1.0"
}
```

## Development Setup

### Prerequisites
- macOS with Xcode 14+
- iOS device (iPhone/iPad) for Bluetooth testing
- Apple Developer Account for device testing
- React Native development environment

### Installation
```bash
# Clone the project
git clone https://github.com/MitsOS23/meshchat-device.git
cd meshchat-device/native-app

# Install dependencies
npm install

# iOS setup
cd ios && pod install && cd ..

# Start Metro bundler
npx react-native start

# Run on iOS device (in another terminal)
npx react-native run-ios --device "Your iPhone"
```

### Development Commands
```bash
# Type checking
npm run typecheck

# Linting
npm run lint

# Testing
npm run test

# iOS build for release
npm run build:ios

# Generate iOS app bundle
cd ios && xcodebuild -workspace MeshChat.xcworkspace -scheme MeshChat archive
```

## Key Features Implementation

### Enhanced Bluetooth Management
- Automatic device discovery and connection
- Background scanning and reconnection
- Connection state persistence
- Multiple device support
- Error handling and recovery

### Advanced Location Services
- Background location tracking
- Geofencing and proximity alerts
- RSSI-based distance estimation
- Automatic positioning updates
- Location sharing privacy controls

### Native UI Components
- iOS-standard navigation patterns
- Haptic feedback integration
- Dark mode support
- Accessibility features
- Dynamic font sizing

### Push Notifications
- Local notifications for new messages
- Background message processing
- Custom notification actions
- Sound and vibration patterns
- Notification grouping

## Deployment

### App Store Preparation
1. **Code Signing**: Configure iOS certificates and provisioning profiles
2. **App Icons**: Generate all required icon sizes
3. **Screenshots**: Create App Store preview screenshots
4. **Metadata**: Prepare app description and keywords
5. **Review**: Submit for App Store review

### Beta Testing
- TestFlight distribution for beta users
- Crash reporting and analytics
- Performance monitoring
- User feedback collection

## Performance Optimizations

### Battery Life
- Efficient BLE scanning patterns
- Smart location update intervals
- Background task optimization
- CPU usage monitoring

### Memory Management
- Lazy loading of screens
- Image optimization
- Message history limits
- Automatic cleanup routines

### Network Efficiency
- Message batching
- Compression algorithms
- Retry logic with backoff
- Connection pooling

## Security Features

### Encryption
- End-to-end message encryption
- Key exchange protocols
- Secure key storage in iOS Keychain
- Perfect forward secrecy

### Privacy
- Optional location sharing
- Message retention controls
- Device anonymization
- No data collection

This native iOS app provides a significantly enhanced user experience compared to the web-based PWA, with better performance, reliability, and iOS system integration.
