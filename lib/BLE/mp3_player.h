#ifndef MP3_PLAYER_H
#define MP3_PLAYER_H

#include "FS.h"
#include "LittleFS.h"
#include "AudioFileSourceLittleFS.h"
#include "AudioFileSourceSD.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2SNoDAC.h"
#include "driver/i2s.h"

class MP3PlayerInterface {
public:
    MP3PlayerInterface();
    virtual ~MP3PlayerInterface();

    virtual void play(const char* filename);
    virtual void stop();
    bool isRunning() const;

protected:
    AudioFileSource* file;
    AudioGeneratorMP3* mp3;
    AudioOutputI2SNoDAC* out;
};

class MP3PlayerLittleFS : public MP3PlayerInterface {
public:
    void play(const char* filename) override;
};

class MP3PlayerSD : public MP3PlayerInterface {
public:
    MP3PlayerSD(uint8_t sdCsPin = 5);
    virtual ~MP3PlayerSD();

    void play(const char* filename) override;
    void initSDCard();

private:
    bool sdCardAvailable;
    uint8_t SD_CS_PIN;
};

#endif // MP3_PLAYER_H
