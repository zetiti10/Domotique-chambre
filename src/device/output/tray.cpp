/**
 * @file device/output/tray.cpp
 * @author Louis L
 * @brief Classe représentant la motorisation du plateau du bureau.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "tray.hpp"
#include "../../logger.hpp"

Tray::Tray(String friendlyName, Display &display, int motorPin1, int motorPin2, int speedPin) : Output(friendlyName, display), m_motorPin1(motorPin1), m_motorPin2(motorPin2), m_speedPin(speedPin) {}

void Tray::setup()
{
    if (m_operational)
        return;

    pinMode(m_motorPin1, OUTPUT);
    pinMode(m_motorPin2, OUTPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le plateau '" + m_friendlyName + "' est initialisé aux broches " + m_motorPin1 + " et " + m_motorPin2 + ".");
}

void Tray::turnOn(bool shareInformation)
{
    if (m_operational && !m_locked && !m_state)
    {
        analogWrite(m_speedPin, 120);
        digitalWrite(m_motorPin1, LOW);
        digitalWrite(m_motorPin2, HIGH);

        m_display.displayTray(shareInformation, true);

        digitalWrite(m_motorPin1, HIGH);
        digitalWrite(m_motorPin2, HIGH);
        analogWrite(m_speedPin, 120);

        m_state = true;

        sendLogMessage(INFO, "Le plateau '" + m_friendlyName + "' est ouvert.");
    }
}

void Tray::turnOff(bool shareInformation)
{
    if (m_operational && !m_locked && m_state)
    {
        analogWrite(m_speedPin, 120);
        digitalWrite(m_motorPin1, HIGH);
        digitalWrite(m_motorPin2, LOW);

        m_display.displayTray(shareInformation, false);

        digitalWrite(m_motorPin1, HIGH);
        digitalWrite(m_motorPin2, HIGH);
        analogWrite(m_speedPin, 120);

        m_state = false;

        sendLogMessage(INFO, "Le plateau '" + m_friendlyName + "' est fermé.");
    }
}