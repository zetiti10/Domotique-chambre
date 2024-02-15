/**
 * @file device/output/output.cpp
 * @author Louis L
 * @brief Cette classe représente tous les appareils de type sortie (lumières...) du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "output.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param display L'écran à utiliser pour afficher des informations / animations.
Output::Output(String friendlyName, Display &display) : Device(friendlyName), m_display(display), m_state(false), m_locked(false) {}

/// @brief Initialise l'objet.
void Output::setup()
{
    m_display.setup();
}

/// @brief Bascule l'état de l'objet.
/// @param shareInformation Affiche ou non à l'écran l'animation de changement d'état.
void Output::toggle(bool shareInformation)
{
    if (m_operational && !m_locked)
    {
        if (m_state)
            turnOff(shareInformation);

        else
            turnOn(shareInformation);
    }
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
}

/// @brief Débloque le périphérique.
void Output::unLock()
{
    m_locked = false;
}

/// @brief Méthode permettant de connaître l'état de blocage du périphérique.
/// @return L'état de blocage du périphérique.
bool Output::isLocked() const
{
    return m_locked;
}