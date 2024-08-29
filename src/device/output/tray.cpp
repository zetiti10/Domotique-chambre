/**
 * @file device/output/tray.cpp
 * @author Louis L
 * @brief Classe représentant la motorisation du plateau du bureau.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "tray.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param motorPin1 La broche 1 liée au contrôleur du moteur.
/// @param motorPin2 La broche 2 liée au contrôleur du moteur.
/// @param speedPin La broche liée au contrôleur du moteur, gérant sa vitesse.
Tray::Tray(const __FlashStringHelper* friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int motorPin1, unsigned int motorPin2, unsigned int speedPin) : Output(friendlyName, ID, connection, display), m_motorPin1(motorPin1), m_motorPin2(motorPin2), m_speedPin(speedPin) {}

/// @brief Initialise l'objet.
void Tray::setup()
{
    if (m_operational)
        return;

    pinMode(m_motorPin1, OUTPUT);
    pinMode(m_motorPin2, OUTPUT);

    m_operational = true;
    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Ouvre le plateau
/// @param shareInformation Affiche ou non l'animation d'ouverture sur l'écran.
void Tray::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    m_connection.updateOutputDeviceState(m_ID, true);

    analogWrite(m_speedPin, 120);
    digitalWrite(m_motorPin1, LOW);
    digitalWrite(m_motorPin2, HIGH);

    m_display.displayTray(true, shareInformation);

    digitalWrite(m_motorPin1, HIGH);
    digitalWrite(m_motorPin2, HIGH);
    analogWrite(m_speedPin, 120);

    m_state = true;
}

/// @brief Ferme le plateau
/// @param shareInformation Affiche ou non l'animation de fermeture sur l'écran.
void Tray::turnOff(bool shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    m_connection.updateOutputDeviceState(m_ID, false);

    analogWrite(m_speedPin, 140);
    digitalWrite(m_motorPin1, HIGH);
    digitalWrite(m_motorPin2, LOW);

    m_display.displayTray(false, shareInformation);

    digitalWrite(m_motorPin1, HIGH);
    digitalWrite(m_motorPin2, HIGH);
    analogWrite(m_speedPin, 140);

    m_state = false;
}