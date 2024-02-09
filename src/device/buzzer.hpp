#ifndef BUZZER_DEFINITIONS
#define BUZZER_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device.hpp"

class Buzzer : public Device
{
public:
    Buzzer(String friendlyName, int pin);
    virtual void setup() override;
    virtual void clickSound();
    virtual void yesSound();
    virtual void noSound();
    virtual void doorbellMusic();

protected:
    int m_pin;
};

#endif