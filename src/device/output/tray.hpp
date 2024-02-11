#ifndef TRAY_DEFINITIONS
#define TRAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

// Classe gérant un plateau.
class Tray : public Output
{
public:
    Tray(String friendlyName, Display &display, int motorPin1, int motorPin2, int speedPin);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;

protected:
    const int m_motorPin1;
    const int m_motorPin2;
    const int m_speedPin;
};

#endif