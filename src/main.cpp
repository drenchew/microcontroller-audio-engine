#include "FS.h"
#include "LittleFS.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "BluetoothA2DPSink.h"
#include "driver/i2s.h"

// consts 
#define COMMAND_BUFFER_SIZE     32U
#define SAMPLE_RATE            44100U
#define DMA_BUF_COUNT          8U
#define DMA_BUF_LEN            64U
#define AUDIO_GAIN             0.8f
#define SERIAL_BAUD_RATE       115200U
#define LOOP_DELAY_MS          10U
#define MAX_FILENAME_LEN       64U

//  pin definitions
#define I2S_BCLK_PIN           22U
#define I2S_WCLK_PIN           25U
#define I2S_DOUT_PIN           26U


#define DEVICE_NAME            "ESP32_Audio_Player"
#define MP3_EXTENSION          ".mp3"
#define ROOT_DIR               "/"


static AudioGeneratorMP3 mp3;
static AudioFileSourceLittleFS file;
static AudioOutputI2SNoDAC out;
static BluetoothA2DPSink a2dp_sink;


typedef enum {
    MODE_IDLE = 0U,
    MODE_MP3,
    MODE_BLUETOOTH
} audio_mode_t;


typedef enum {
    RESULT_OK = 0U,
    RESULT_ERROR,
    RESULT_INVALID_PARAM,
    RESULT_NOT_FOUND
} result_code_t;

static audio_mode_t current_mode = MODE_IDLE;
static bool bluetooth_enabled = false;


static void stop_mp3(void);
static result_code_t start_bluetooth_audio(void);
static void stop_bluetooth_audio(void);
static result_code_t play_mp3(const char* filename);
static void list_dir(fs::FS &fs, const char * dirname, uint8_t levels);
static void convert_to_lowercase(char* str, size_t len);
static result_code_t find_and_play_mp3(void);
static void print_system_info(void);
static void print_help(void);


static void list_dir(fs::FS &fs, const char * dirname, uint8_t levels) {
    if (dirname == NULL) {
        Serial.println("Invalid directory name");
        return;
    }
    
    Serial.printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if (!root || !root.isDirectory()) {
        Serial.println("- failed to open directory");
        return;
    }

    File entry = root.openNextFile();
    while (entry) {
        if (entry.isDirectory()) {
            Serial.printf("DIR  : %s\n", entry.name());
            if (levels > 0U) {
                list_dir(fs, entry.name(), levels - 1U);
            }
        } else {
            Serial.printf("File : %s\tSize : %u\n", entry.name(), (unsigned int)entry.size());
        }
        entry = root.openNextFile();
    }
}


static result_code_t start_bluetooth_audio(void) {
    if (current_mode == MODE_BLUETOOTH) {
        return RESULT_OK;
    }
    stop_mp3();

    Serial.println("Starting Bluetooth Audio...");
    current_mode = MODE_BLUETOOTH;

    // I2S config for internal DAC
    i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN),
        .sample_rate = SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
        .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_MSB,
        .intr_alloc_flags = 0,
        .dma_buf_count = DMA_BUF_COUNT,
        .dma_buf_len = DMA_BUF_LEN,
        .use_apll = false,
        .tx_desc_auto_clear = true,
        .fixed_mclk = 0
    };

    a2dp_sink.set_i2s_config(i2s_config);
    a2dp_sink.start(DEVICE_NAME);
    bluetooth_enabled = true;

    Serial.println("Bluetooth Audio started! Ready to pair.");
    return RESULT_OK;
}


static void stop_bluetooth_audio(void) {
    if (!bluetooth_enabled) {
        return;
    }
    Serial.println("Stopping Bluetooth Audio...");
    a2dp_sink.end();
    bluetooth_enabled = false;
    current_mode = MODE_IDLE;
}


