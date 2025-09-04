# ESP32 Multi-Audio Player

A ESP32-based audio player that supports both MP3 file playback from internal storage and Bluetooth A2DP audio streaming. Built with best practices for optimal performance on microcontroller platforms.

## 🎵 Features

- **MP3 Playback**: Play MP3 files stored on the ESP32's LittleFS filesystem
- **Bluetooth Audio**: Stream audio via Bluetooth A2DP protocol
- **Serial Control**: Simple command-line interface via USB serial
- **Dual Mode Operation**: Seamlessly switch between MP3 and Bluetooth modes
- **File System Integration**: Automatic MP3 file discovery and listing
- **Robust Error Handling**: Comprehensive error codes and parameter validation

## 🔧 Hardware Requirements

### ESP32 Development Board
- **Tested on**: uPesy ESP32 Wroom DevKit
- **Compatible with**: Most ESP32 development boards

### Audio Output
- **I2S DAC**: Uses ESP32's built-in DAC for audio output
    (can be used also an external DAC)
- **Pin Configuration**:
  - BCLK (Bit Clock): GPIO 22
  - WCLK (Word Clock): GPIO 25  
  - DOUT (Data Out): GPIO 26

### Optional External Components
- Amplifier circuit for speaker output
- 3.5mm audio jack for headphone output
- External DAC for improved audio quality

## 📋 Software Requirements



### Dependencies
The following libraries are automatically managed by PlatformIO:
- `ESP8266Audio@^1.9.7` - Audio processing and codecs
- `ESP32-A2DP` - Bluetooth A2DP sink functionality
- `LittleFS` - File system support
- `FS` - File system interface

## 🚀 Getting Started

### 1. Clone the Repository
```bash
git clone https://github.com/drenchew/ESP-32-BLE.git
cd ESP-32-BLE
```

### 2. Open in PlatformIO
- Open VS Code with PlatformIO extension
- Open the project folder
- PlatformIO will automatically install dependencies

### 3. Upload MP3 Files (Optional)
Place your MP3 files in the `data/` folder, then upload the filesystem:
```bash
pio run --target uploadfs
```

### 4. Build and Upload
```bash
# Build the project
pio run

# Upload to ESP32
pio run --target upload
```

### 5. Connect Serial Monitor
```bash
pio device monitor
```

## 🎮 Usage

### Serial Commands
Connect to the ESP32 via serial monitor (115200 baud) and use these commands:

| Command | Alias | Description |
|---------|-------|-------------|
| `play` | `p` | Play the first MP3 file found on filesystem |
| `bt` | `b` | Start Bluetooth audio mode |
| `stop` | `s` | Stop current playback (MP3 or Bluetooth) |
| `info` | - | Display current system status |
| `help` | - | Show available commands |

### Bluetooth Pairing
1. Send `bt` command via serial
2. Device will appear as "ESP32_Audio_Player"
3. Pair from your phone/computer
4. Start streaming audio

### File Management
- MP3 files are stored in LittleFS filesystem
- Use `uploadfs` target to upload files from `data/` folder
- System automatically scans for `.mp3` files

## ⚙️ Configuration


### Memory Usage
- **RAM**: ~12.2% (40,072 bytes used)
- **Flash**: ~97.2% (1,274,097 bytes used)

### Performance
- **Sample Rate**: 44.1 kHz
- **Bit Depth**: 16-bit
- **Channels**: Configurable (default: mono)
- **Latency**: Low-latency I2S output

#

## 📄 License

This project is open source. Feel free to use and modify for your projects.

