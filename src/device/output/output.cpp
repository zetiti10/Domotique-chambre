/**
 * @file device/output/output.cpp
 * @author Louis L
 * @brief Cette classe représente tous les appareils de type sortie (lumières...) du système de domotique.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"
#include "device/device.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param display L'écran à utiliser pour afficher des informations / animations.
Output::Output(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display) : Device(friendlyName, ID), m_display(display), m_connection(connection), m_state(false), m_locked(false) {}

/// @brief Initialise l'objet.
void Output::setup()
{
    m_display.setup();
    m_connection.setup();
}

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void Output::reportState()
{
    if (!m_operational)
        return;

    m_connection.updateOutputDeviceState(m_ID, m_state);
}

/// @brief Bascule l'état de l'objet.
/// @param shareInformation Affiche ou non à l'écran l'animation de changement d'état.
void Output::toggle(bool shareInformation)
{
    if (!m_operational || m_locked)
        return;

    if (m_state)
        this->turnOff(shareInformation);

    else
        this->turnOn(shareInformation);
}

/// @brief Méthode permettant de récupérer l'état actuel du périphrique.
/// @return L'état actuel du périphérique.
bool Output::getState() const
{
    return m_state;
}

/// @brief Bloque le périphérique.
void Output::lock()
{
    m_locked = true;
    m_connection.updateDeviceAvailability(m_ID, false);
}

/// @brief Débloque le périphérique.
void Output::unLock()
{
    m_locked = false;
    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Méthode permettant de connaître l'état de blocage du périphérique.
/// @return L'état de blocage du périphérique.
bool Output::isLocked() const
{
    return m_locked;
}

/// @brief Méthode arrêtant le périphérique avant l'arrêt du système.
void Output::shutdown()
{
    this->turnOff();
    Device::shutdown();
}
