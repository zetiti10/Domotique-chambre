/**
 * @file device/output/output.cpp
 * @author Louis L
 * @brief Cette classe représente tous les appareils de type sortie (lumières...) du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"
#include "../../logger.hpp"

Output::Output(String friendlyName) : Device(friendlyName), m_state(false), m_locked(false) {}

void Output::toggle(boolean shareInformation)
{
    if (m_operational && !m_locked)
    {
        if (m_state)
            turnOff(shareInformation);

        else
            turnOn(shareInformation);
    }
}

boolean Output::getState() const
{
    return m_state;
}

void Output::lock()
{
    m_locked = true;

    sendLogMessage(INFO, "Le périphérique '" + m_friendlyName + "' a été bloqué.");
}

void Output::unLock()
{
    m_locked = false;

    sendLogMessage(INFO, "Le périphérique '" + m_friendlyName + "' a été débloqué.");
}

boolean Output::isLocked()
{
    return m_locked;
}