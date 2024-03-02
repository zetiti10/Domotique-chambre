/**
 * @file device/device.cpp
 * @author Louis L
 * @brief Classe mère commune à tous les éléments du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
Device::Device(String friendlyName, int ID) : m_friendlyName(friendlyName), m_ID(ID), m_operational(false) {}

/// @brief Méthode permettant d'obtenir le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @return Le nom formaté pour être présenté à l'utilisateur du périphérique.
String Device::getFriendlyName() const
{
    return m_friendlyName;
}

int Device::getID() const
{
    return m_ID;
}

/// @brief Méthode permettant de savoir si le périphérique est opérationnel.
/// @return Un booléen indiquant si le périphérique est opérationnel.
bool Device::getAvailability() const
{
    return m_operational;
}