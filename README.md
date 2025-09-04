# ESP32 Microcontroller Audio Engine

A **FreeRTOS-based** ESP32 audio player with real-time multitasking capabilities. Supports both MP3 file playback from internal storage and Bluetooth A2DP audio streaming with professional-grade task scheduling and resource management.

## 🎵 Features

- **Real-Time Multitasking**: FreeRTOS-based architecture with priority-based task scheduling
- **MP3 Playback**: Play MP3 files stored on the ESP32's LittleFS filesystem  
- **Bluetooth Audio**: Stream audio via Bluetooth A2DP protocol
- **Thread-Safe Operations**: Mutex-protected shared resources and atomic operations
- **Asynchronous Command Processing**: Non-blocking command handling via FreeRTOS queues
- **Serial Control**: Command-line interface with real-time response
- **Dual Mode Operation**: Seamless switching between MP3 and Bluetooth modes
- **System Monitoring**: Real-time memory and task performance monitoring
- **Robust Error Handling**: Comprehensive error codes and timeout management



### FreeRTOS Task Structure
The system runs **three concurrent tasks** with priority-based scheduling:

| Task | Priority | Stack | Timing | Purpose |
|------|----------|-------|---------|---------|
| **Audio Task** | 3 (Highest) | 4KB | 5ms | Real-time audio processing |
| **Command Task** | 2 (Medium) | 8KB | 50ms | User interface and command handling |
| **System Task** | 1 (Lowest) | 2KB | 1000ms | Background monitoring |



##  Hardware Requirements

### ESP32 Development Board
- **Tested on**: uPesy ESP32 Wroom DevKit
- **Compatible with**: Most ESP32 development boards
- **RAM Requirements**: Minimum 512KB (uses ~320KB)

### Audio Output
- **I2S DAC**: Uses ESP32's built-in DAC for audio output
    (external DAC can also be used)
- **Pin Configuration**:
  - BCLK (Bit Clock): GPIO 22
  - WCLK (Word Clock): GPIO 25  
  - DOUT (Data Out): GPIO 26

### Optional External Components
- External DAC for improved audio quality

## Software Requirements

### Development Environment
- **PlatformIO**: Recommended IDE with ESP32 support
- **Framework**: Arduino Core for ESP32
- **RTOS**: FreeRTOS

### Dependencies
The following libraries are automatically managed by PlatformIO:
- `ESP8266Audio@^1.9.7` - Audio processing and codecs
- `ESP32-A2DP` - Bluetooth A2DP sink functionality
- `LittleFS` - File system support
- `FS` - File system interface


### 1. Clone the Repository
```bash
git clone https://github.com/drenchew/ESP-32-BLE-1.git
cd ESP-32-BLE-1
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
| `info` | - | Display system status and task information |
| `help` | - | Show available commands and RTOS info |

### Expected Output
```
=== ESP32 Multi-Audio Player (FreeRTOS) ===
Listing directory: /
FreeRTOS tasks created successfully!

Available commands: play/p, bt/b, stop/s, info, help
```


### Memory Usage
- **RAM Usage**: ~45KB runtime + task stacks
  - Audio Task: 4KB stack
  - Command Task: 8KB stack  
  - System Task: 2KB stack
- **Flash Usage**: ~180KB program code


## 🔍 System Monitoring

### Runtime Information
Use the `info` command to display:
```
Current mode: Bluetooth
Bluetooth enabled: Yes
Free heap: 245760 bytes
Audio task stack free: 2048 bytes
```


##  Development

### FreeRTOS Features Used
- **Tasks**: Concurrent execution with priorities
- **Mutexes**: Thread-safe shared resource access
- **Queues**: Inter-task communication
- **Precise Timing**: `vTaskDelayUntil()` for audio consistency

### Architecture Benefits
- **Scalability**: Easy to add new tasks (display, network, sensors)
- **Maintainability**: Clear separation of concerns
- **Real-time Performance**: Guaranteed audio processing timing
- **Professional Grade**: Industry-standard embedded development patterns

##  Learning Resources

This project demonstrates:
- **Embedded C Programming**: Static allocation, hardware interfacing
- **Real-Time Systems**: Task scheduling, timing constraints
- **Audio Processing**: I2S protocol, DMA buffers
- **Wireless Communication**: Bluetooth A2DP implementation
- **System Architecture**: Multi-threaded embedded design

See `FreeRTOS_Learning_Guide.md` for comprehensive implementation details.

##  License
This project is open source. Feel free to use and modify for your projects.

---

**Technologies:** C/C++, FreeRTOS, ESP32, I2S Protocol, Bluetooth A2DP, LittleFS  
**Features:** Real-time multitasking audio engine with thread-safe operations and priority-based task scheduling
