#ifndef BINARY_OUTPUT_DEFINITIONS
#define BINARY_OUTPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"
#include "../interface/display.hpp"

// Classe représentant un périphérique de sortie basique.
class BinaryOutput : public Output
{
public:
    BinaryOutput(String friendlyName, int ID, Display &display, HomeAssistant &connection, int relayPin);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;

protected:
    const int m_relayPin;
};

#endif