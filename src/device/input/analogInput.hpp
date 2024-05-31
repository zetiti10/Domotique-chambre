#ifndef ANALOG_INPUT_DEFINITIONS
#define ANALOG_INPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/input/input.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Classe représentant un capteur dont la valeur mesurée est analogique.
class AnalogInput : public Input
{
public:
    AnalogInput(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool connected);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void loop() override;
    virtual unsigned int getValue();

protected:
    unsigned int m_value;
    const unsigned int m_pin;
    bool m_connected;
    unsigned long m_lastTime;
};

#endif