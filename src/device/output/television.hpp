#ifndef TELEVISION_DEFINITIONS
#define TELEVISION_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "utils/readPROGMEMString.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/connectedOutput.hpp"
#include "device/input/analogInput.hpp"

/// @brief Structure stockant une liste d'actions associées à un temps pour une musique.
struct Action
{
    unsigned long timecode;
    char action[32];
};

/// @brief Structure stockant une musique pour le système de musique animée.
struct Music
{
    const char *friendlyName;
    const char *videoURL;
    const Action *actionList;
    unsigned int actionsNumber;
};

/// @brief Classe représentant une télévision.
class Television : public Output
{
public:
    Television(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, int servomotorPin, int IRLEDPin, int volume, MusicsAnimationsMode &mode);
    virtual void setMusicDevices(Output *deviceList[], unsigned int devicesNumber);
    virtual void setMusicsList(const Music *const *musicList, unsigned int musicsNumber);
    virtual void setMicrophone(AnalogInput &microphone);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void loop() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void syncVolume(bool shareInformation = false);
    virtual void increaseVolume(bool shareInformation = false);
    virtual void decreaseVolume(bool shareInformation = false);
    virtual unsigned int getVolume() const;
    virtual void mute(bool shareInformation = false);
    virtual void unMute(bool shareInformation = false);
    virtual void toggleMute(bool shareInformation = false);
    virtual bool getMute() const;
    virtual const Music *const *getMusicsList() const;
    virtual unsigned int getMusicNumber() const;
    virtual Music getMusicFromIndex(unsigned int index);
    virtual void playMusic(unsigned int musicIndex);
    virtual void stopMusic();
    virtual void shutdown() override;

protected:
    virtual void moveDisplayServo(unsigned int angle);
    virtual void switchDisplay();
    virtual int detectTriggerSound();
    virtual void scheduleMusic();
    virtual Output *getDeviceFromID(unsigned int ID);
    virtual Action getAction(Music music, unsigned int actionIndex);

    static String addZeros(unsigned int number, unsigned int length);
    static unsigned int getIntFromString(String &string, unsigned int position, unsigned int lenght);

    const int m_servomotorPin;
    const int m_IRLEDPin;
    IRsend m_IRSender;
    unsigned int m_volume;
    bool m_volumeMuted;
    unsigned long m_lastTime;
    bool m_waitingForTriggerSound;
    AnalogInput *m_microphone;
    unsigned long m_musicStartTime;
    unsigned int m_lastActionIndex;
    const Music *const *m_musicList;
    int m_currentMusicIndex;
    unsigned int m_musicsNumber;
    Output **m_deviceList;
    unsigned int m_devicesNumber;
    MusicsAnimationsMode &m_mode;
    unsigned long m_detectionStartTime;
};

#endif