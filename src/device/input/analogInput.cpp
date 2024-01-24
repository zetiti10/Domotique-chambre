/**
 * @file device/input/analogInput.cpp
 * @author Louis L
 * @brief Classe représentant un capteur dont l'état est anlogique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "analogInput.hpp"
#include "../../logger.hpp"

AnalogInput::AnalogInput(String friendlyName, int pin) : Input(friendlyName), m_value(0), m_pin(pin) {}

void AnalogInput::setup()
{
    pinMode(m_pin, INPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le capteur analogique '" + m_friendlyName + "' est initialisé à la broche " + m_pin + ".");
}

int AnalogInput::getValue()
{
    if (m_operational)
        m_value = analogRead(m_pin);

    return m_value;
}

void AnalogInput::loop() {}