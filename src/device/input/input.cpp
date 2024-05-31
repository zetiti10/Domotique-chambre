/**
 * @file device/input/input.cpp
 * @author Louis L
 * @brief Classe mère commune à tous les capteurs du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "input.hpp"
#include "device/device.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
Input::Input(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection) : Device(friendlyName, ID), m_connection(connection) {}

/// @brief Initialise l'objet.
void Input::setup()
{
    m_connection.setup();
}