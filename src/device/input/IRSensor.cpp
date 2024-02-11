/**
 * @file device/input/IRSensor.cpp
 * @author Louis L
 * @brief Classe représentant un capteur d'ondes infrarouges.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "IRSensor.hpp"
#include "../../logger.hpp"

IRSensor::IRSensor(String friendlyName, int pin) : Input(friendlyName), m_pin(pin), m_sensor(pin), m_counter(0) {}

void IRSensor::setup()
{
    if (m_operational)
        return;

    m_sensor.enableIRIn();

    m_counter = millis();

    m_operational = true;

    sendLogMessage(INFO, "Le capteur d'ondes infrarouges '" + m_friendlyName + "' est initialisé à la broche " + m_pin + ".");
}

void IRSensor::loop()
{
    if (m_operational && m_sensor.decode() && ((millis() - m_counter) >= 250))
    {
        m_counter = millis();

        switch (m_sensor.decodedIRData.decodedRawData)
        {

        case 4244768519: // ON/OFF.
            // switchTV(TOGGLE, true);
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