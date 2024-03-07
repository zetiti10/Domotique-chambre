/**
 * @file device/input/IRSensor.cpp
 * @author Louis L
 * @brief Classe représentant un capteur d'ondes infrarouges.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/input/IRSensor.hpp"
#include "device/input/input.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param pin La broche de l'Arduino liée au capteur infrarouge.
IRSensor::IRSensor(String friendlyName, int ID, HomeAssistant &connection, int pin) : Input(friendlyName, ID, connection), m_pin(pin), m_sensor(pin), m_lastTime(0) {}

/// @brief Initialise l'objet.
void IRSensor::setup()
{
    if (m_operational)
        return;

    Input::setup();

    m_sensor.enableIRIn();

    m_lastTime = millis();

    m_operational = true;

    m_connection.updateDeviceAvailability(m_ID, true);
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
            // switchTV(TOGGLE, true); // Le constructeur pourrait prendre en paramètre une méthode à éxecuter lors d'un clic.
            break;

        case 4261480199: // Source.
            /*if (LEDCubeState == LOW && multicolorState == false)
            {
                switchMulticolor(SWITCH_ON, false);
                switchLEDCube(SWITCH_ON, false);
                displayDeviceState(true);
            }

            else
            {
                switchMulticolor(SWITCH_OFF, false);
                switchLEDCube(SWITCH_OFF, false);
                displayDeviceState(false);
            }*/
            break;

        case 4161210119: // Vol+.
            // volumeSono(INCREASE, true);
            break;

        case 4094363399: // Vol-.
            // volumeSono(DECREASE, true);
            break;

        case 4027516679: // Mute.
            // volumeSono(TOGGLE_MUTE, true);
            break;
        }
    }

    m_sensor.resume();
}