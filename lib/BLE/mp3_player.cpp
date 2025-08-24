#include "mp3_player.h"
#include "SD.h"
#include <Arduino.h>


MP3PlayerInterface::MP3PlayerInterface()
    : file(nullptr), mp3(nullptr), out(nullptr) {}

MP3PlayerInterface::~MP3PlayerInterface() {
    stop();
}

void MP3PlayerInterface::play(const char* filename) {
    stop(); 
    out = new AudioOutputI2SNoDAC();
    mp3 = new AudioGeneratorMP3();

    out->SetPinout(22, 25, 26);
    out->SetChannels(1);
    out->SetGain(0.8);
}

void MP3PlayerInterface::stop() {
    if (mp3) {
        mp3->stop();
        delete mp3;
        mp3 = nullptr;
    }
    if (file) {
        delete file;
        file = nullptr;
    }
    if (out) {
        delete out;
        out = nullptr;
    }
    Serial.println("MP3 playback stopped!");
}

bool MP3PlayerInterface::isRunning() const {
    return mp3 && mp3->isRunning();
}


void MP3PlayerLittleFS::play(const char* filename) {
    file = new AudioFileSourceLittleFS(filename);
    MP3PlayerInterface::play(filename);
    mp3->begin(file, out);
    Serial.println("LittleFS MP3 playback started!");
}


MP3PlayerSD::MP3PlayerSD(uint8_t sdCsPin)
    : SD_CS_PIN(sdCsPin), sdCardAvailable(false) {
    initSDCard();
}

void MP3PlayerSD::play(const char* filename) {
    if (!sdCardAvailable) {
        Serial.println("SD card not available!");
        return;
    }
    file = new AudioFileSourceSD(filename);
    MP3PlayerInterface::play(filename);
    mp3->begin(file, out);
    Serial.println("SD MP3 playback started!");
}

void MP3PlayerSD::initSDCard() {
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD card initialization failed!");
        sdCardAvailable = false;
    } else {
        sdCardAvailable = true;
        Serial.println("SD card initialized successfully!");
    }
}
