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

class Television : public Output
{
public:
    Television(const __FlashStringHelper* friendlyName, int ID, HomeAssistant &connection, Display &display, int servomotorPin, int IRLEDPin, int volume);
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

protected:
    virtual void moveDisplayServo(int angle);
    virtual void switchDisplay();
    virtual void detectTriggerSound();

    const int m_servomotorPin;
    const int m_IRLEDPin;
    IRsend m_IRSender;
    int m_volume;
    bool m_volumeMuted;
    unsigned long m_lastTime;
};

#endif