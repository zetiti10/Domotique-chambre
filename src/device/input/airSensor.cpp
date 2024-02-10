/**
 * @file device/input/airSensor.cpp
 * @author Louis L
 * @brief Classe représentant un capteur DHT22.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <DHT_U.h>

// Autres fichiers du programme.
#include "airSensor.hpp"
#include "../../logger.hpp"

AirSensor::AirSensor(String friendlyName, int pin) : Input(friendlyName), m_sensor(pin, DHT22), m_temperature(0), m_humidity(0) {}

void AirSensor::setup()
{
    m_sensor.begin();
    sensors_event_t event;
    m_sensor.temperature().getEvent(&event);
    if (isnan(event.temperature))
        sendLogMessage(ERROR, "Le capteur de l'air '" + m_friendlyName + "' n'a pas pu être initialisé à la broche " + m_pin + ".");

    else
    {
        m_operational = true;

        m_temperature = event.temperature;

        m_sensor.humidity().getEvent(&event);
        m_humidity = event.relative_humidity;

        m_lastMeasure = millis();

        sendLogMessage(INFO, "Le capteur de l'air '" + m_friendlyName + "' a été initialisé à la broche " + m_pin + ".");
    }
}

void AirSensor::loop()
{
    if (m_operational && ((millis() - m_lastMeasure) >= 60000))
    {
        sensors_event_t event;
        m_sensor.temperature().getEvent(&event);
        if (isnan(event.temperature))
        {
            m_operational = false;

            sendLogMessage(ERROR, "Une erreur est survenue lors de la lecture du capteur de l'air '" + m_friendlyName + "'. Le préiphérique est désactivé.");
        }

        else
        {
            m_temperature = event.temperature;

            m_sensor.humidity().getEvent(&event);
            m_humidity = event.relative_humidity;

            m_lastMeasure = millis();

            sendLogMessage(INFO, "Le capteur de l'air '" + m_friendlyName + "' a récupéré une température de " + m_temperature + "°C, et une humidité relative de " + m_humidity + "%.");
        }
    }
}

float AirSensor::getTemperature() const
{
    return m_temperature;
}

float AirSensor::getHumidity() const
{
    return m_humidity;
}