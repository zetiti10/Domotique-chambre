#ifndef ANALOG_INPUT_DEFINITIONS
#define ANALOG_INPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/input/input.hpp"
#include "device/interface/HomeAssistant.hpp"

// Classe représentant un capteur dont la valeur mesurée est analogique.
class AnalogInput : public Input
{
public:
    AnalogInput(String friendlyName, int ID, HomeAssistant &connection, int pin);
    virtual void setup() override;
    virtual void loop() override;
    virtual int getValue();

protected:
    int m_value;
    const int m_pin;
};

#endif