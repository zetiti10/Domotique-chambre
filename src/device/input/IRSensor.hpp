#ifndef IR_SENSOR_DEFINITIONS
#define IR_SENSOR_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "input.hpp"

// Classe implémentant la gestion d'un capteur infrarouge.
class IRSensor : public Input
{
public:
    IRSensor(String friendlyName, int ID, HomeAssistant connection, int pin);
    virtual void setup() override;
    virtual void loop() override;

protected:
    const int m_pin;
    IRrecv m_sensor;
    unsigned long m_lastTime;
};

#endif