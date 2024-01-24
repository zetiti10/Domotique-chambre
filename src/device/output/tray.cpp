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

Tray::Tray(String friendlyName, int motorPin1, int motorPin2) : Output(friendlyName), m_motorPin1(motorPin1), m_motorPin2(motorPin2) {}

void Tray::setup()
{
    pinMode(m_motorPin1, OUTPUT);
    pinMode(m_motorPin2, OUTPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le plateau '" + m_friendlyName + "' est initialisé aux broches " + m_motorPin1 + " et " + m_motorPin2 + ".");
}

void Tray::turnOn(boolean shareInformation)
{
    if (m_operational && !m_locked && !m_state)
    {
        // Ouverture.

        m_state = true;

        if (shareInformation)
        {
            // Affichage de l'animation d'allumage.
        }

        sendLogMessage(INFO, "Le plateau '" + m_friendlyName + "' est ouvert.");
    }
}

void Tray::turnOff(boolean shareInformation)
{
    if (m_operational && !m_locked && m_state)
    {
        // Fermeture.

        m_state = false;

        if (shareInformation)
        {
            // Affichage de l'animation d'allumage.
        }

        sendLogMessage(INFO, "Le plateau '" + m_friendlyName + "' est fermé.");
    }
}