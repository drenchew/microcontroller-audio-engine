#include "FS.h"
#include "LittleFS.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "BluetoothA2DPSink.h"
#include "driver/i2s.h"

AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2SNoDAC *out;

// internal dac by default
BluetoothA2DPSink a2dp_sink;


enum AudioMode {
  MODE_MP3,
  MODE_BLUETOOTH,
  MODE_IDLE
};

AudioMode currentMode = MODE_IDLE;
bool bluetoothEnabled = false;


void stopMP3();
void startBluetoothAudio();
void stopBluetoothAudio();

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("- failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("- not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("\tSIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void startBluetoothAudio() {
  if (currentMode == MODE_BLUETOOTH) return;
  
 
  stopMP3();
  
  Serial.println("Starting Bluetooth Audio...");
  currentMode = MODE_BLUETOOTH;
  
 
  Serial.println("Initializing Bluetooth A2DP Sink for Internal DAC...");
  Serial.println("Device name: ESP32_Audio_Player");
  
  // Configure I2S for internal DAC (built-in DAC mode)
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_MSB,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  
  a2dp_sink.set_i2s_config(i2s_config);
  
  
  a2dp_sink.start("ESP32_Audio_Player");
  
  bluetoothEnabled = true;
  
  Serial.println("Bluetooth Audio started!");
  Serial.println("Audio configured for internal DAC on pins 25 and 26");
  Serial.println("Ready to pair - look for 'ESP32_Audio_Player' in your phone's Bluetooth settings");
}

void stopBluetoothAudio() {
  if (!bluetoothEnabled) return;
  
  Serial.println("Stopping Bluetooth Audio...");
  
  // Stop A2DP sink
  a2dp_sink.end();
  
  bluetoothEnabled = false;
  currentMode = MODE_IDLE;
  Serial.println("Bluetooth Audio stopped!");
}

void playMP3(const char* filename) {
  if (currentMode == MODE_MP3) return;
  
  stopBluetoothAudio();
  
  Serial.printf("Starting MP3 playback: %s\n", filename);
  currentMode = MODE_MP3;
  

  file = new AudioFileSourceLittleFS(filename);
  out = new AudioOutputI2SNoDAC();
  mp3 = new AudioGeneratorMP3();
  
 
  out->SetPinout(22, 25, 26); // bclk, wclk, dout (using internal DAC pins)
  out->SetChannels(1); // Mono output
  out->SetGain(0.8); // Adjust volume (0.0 to 1.0)
  
 
  mp3->begin(file, out);
  
  Serial.println("MP3 playback started!");
}

void stopMP3() {
  if (mp3) {
    mp3->stop();
    delete mp3;
    delete file;
    delete out;
    mp3 = nullptr;
    file = nullptr;
    out = nullptr;
    currentMode = MODE_IDLE;
    Serial.println("MP3 playback stopped!");
  }
}

void setup() {
  Serial.begin(115200);
  
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed!");
    return;
  }

  Serial.println("=== ESP32 Multi-Audio Player (DAC Pin 25) ===");
  Serial.println("Features: MP3 Files + Bluetooth Audio");
  
  listDir(LittleFS, "/", 1);
  

  size_t totalBytes = LittleFS.totalBytes();
  size_t usedBytes = LittleFS.usedBytes();
  Serial.printf("Filesystem: %d/%d bytes used (%.1f%%)\n", 
                usedBytes, totalBytes, (usedBytes * 100.0) / totalBytes);
  Serial.printf("Available for audio: %.1f MB\n", (totalBytes - usedBytes) / 1024.0 / 1024.0);
  

  File root = LittleFS.open("/");
  File file = root.openNextFile();
  bool foundMP3 = false;
  
  while (file) {
    String name = file.name();
    if (name.endsWith(".mp3")) {
      size_t fileSize = file.size();
      Serial.printf("Found MP3: %s (%d bytes = %.1f KB)\n", 
                    name.c_str(), fileSize, fileSize / 1024.0);
      
      // Estimate duration (rough: ~1KB per second for typical MP3)
      float estimatedDuration = fileSize / 1024.0;
      Serial.printf("Estimated duration: %.1f seconds\n", estimatedDuration);
      foundMP3 = true;
    }
    file = root.openNextFile();
  }
  
  if (!foundMP3) {
    Serial.println("\n*** No MP3 files found! ***");
  }
  
  Serial.println("\n=== Available Commands ===");
  Serial.println("p or play    - Play MP3 files");
  Serial.println("b or bt      - Start Bluetooth audio");
  Serial.println("s or stop    - Stop current audio");
  Serial.println("info         - Show system information");
  Serial.println("help         - Show this help");
  
  Serial.println("\nReady! Type 'p' to play MP3 or 'help' for commands");
}

void loop() {

  if (currentMode == MODE_MP3 && mp3 && mp3->isRunning()) {
    if (!mp3->loop()) {
      Serial.println("MP3 playback finished");
      stopMP3();
      
      // Auto-restart MP3 after 2 seconds
      delay(2000);
      Serial.println("Restarting MP3 playback...");
      File root = LittleFS.open("/");
      File file = root.openNextFile();
      while (file) {
        String name = file.name();
        if (name.endsWith(".mp3")) {
          playMP3(("/" + name).c_str());
          break;
        }
        file = root.openNextFile();
      }
    }
  }
  

  if (Serial.available()) {
    String cmd = Serial.readString();
    cmd.trim();
    cmd.toLowerCase();
    
    if (cmd == "p" || cmd == "play") {
      // Switch to MP3 mode
      File root = LittleFS.open("/");
      File file = root.openNextFile();
      while (file) {
        String name = file.name();
        if (name.endsWith(".mp3")) {
          Serial.println("Switching to MP3 playback...");
          playMP3(("/" + name).c_str());
          break;
        }
        file = root.openNextFile();
      }
    } else if (cmd == "b" || cmd == "bt" || cmd == "bluetooth") {
      // Switch to Bluetooth mode
      Serial.println("Switching to Bluetooth mode...");
      startBluetoothAudio();
    } else if (cmd == "s" || cmd == "stop") {
      // Stop all audio
      Serial.println("Stopping all audio...");
      stopMP3();
      stopBluetoothAudio();
    } else if (cmd == "info") {
      Serial.println("\n=== System Info ===");
      Serial.printf("Current mode: %s\n", 
        currentMode == MODE_MP3 ? "MP3" : 
        currentMode == MODE_BLUETOOTH ? "Bluetooth" : "Idle");
      Serial.printf("Bluetooth enabled: %s\n", bluetoothEnabled ? "Yes" : "No");
      
      listDir(LittleFS, "/", 1);
      size_t totalBytes = LittleFS.totalBytes();
      size_t usedBytes = LittleFS.usedBytes();
      Serial.printf("Storage: %d/%d bytes (%.1f%% used)\n", 
                    usedBytes, totalBytes, (usedBytes * 100.0) / totalBytes);
    } else if (cmd == "help") {
      Serial.println("\n=== Available Commands ===");
      Serial.println("p or play    - Play MP3 files");
      Serial.println("b or bt      - Start Bluetooth audio");
      Serial.println("s or stop    - Stop current audio");
      Serial.println("info         - Show system information");
      Serial.println("help         - Show this help");
    } else {
      Serial.printf("Unknown command: %s\n", cmd.c_str());
      Serial.println("Type 'help' for available commands");
    }
  }
  
  // Small delay to prevent watchdog issues
  delay(10);
}
