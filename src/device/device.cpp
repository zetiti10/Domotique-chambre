/**
 * @file device/device.cpp
 * @author Louis L
 * @brief Classe mère commune à tous les éléments du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device.hpp"

Device::Device(String friendlyName) : m_friendlyName(friendlyName), m_operational(false) {}

String Device::getFriendlyName() const
{
    return m_friendlyName;
}

bool Device::getAvailability() const
{
    return m_operational;
}