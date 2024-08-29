/**
 * @file device/input/IRSensor.cpp
 * @author Louis L
 * @brief Classe représentant un capteur d'ondes infrarouges.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "IRSensor.hpp"
#include "device/input/input.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param pin La broche de l'Arduino liée au capteur infrarouge.
IRSensor::IRSensor(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, Television &television, Output *deviceList[], int devicesNumber) : Input(friendlyName, ID, connection), m_pin(pin), m_sensor(pin), m_lastTime(0), m_television(television), m_deviceList(deviceList), m_devicesNumber(devicesNumber) {}

/// @brief Initialise l'objet.
void IRSensor::setup()
{
    if (m_operational)
        return;

    Input::setup();
    m_sensor.enableIRIn();

    m_television.setup();
    if (!m_television.getAvailability())
        return;

    for (int i = 0; i < m_devicesNumber; i++)
    {
        m_deviceList[i]->setup();
        if (!m_deviceList[i]->getAvailability())
            return;
    }

    m_lastTime = millis();
    m_operational = true;
    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void IRSensor::reportState()
{
}

/// @brief Boucle d'exécution des tâches liées au capteur.
void IRSensor::loop()
{
    if (m_operational && m_sensor.decode() && ((millis() - m_lastTime) >= 250))
    {
        m_lastTime = millis();

        switch (m_sensor.decodedIRData.decodedRawData)
        {

        case 4244768519: // ON/OFF.
        {
            m_television.toggle(true);
            break;
        }

        case 4261480199: // Source.
        {
            bool devicesOff = true;
            for (int i = 0; i < m_devicesNumber; i++)
            {
                if (m_deviceList[i]->getState())
                    devicesOff = false;
            }

            if (devicesOff)
            {
                for (int i = 0; i < m_devicesNumber; i++)
                    m_deviceList[i]->turnOn(true);
            }

            else
            {
                for (int i = 0; i < m_devicesNumber; i++)
                    m_deviceList[i]->turnOff(true);
            }
            break;
        }

        case 4161210119: // Vol+.
        {
            m_television.increaseVolume(true);
            break;
        }

        case 4094363399: // Vol-.
        {
            m_television.decreaseVolume(true);
            break;
        }

        case 4027516679: // Mute.
        {
            m_television.toggleMute(true);
            break;
        }

        default:
            break;
        }
    }

    m_sensor.resume();
}