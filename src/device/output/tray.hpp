#ifndef TRAY_DEFINITIONS
#define TRAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

class Tray : public Output
{
public:
    Tray(String friendlyName, Display &display, int motorPin1, int motorPin2, int speedPin);
    virtual void setup() override;
    virtual void turnOn(boolean shareInformation = false) override;
    virtual void turnOff(boolean shareInformation = false) override;

protected:
    int m_motorPin1;
    int m_motorPin2;
    int m_speedPin;
};

#endif