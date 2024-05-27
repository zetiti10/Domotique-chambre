#ifndef TELEVISION_DEFINITIONS
#define TELEVISION_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>
#include <arduinoFFT.h>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/connectedOutput.hpp"
#include "device/input/analogInput.hpp"

struct Action
{
    unsigned long timecode;
    const __FlashStringHelper *action;
};

struct Music
{
    const __FlashStringHelper *friendlyName;
    const __FlashStringHelper *videoURL;
    const Action *actionList;
    unsigned int actionsNumber;
};

class Television : public Output
{
public:
    Television(const __FlashStringHelper *friendlyName, int ID, HomeAssistant &connection, Display &display, int servomotorPin, int IRLEDPin, int volume, MusicsAnimationsMode &mode);
    virtual void setMusicDevices(Output *deviceList[], int &devicesNumber);
    virtual void setMusicsList(Music **musicList, int &musicsNumber);
    virtual void setMicrophone(AnalogInput &microphone);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void loop();
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void syncVolume(bool shareInformation = false);
    virtual void increaseVolume(bool shareInformation = false);
    virtual void decreaseVolume(bool shareInformation = false);
    virtual int getVolume();
    virtual void mute(bool shareInformation = false);
    virtual void unMute(bool shareInformation = false);
    virtual void toggleMute(bool shareInformation = false);
    virtual bool getMute();
    virtual Music **getMusicsList();
    virtual int getMusicNumber();
    virtual void playMusic(int musicIndex);
    virtual void stopMusic();

protected:
    virtual void moveDisplayServo(int angle);
    virtual void switchDisplay();
    virtual void detectTriggerSound();
    virtual void scheduleMusic();
    virtual Output *getDeviceFromID(int ID);
    static String addZeros(int number, int length);
    static int getIntFromString(String &string, int position, int lenght);

    const int m_servomotorPin;
    const int m_IRLEDPin;
    IRsend m_IRSender;
    int m_volume;
    bool m_volumeMuted;
    unsigned long m_lastTime;
    bool m_waitingForTriggerSound;
    AnalogInput *m_microphone;
    unsigned long m_musicStartTime;
    unsigned int m_lastActionIndex;
    Music **m_musicList;
    int m_currentMusicIndex;
    int m_musicsNumber;
    Output **m_deviceList;
    int m_devicesNumber;
    MusicsAnimationsMode m_mode;
};

#endif