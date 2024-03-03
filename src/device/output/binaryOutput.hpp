#ifndef BINARY_OUTPUT_DEFINITIONS
#define BINARY_OUTPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

// Classe représentant un périphérique de sortie basique.
class BinaryOutput : public Output
{
public:
    BinaryOutput(String friendlyName, int ID, HomeAssistant &connection, Display &display, int relayPin);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;

protected:
    const int m_relayPin;
};

#endif