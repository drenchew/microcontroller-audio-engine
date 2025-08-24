# ESP32 Multi-Audio Player

A versatile ESP32-based audio player that supports both local MP3 file playback and Bluetooth audio streaming through the internal DAC.

## Features

- 🎵 **MP3 File Playback**: Play MP3 files stored on LittleFS filesystem
- 📻 **Bluetooth Audio Streaming**: Receive audio via Bluetooth A2DP (acts as Bluetooth speaker)
- 🔊 **Internal DAC Output**: Uses ESP32's built-in DAC on pin 25 (and 26 for stereo)
- 💾 **LittleFS Storage**: Efficient file system for audio storage
- ⌨️ **Serial Command Interface**: Control via serial monitor
- 🔄 **Seamless Mode Switching**: Switch between MP3 and Bluetooth audio

## Hardware Requirements

- **ESP32 Development Board** (tested on uPesy ESP32 Wroom DevKit)
- **Speaker/Headphones** connected to pin 25 (audio output) and GND
- **Optional**: Connect pin 26 for stereo output

## Hardware Connection

```
ESP32 Pin 25 (DAC1) → Speaker Positive (+) or Audio Jack Tip
ESP32 GND           → Speaker Negative (-) or Audio Jack Ground
```

## Software Requirements

- [PlatformIO](https://platformio.org/) (recommended) or Arduino IDE
- ESP32 Arduino Core

## Installation

1. **Clone this repository:**
   ```bash
   git clone https://github.com/drenchew/ESP-32-BLE.git
   cd ESP-32-BLE
   ```

2. **Install dependencies:**
   The project uses PlatformIO which will automatically install required libraries:
   - ESP8266Audio v1.9.7
   - ESP32-A2DP (latest)

3. **Prepare MP3 files:**
   - Place your MP3 files in the `data/` folder
   - Recommended: Use 128kbps or lower bitrate for better compatibility

4. **Upload filesystem:**
   ```bash
   pio run --target uploadfs
   ```

5. **Build and upload firmware:**
   ```bash
   pio run --target upload
   ```

## Usage

### Serial Commands

Open serial monitor at 115200 baud and use these commands:

- `p` or `play` - Start MP3 playback
- `b` or `bt` - Start Bluetooth audio mode
- `s` or `stop` - Stop all audio
- `info` - Show system information
- `help` - Show available commands

### Bluetooth Audio

1. Send `b` command via serial monitor
2. Look for "ESP32_Audio_Player" in your phone's Bluetooth settings
3. Connect and play audio from your device

### MP3 Playback

1. Upload MP3 files to the `data/` folder
2. Upload filesystem: `pio run --target uploadfs`
3. Send `p` command via serial monitor

## Technical Details

### Audio Configuration

- **Sample Rate**: 44.1 kHz
- **Bit Depth**: 16-bit
- **Output**: Internal DAC (8-bit effective resolution)
- **Channels**: Configurable mono/stereo

### Memory Usage

- **Flash**: ~97% usage (1.28MB out of 1.31MB)
- **LittleFS**: 1.4MB available for MP3 files
- **Custom Partition**: Available in `partitions_custom.csv` for more firmware space

### Libraries Used

- **ESP8266Audio**: MP3 decoding and I2S output
- **ESP32-A2DP**: Bluetooth A2DP sink functionality
- **LittleFS**: File system for audio storage

## File Structure

```
├── src/
│   └── main.cpp              # Main application code
├── data/
│   └── output.mp3           # Your MP3 files go here
├── include/
├── lib/
├── test/
├── platformio.ini           # PlatformIO configuration
├── partitions_custom.csv    # Custom partition table (optional)
└── README.md               # This file
```

## Troubleshooting

### No Audio Output
- Check connections to pin 25 and GND
- Verify speaker/headphones are working
- Check serial monitor for error messages

### Bluetooth Issues
- Ensure device is in pairing mode
- Check if "ESP32_Audio_Player" appears in Bluetooth devices
- Try restarting Bluetooth on your phone

### MP3 Playback Issues
- Verify MP3 files are in `data/` folder
- Check file format (MP3, 128kbps recommended)
- Ensure filesystem was uploaded: `pio run --target uploadfs`

### Memory Issues
- Use the custom partition table in `partitions_custom.csv` for more firmware space
- Compress MP3 files to lower bitrates
- Remove unused features from code

## Development

### Adding New Features

The code is designed to be extensible:

- Add new audio modes to the `AudioMode` enum
- Implement new command handlers in the serial interface
- Extend audio output options

### Custom Partition Table

To use more firmware space, uncomment this line in `platformio.ini`:
```ini
board_build.partitions = partitions_custom.csv
```

## License

This project is open source. Feel free to modify and distribute.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Acknowledgments

- ESP8266Audio library by Earle F. Philhower III
- ESP32-A2DP library by Phil Schatzmann
- ESP32 community and documentation
