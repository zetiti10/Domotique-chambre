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
#include "device/output/binaryOutput.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param relayPin La broche de l'Arduino liée au relai qui contrôle le périphérique.
BinaryOutput::BinaryOutput(String friendlyName, int ID, HomeAssistant &connection, Display &display, int relayPin) : Output(friendlyName, ID, connection, display), m_relayPin(relayPin) {}

/// @brief Initialise l'objet.
void BinaryOutput::setup()
{
    if (m_operational)
        return;

    Output::setup();

    pinMode(m_relayPin, OUTPUT);

    m_operational = true;

    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Met en marche le périphérique.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void BinaryOutput::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    digitalWrite(m_relayPin, HIGH);

    m_state = true;

    m_connection.updateOutputDeviceState(m_ID, true);

    if (shareInformation)
        m_display.displayDeviceState(true);
}

/// @brief Arrête le périphérique.
/// @param shareInformation Affiche ou non l'animation d'arrêt sur l'écran.
void BinaryOutput::turnOff(bool shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    digitalWrite(m_relayPin, LOW);

    m_state = false;

    m_connection.updateOutputDeviceState(m_ID, false);

    if (shareInformation)
        m_display.displayDeviceState(false);
}