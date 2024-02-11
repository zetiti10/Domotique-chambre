#ifndef AIR_SENSOR_DEFINITIONS
#define AIR_SENSOR_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#include <DHT_U.h>

// Autres fichiers du programme.
#include "input.hpp"

class AirSensor : public Input
{
public:
    AirSensor(String friendlyName, int pin);
    virtual void setup() override;
    virtual void loop() override;
    virtual float getTemperature() const;
    virtual float getHumidity() const;

protected:
    const int m_pin;
    DHT_Unified m_sensor;
    float m_temperature;
    float m_humidity;
    unsigned long m_lastMeasure;
};

#endif