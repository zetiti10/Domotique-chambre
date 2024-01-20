/**
 * @file device/output/device.cpp
 * @author Louis L
 * @brief Cette classe représente tous les appareils de type sortie (lumières...) du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

Output::Output(String friendlyName) : Device(friendlyName) {}

void Output::setOperational()
{
    m_operational = true;
}

void Output::setUnavailable()
{
    m_operational = false;
}

boolean Output::getAvailability() const
{
    return m_operational;
}

void Output::lock()
{
    m_locked = true;
}

void Output::unLock()
{
    m_locked = false;
}