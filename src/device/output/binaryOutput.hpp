#ifndef BINARY_OUTPUT_DEFINITIONS
#define BINARY_OUTPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

class BinaryOutput : public Output
{
public:
    BinaryOutput(String friendlyName, int relayPin);
    virtual void setup();
    virtual void turnOn(boolean shareInformation = false);
    virtual void turnOff(boolean shareInformation = false);
    virtual void toggle(boolean shareInformation = false);

protected:
    int m_relayPin;
};

#endif