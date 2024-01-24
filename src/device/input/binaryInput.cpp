/**
 * @file device/input/binaryInput.cpp
 * @author Louis L
 * @brief Classe représentant un capteur dont l'état est binaire.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "binaryInput.hpp"
#include "../../buzzer.hpp"
#include "../../logger.hpp"

BinaryInput::BinaryInput(String friendlyName, int pin, boolean revert, boolean pullup) : Input(friendlyName), m_state(false), m_pin(pin), m_reverted(revert), m_pullup(pullup) {}

void BinaryInput::setup()
{
    if (m_pullup)
        pinMode(m_pin, INPUT_PULLUP);

    else
        pinMode(m_pin, INPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le capteur binaire '" + m_friendlyName + "' est initialisé à la broche " + m_pin + ".");
}

void BinaryInput::loop() {}

boolean BinaryInput::getState()
{
    int previousState = m_state;

    if (m_reverted)
        m_state = !digitalRead(m_pin);

    else
        m_state = digitalRead(m_pin);

    if (previousState != m_state)
        sendLogMessage(INFO, "Le capteur binaire '" + m_friendlyName + "' a changé d'état : " + m_state + ".");

    return m_state;
}

WardrobeDoorSensor::WardrobeDoorSensor(String friendlyName, int pin, boolean revert, boolean pullup, BinaryOutput &output) : BinaryInput(friendlyName, pin, revert, pullup), m_output(output), m_activated(true) {}

void WardrobeDoorSensor::loop()
{
    if (m_activated && m_output.getAvailability() && (getState() != m_output.getState()))
    {
        if (getState())
            m_output.turnOn();

        else
            m_output.turnOff();
    }
}

void WardrobeDoorSensor::activate()
{
    m_activated = true;

    sendLogMessage(INFO, "Le mode automatique du capteur binaire de porte d'armoire '" + m_friendlyName + "' a été activé.");
}

void WardrobeDoorSensor::desactivate()
{
    m_activated = false;

    sendLogMessage(INFO, "Le mode automatique du capteur binaire de porte d'armoire '" + m_friendlyName + "' a été désactivé.");
}

boolean WardrobeDoorSensor::getActivation() const
{
    return m_activated;
}

/*DoorSensor::DoorSensor(String friendlyName, int pin, boolean revert, boolean pullup, Alarm &alarm) : BinaryInput(friendlyName, pin, revert, pullup), m_alarm(alarm) {}

void DoorSensor::loop()
{
    if (m_alarm.getAvailability() && m_alarm.getState() && getState)
    {
        m_alarm.trigger();

        sendLogMessage(INFO, "L'alarme '" + m_alarm.getFriendlyName() + "' a été déclenchée par le capteur de porte '" + m_friendlyName + "'.");
    }
}*/

Doorbell::Doorbell(String friendlyName, int pin, boolean revert, boolean pullup) : BinaryInput(friendlyName, pin, revert, pullup) {}

void Doorbell::loop()
{
    if (getState())
    {
        doorbellMusic();

        sendLogMessage(INFO, "La sonnette a été déclenchée par le bouton de sonnette '" + m_friendlyName + "'.");
    }
}