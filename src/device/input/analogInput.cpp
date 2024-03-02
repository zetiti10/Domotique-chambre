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
#include "input.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
AnalogInput::AnalogInput(String friendlyName, int ID, HomeAssistant connection, int pin) : Input(friendlyName, ID, connection), m_value(0), m_pin(pin) {}

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

/// @brief Boucle d'exécution des tâches liées au capteur.
void AnalogInput::loop() {}

/// @brief Méthode permettant de récupérer la valeur actuelle du capteur.
/// @return La valeur actuelle du capteur.
int AnalogInput::getValue()
{
    if (m_operational)
        m_value = analogRead(m_pin);

    return m_value;
}