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
#include "device.hpp"

Device::Device() : m_state(false), m_operational(false), m_locked(false) {}

Device::~Device() {}

void Device::setOperational()
{
    m_operational = true;
}

void Device::setUnavailable()
{
    m_operational = false;
}

boolean Device::getAvailability()
{
    return m_operational;
}

void Device::lock()
{
    m_locked = true;
}

void Device::unLock()
{
    m_locked = false;
}