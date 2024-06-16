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
/// @param friendlyName Le nom du périphérique formaté pour être présenté à l'utilisateur.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
Device::Device(const __FlashStringHelper *friendlyName, unsigned int ID) : m_friendlyName(friendlyName), m_ID(ID), m_operational(false) {}

/// @brief Méthode permettant d'obtenir le nom du périphérique formaté pour être présenté à l'utilisateur.
/// @return Le nom du périphérique formaté pour être présenté à l'utilisateur.
const __FlashStringHelper *Device::getFriendlyName() const
{
    return m_friendlyName;
}

/// @brief Méthode permettant d'obtenir l'identifiant unique du périphérique.
/// @return L'identifiant unique du périphérique.
const unsigned int Device::getID() const
{
    return m_ID;
}

/// @brief Méthode permettant de savoir si le périphérique est opérationnel.
/// @return Un booléen indiquant si le périphérique est opérationnel.
bool Device::getAvailability() const
{
    return m_operational;
}

/// @brief Méthode d'exécution des tâches liées au périphérique.
void Device::loop()
{
}

/// @brief Méthode arrêtant le périphérique avant l'arrêt du système.
void Device::shutdown()
{
    m_operational = false;
}
