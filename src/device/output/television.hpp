#ifndef TELEVISION_DEFINITIONS
#define TELEVISION_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "output.hpp"

class Television : public Output
{
public:
    Television(String friendlyName, Display &display, int servomotorPin, int IRLEDPin);
    virtual void setup() override;
    virtual void turnOn(boolean shareInformation = false) override;
    virtual void turnOff(boolean shareInformation = false) override;
    virtual void syncVolume(boolean shareInformation = false);
    virtual void increaseVolume(boolean shareInformation = false);
    virtual void decreaseVolume(boolean shareInformation = false);
    virtual int getVolume();
    virtual void mute(boolean shareInformation = false);
    virtual void unMute(boolean shareInformation = false);
    virtual void toggleMute(boolean shareInformation = false);
    virtual boolean getMute();

protected:
    virtual void moveDisplayServo(int angle);
    virtual void switchDisplay();
    int m_servomotorPin;
    int m_IRLEDPin;
    IRsend m_IRSender;
    int m_volume;
    boolean m_volumeMuted;
};

#endif