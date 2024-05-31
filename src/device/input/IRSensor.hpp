#ifndef IR_SENSOR_DEFINITIONS
#define IR_SENSOR_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "device/input/input.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Classe implémentant la gestion d'un capteur infrarouge.
class IRSensor : public Input
{
public:
    IRSensor(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void loop() override;

protected:
    const unsigned int m_pin;
    IRrecv m_sensor;
    unsigned long m_lastTime;
};

#endif