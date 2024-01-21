#ifndef TRAY_DEFINITIONS
#define TRAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

class Tray : public Output
{
public:
    Tray(String friendlyName, int motorPin1, int motorPin2);
    virtual void setup();
    virtual void turnOn(boolean shareInformation = false);
    virtual void turnOff(boolean shareInformation = false);

protected:
    int m_motorPin1;
    int m_motorPin2;
};

#endif