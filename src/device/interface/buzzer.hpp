#ifndef BUZZER_DEFINITIONS
#define BUZZER_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/device.hpp"

// Classe intégrant les méthodes nécessaires pour émettre des sons avec le buzzer.
class Buzzer : public Device
{
public:
    Buzzer(const __FlashStringHelper* friendlyName, unsigned int ID, unsigned int pin);
    virtual void setup() override;
    virtual void clickSound();
    virtual void yesSound();
    virtual void noSound();
    virtual void doorbellMusic();
    virtual void shutdown() override;

protected:
    const unsigned int m_pin;
};

#endif