static result_code_t play_mp3(const char* filename) {
    if (filename == NULL) {
        return RESULT_INVALID_PARAM;
    }
    
    if (current_mode == MODE_MP3) {
        return RESULT_OK;
    }
    stop_bluetooth_audio();

    Serial.printf("Starting MP3 playback: %s\n", filename);
    current_mode = MODE_MP3;

    file = AudioFileSourceLittleFS(filename);
    out = AudioOutputI2SNoDAC();
    mp3 = AudioGeneratorMP3();

    out.SetPinout(I2S_BCLK_PIN, I2S_WCLK_PIN, I2S_DOUT_PIN); // BCLK, WCLK, DOUT
    out.SetChannels(1);
    out.SetGain(AUDIO_GAIN);

    mp3.begin(&file, &out);
    return RESULT_OK;
}


static void stop_mp3(void) {
    if (mp3.isRunning()) {
        mp3.stop();
        current_mode = MODE_IDLE;
        Serial.println("MP3 playback stopped!");
    }
}


static void convert_to_lowercase(char* str, size_t len) {
    if (str == NULL) {
        return;
    }
    
    for (size_t i = 0; i < len; i++) {
        if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] += 32;
        }
    }
}


static result_code_t find_and_play_mp3(void) {
    File root = LittleFS.open(ROOT_DIR);
    if (!root) {
        Serial.println("Failed to open root directory");
        return RESULT_ERROR;
    }
    
    File entry = root.openNextFile();
    while (entry) {
        if (strstr(entry.name(), MP3_EXTENSION) != NULL) {
            result_code_t result = play_mp3(entry.name());
            return result;
        }
        entry = root.openNextFile();
    }
    
    Serial.println("No MP3 files found");
    return RESULT_NOT_FOUND;
}


static void print_system_info(void) {
    const char* mode_str;
    
    switch (current_mode) {
        case MODE_MP3:
            mode_str = "MP3";
            break;
        case MODE_BLUETOOTH:
            mode_str = "Bluetooth";
            break;
        case MODE_IDLE:
        default:
            mode_str = "Idle";
            break;
    }
    
    Serial.printf("Current mode: %s\n", mode_str);
    Serial.printf("Bluetooth enabled: %s\n", bluetooth_enabled ? "Yes" : "No");
}


static void print_help(void) {
    Serial.println("Commands: play/p, bt/b, stop/s, info, help");
}


void setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    
    if (!LittleFS.begin(true)) {
        Serial.println("LittleFS Mount Failed!");
        return;
    }

    Serial.println("=== ESP32 Multi-Audio Player ===");
    list_dir(LittleFS, ROOT_DIR, 1U);

    Serial.println("\nAvailable commands: play/p, bt/b, stop/s, info, help");
}


void loop() {
    static char cmd[COMMAND_BUFFER_SIZE];

    if (Serial.available()) {
        size_t len = Serial.readBytesUntil('\n', cmd, sizeof(cmd) - 1U);
        cmd[len] = '\0'; 

        convert_to_lowercase(cmd, len);

        if (strcmp(cmd, "play") == 0 || strcmp(cmd, "p") == 0) {
            result_code_t result = find_and_play_mp3();
            if (result != RESULT_OK) {
                Serial.println("Failed to start MP3 playback");
            }
        } else if (strcmp(cmd, "bt") == 0 || strcmp(cmd, "b") == 0) {
            result_code_t result = start_bluetooth_audio();
            if (result != RESULT_OK) {
                Serial.println("Failed to start Bluetooth audio");
            }
        } else if (strcmp(cmd, "stop") == 0 || strcmp(cmd, "s") == 0) {
            stop_mp3();
            stop_bluetooth_audio();
        } else if (strcmp(cmd, "info") == 0) {
            print_system_info();
        } else if (strcmp(cmd, "help") == 0) {
            print_help();
        } else {
            Serial.printf("Unknown command: %s\n", cmd);
        }
    }


    if (current_mode == MODE_MP3 && mp3.isRunning()) {
        if (!mp3.loop()) {
            Serial.println("MP3 playback finished");
            stop_mp3();
        }
    }

    delay(LOOP_DELAY_MS);
}
