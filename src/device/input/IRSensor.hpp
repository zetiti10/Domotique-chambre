#ifndef IR_SENSOR_DEFINITIONS
#define IR_SENSOR_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "input.hpp"

class IRSensor : public Input
{
public:
    IRSensor(String friendlyName, int pin);
    virtual void setup() override;
    virtual void loop() override;

protected:
    int m_pin;
    IRrecv m_sensor;
    unsigned long m_counter;
};

#endif