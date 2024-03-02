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
#include "output.hpp"
#include "../interface/display.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param motorPin1 La broche 1 liée au contrôleur du moteur.
/// @param motorPin2 La broche 2 liée au contrôleur du moteur.
/// @param speedPin La broche liée au contrôleur du moteur, gérant sa vitesse.
Tray::Tray(String friendlyName, int ID, Display &display, HomeAssistant &connection, int motorPin1, int motorPin2, int speedPin) : Output(friendlyName, ID, display, connection), m_motorPin1(motorPin1), m_motorPin2(motorPin2), m_speedPin(speedPin) {}

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

    m_display.displayTray(shareInformation, true);

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

    analogWrite(m_speedPin, 120);
    digitalWrite(m_motorPin1, HIGH);
    digitalWrite(m_motorPin2, LOW);

    m_display.displayTray(shareInformation, false);

    digitalWrite(m_motorPin1, HIGH);
    digitalWrite(m_motorPin2, HIGH);
    analogWrite(m_speedPin, 120);

    m_state = false;
}