#ifndef ANALOG_INPUT_DEFINITIONS
#define ANALOG_INPUT_DEFINITIONS

// Autres fichiers du programme.
#include "input.hpp"

// Classe représentant un capteur dont la valeur mesurée est analogique.
class AnalogInput : public Input
{
public:
    AnalogInput(String friendlyName, int ID, int pin);
    virtual void setup() override;
    virtual void loop() override;
    virtual int getValue();

protected:
    int m_value;
    const int m_pin;
};

#endif