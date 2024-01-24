#ifndef ANALOG_INPUT_DEFINITIONS
#define ANALOG_INPUT_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "input.hpp"

class AnalogInput : public Input
{
public:
    AnalogInput(String friendlyName, int pin);
    virtual void setup() override;
    virtual void loop() override;
    virtual int getValue();

protected:
    int m_value;
    int m_pin;
};

#endif