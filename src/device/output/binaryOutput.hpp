#ifndef BINARY_OUTPUT_DEFINITIONS
#define BINARY_OUTPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

class BinaryOutput : public Output
{
public:
    BinaryOutput(String friendlyName, Display &display, int relayPin);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;

protected:
    const int m_relayPin;
};

#endif