/**
 * @file device/input/analogInput.cpp
 * @author Louis L
 * @brief Classe représentant un capteur dont l'état est anlogique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "analogInput.hpp"
#include "device/input/input.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param pin La broche liée au capteur.
/// @param connected Permet d'envoyer l'état du capteur à Home Assistant ou non.
AnalogInput::AnalogInput(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool connected) : Input(friendlyName, ID, connection), m_value(0), m_pin(pin), m_connected(connected), m_lastTime(0) {}

/// @brief Initialise l'objet.
void AnalogInput::setup()
{
    if (m_operational)
        return;

    Input::setup();
    pinMode(m_pin, INPUT);
    m_operational = true;
    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void AnalogInput::reportState()
{
    if (!m_operational)
        return;

    m_connection.updateAnalogInput(m_ID, getValue());
}

/// @brief Boucle d'exécution des tâches liées au capteur.
void AnalogInput::loop()
{
    if (m_connected && ((millis() - m_lastTime) >= 10000))
    {
        m_connection.updateAnalogInput(m_ID, getValue());
        m_lastTime = millis();
    }
}

/// @brief Méthode permettant de récupérer la valeur actuelle du capteur.
/// @return La valeur actuelle du capteur.
unsigned int AnalogInput::getValue()
{
    if (m_operational)
        m_value = analogRead(m_pin);

    return m_value;
}