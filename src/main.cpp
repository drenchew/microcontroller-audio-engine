#include "FS.h"
#include "LittleFS.h"
#include "SD.h"
#include "SPI.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "BluetoothA2DPSink.h"
#include "driver/i2s.h"



#include "mp3_player.h"
#include "ble_wrapper.h"

PlaybackManager playbackManager =  PlaybackManager();

enum AudioMode {
  MODE_MP3,
  MODE_MP3_SD,
  MODE_BLUETOOTH,
  MODE_IDLE,  
  ERROR_STATE
};



void printAvailableCommands() {
  Serial.println("\n=== Available Commands ===");
  Serial.println("p or play    - Play MP3 files from LittleFS");
  Serial.println("sd           - Play MP3 files from SD card");
  Serial.println("b or bt      - Start Bluetooth audio");
  Serial.println("s or stop    - Stop current audio");
  Serial.println("info         - Show system information");
  Serial.println("help         - Show this help");
}

std::string mapCurrentModeToString(AudioMode mode) {
  switch (mode) {
    case MODE_MP3: return "MP3 (LittleFS)";
    case MODE_MP3_SD: return "MP3 (SD Card)";
    case MODE_BLUETOOTH: return "Bluetooth";
    case MODE_IDLE: return "Idle";
    default: return "Unknown";
  }
}

void handleSerialCommands() {
  if (Serial.available()) {
    String cmd = Serial.readString();
    cmd.trim();
    cmd.toLowerCase();

    if (cmd == "p"){}
    else if (cmd == "sd") {}
    else if (cmd == "b") {}
    else if (cmd == "s") {}
    else if (cmd == "info") {
      Serial.println("\n=== System Info ===");
      Serial.printf("Current mode: %s\n", 
      mapCurrentModeToString(playbackManager.getCurrentMode()).c_str());
      Serial.printf("Bluetooth enabled: %s\n", playbackManager.isBluetoothEnabled() ? "Yes" : "No");
      Serial.printf("SD Card available: %s\n", playbackManager.isAvailable() ? "Yes" : "No");
    }
  }
}





class PlaybackManager {
  private:
    MP3PlayerInterface* player;
    BLEWrapper *bleWrapper;
    AudioMode currentMode = MODE_IDLE;

  public:
    PlaybackManager() : player(nullptr), bleWrapper(new BLEWrapper("ESP32_Audio_Player")) {}
    PlaybackManager(const AudioMode mode) : currentMode(mode) {
      switch (mode)
      {
      case MODE_MP3:
        player = new MP3PlayerLittleFS();
        break;
      case MODE_MP3_SD:
        player = new MP3PlayerSD(SD_CS_PIN);
        break;
      case MODE_BLUETOOTH:
        bleWrapper->start();
        break;
      case MODE_IDLE:
        bleWrapper->start();
        break;
      
      default:
        Serial.println("Unknown mode, cannot initialize player.");
        player = nullptr;
        bleWrapper->stop();
        break;
      }
    }

    bool isAvailable() const {
      return player && player->isAvailable();
    }

    AudioMode getCurrentMode() const {
      return currentMode;
    }
    bool isBluetoothEnabled() const {
      return bleWrapper && bleWrapper->isConnected();
    }
    bool isSDCardAvailable() const {
      return false;;
    }

    void play(const char* filename) {
      if (player) {
        player->play(filename);
      }
      else {
        Serial.println("Player not initialized.");
      }
    }

    void playAllFromSD() {
      if (currentMode != MODE_MP3_SD) {
        Serial.println("Current mode is not SD MP3. Cannot play from SD.");
        return;
      }
      if (player) {
        player->play("/");
        File root = SD.open("/");
        File file = root.openNextFile();
        while (file) {
          String name = file.name();
          if (name.endsWith(".mp3") || name.endsWith(".MP3")) {
            Serial.printf("Playing from SD: %s\n", name.c_str());
            player->play(name.c_str());
            while (player->isRunning()) {
              delay(100);
            }
          }
          file = root.openNextFile();
        }
      } else {
        Serial.println("Player not initialized.");
      }
    }

    ~PlaybackManager() {
      if (player) {
        delete player;
        player = nullptr;
      }
      if (bleWrapper) {
        if (bleWrapper->isConnected()) {
          bleWrapper->stop();
        }
        delete bleWrapper;
        bleWrapper = nullptr;
      }
    }
  
};


void setup() {
  Serial.begin(115200);


  Serial.println("=== ESP32 Multi-Audio Player ===");
  Serial.println("Features: MP3 Files (LittleFS + SD Card) + Bluetooth Audio");
  
  

  
  Serial.println("\n=== Available Commands ===");
  Serial.println("p            - Play MP3 files from LittleFS");
  Serial.println("sd           - Play MP3 files from SD card");
  Serial.println("b            - Start Bluetooth audio");
  Serial.println("s            - Stop current audio");
  Serial.println("info         - Show system information");
  Serial.println("help         - Show this help");
  
  Serial.println("\nReady! Type 'p' to play MP3 or 'help' for commands");
}

void loop() {

 handleSerialCommands();


  delay(10);
}
