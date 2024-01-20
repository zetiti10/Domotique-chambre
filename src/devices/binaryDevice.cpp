/**
 * @file devices/device.cpp
 * @author Louis L
 * @brief Classe mère des périphériques du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "binaryDevice.hpp"

BinaryDevice::BinaryDevice(int relayPin) : m_relayPin(relayPin) {}

BinaryDevice::~BinaryDevice() {}

void BinaryDevice::setup()
{
    pinMode(m_relayPin, OUTPUT);

    m_operational = true;
}

void BinaryDevice::turnOn(boolean shareInformation)
{
    if (!m_state && m_operational && !m_locked)
    {
        digitalWrite(m_relayPin, HIGH);

        m_state = true;

        if (shareInformation)
        {
            // Affichage de l'animation d'allumage.
        }
    }
}

void BinaryDevice::turnOff(boolean shareInformation)
{
    if (m_state && m_operational && !m_locked)
    {
        digitalWrite(m_relayPin, LOW);

        m_state = false;

        if (shareInformation)
        {
            // Affichage de l'animation d'arrêt.
        }
    }
}

void BinaryDevice::toggle(boolean shareInformation)
{
    if (m_operational && !m_locked)
    {
        if (m_state)
            turnOff(shareInformation);

        else
            turnOn(shareInformation);
    }
}