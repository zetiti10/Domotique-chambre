/**
 * @file device/output/binaryOutput.cpp
 * @author Louis L
 * @brief Objet simple incluant en plus de l'héritage de Device, une broche de l'Arduino pour la contrôler.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "binaryOutput.hpp"
#include "../../logger.hpp"

BinaryOutput::BinaryOutput(String friendlyName, int relayPin) : Output(friendlyName), m_relayPin(relayPin) {}

void BinaryOutput::setup()
{
    pinMode(m_relayPin, OUTPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le périphérique '" + m_friendlyName + "' est initialisé à la broche " + m_relayPin + ".");
}

void BinaryOutput::turnOn(boolean shareInformation)
{
    if (m_operational && !m_locked && !m_state)
    {
        digitalWrite(m_relayPin, HIGH);

        m_state = true;

        if (shareInformation)
        {
            // Affichage de l'animation d'allumage.
        }

        sendLogMessage(INFO, "Le périphérique '" + m_friendlyName + "' est allumé.");
    }
}

void BinaryOutput::turnOff(boolean shareInformation)
{
    if (m_operational && !m_locked && m_state)
    {
        digitalWrite(m_relayPin, LOW);

        m_state = false;

        if (shareInformation)
        {
            // Affichage de l'animation d'arrêt.
        }

        sendLogMessage(INFO, "Le périphérique '" + m_friendlyName + "' est éteint.");
    }
}