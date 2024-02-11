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
#include "../buzzer.hpp"
#include "../../logger.hpp"

BinaryInput::BinaryInput(String friendlyName, int pin, bool revert, bool pullup) : Input(friendlyName), m_state(false), m_pin(pin), m_reverted(revert), m_pullup(pullup) {}

void BinaryInput::setup()
{
    if (m_operational)
        return;

    if (m_pullup)
        pinMode(m_pin, INPUT_PULLUP);

    else
        pinMode(m_pin, INPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le capteur binaire '" + m_friendlyName + "' est initialisé à la broche " + m_pin + ".");
}

void BinaryInput::loop() {}

bool BinaryInput::getState()
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

WardrobeDoorSensor::WardrobeDoorSensor(String friendlyName, int pin, bool revert, bool pullup, BinaryOutput &output) : BinaryInput(friendlyName, pin, revert, pullup), m_output(output), m_activated(true) {}

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

bool WardrobeDoorSensor::getActivation() const
{
    return m_activated;
}

DoorSensor::DoorSensor(String friendlyName, int pin, bool revert, bool pullup, Alarm &alarm) : BinaryInput(friendlyName, pin, revert, pullup), m_alarm(alarm) {}

void DoorSensor::loop()
{
    if (getState() && m_alarm.getAvailability() && m_alarm.getState())
    {
        m_alarm.trigger();

        sendLogMessage(INFO, "L'alarme '" + m_alarm.getFriendlyName() + "' a été déclenchée par le capteur de porte '" + m_friendlyName + "'.");
    }
}

Doorbell::Doorbell(String friendlyName, int pin, bool revert, bool pullup, Display &display, Buzzer &buzzer) : BinaryInput(friendlyName, pin, revert, pullup), m_display(display), m_buzzer(buzzer), m_delay(0) {}

void Doorbell::setup()
{
    BinaryInput::setup();

    m_display.setup();
    m_buzzer.setup();

    m_delay = millis();

    if (!m_display.getAvailability())
        sendLogMessage(WARN, "L'écran '" + m_display.getFriendlyName() + "' n'a pas pu être initialisé lors de l'initialisation de la sonnette '" + m_friendlyName + "'.");
}

void Doorbell::loop()
{
    if (getState() && ((millis() - 10000) >= m_delay))
    {
        m_display.displayBell();
        m_buzzer.doorbellMusic();

        m_delay = millis();

        sendLogMessage(INFO, "La sonnette '" + m_buzzer.getFriendlyName() + "' a été déclenchée par le bouton de sonnette '" + m_friendlyName + "'.");
    }
}