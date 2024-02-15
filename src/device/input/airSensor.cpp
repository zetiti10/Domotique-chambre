/**
 * @file device/input/airSensor.cpp
 * @author Louis L
 * @brief Classe représentant un capteur DHT22.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "airSensor.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin Broche liée au capteur.
AirSensor::AirSensor(String friendlyName, int pin) : Input(friendlyName), m_pin(pin), m_sensor(pin, DHT22), m_temperature(0), m_humidity(0), m_lastTime(0) {}

/// @brief Initialise l'objet.
void AirSensor::setup()
{
    if (m_operational)
        return;

    m_sensor.begin();
    sensors_event_t event;
    m_sensor.temperature().getEvent(&event);
    if (!isnan(event.temperature))
    {
        m_operational = true;

        m_temperature = event.temperature;

        m_sensor.humidity().getEvent(&event);
        m_humidity = event.relative_humidity;

        m_lastTime = millis();
    }
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void AirSensor::loop()
{
    if (m_operational && ((millis() - m_lastTime) >= 60000))
    {
        sensors_event_t event;
        m_sensor.temperature().getEvent(&event);
        if (isnan(event.temperature))
            m_operational = false;

        else
        {
            m_temperature = event.temperature;

            m_sensor.humidity().getEvent(&event);
            m_humidity = event.relative_humidity;

            m_lastTime = millis();
        }
    }
}

/// @brief Méthode permettant de récupérer la température actuelle.
/// @return La température actuelle.
float AirSensor::getTemperature() const
{
    return m_temperature;
}

/// @brief Méthode permettant de récupérer l'humidité actuelle.
/// @return L'humidité actuelle.
float AirSensor::getHumidity() const
{
    return m_humidity;
}