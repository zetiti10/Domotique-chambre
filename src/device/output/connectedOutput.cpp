/**
 * @file device/output/connectedLight.cpp
 * @author Louis L
 * @brief Objet gérant une lampe connectée connectée en réseau.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "connectedOutput.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Objet représentant un périphériques contrôlé depuis le réseau.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
ConnectedOutput::ConnectedOutput(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display) : Output(friendlyName, ID, connection, display) {}

/// @brief Initialise l'objet.
void ConnectedOutput::setup()
{
    if (m_operational)
        return;

    Output::setup();

    m_connection.setup();

    m_operational = true;
}

/// @brief Met à jour l'état du périphérique virtuel. Cette méthode doit être exécutée deux fois, afin de basculer deux fois l'état du périphérique.
void ConnectedOutput::reportState()
{
    if (!m_operational)
        return;

    // Changement de l'état de l'appareil pour reçevoir une mise à jour de son état.
    this->toggle();
}

/// @brief Met en marche le périphérique distant.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void ConnectedOutput::turnOn(bool shareInformation)
{
    if (!m_operational || m_state)
        return;

    m_connection.turnOnConnectedDevice(m_ID);
}

/// @brief Arrête le périphérique distant.
/// @param shareInformation Affiche ou non l'animation d'arrêt sur l'écran.
void ConnectedOutput::turnOff(bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    m_connection.turnOffConnectedDevice(m_ID);
}

/// @brief Méthode permettant de mettre à jour l'état du périphérique.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void ConnectedOutput::updateOn(bool shareInformation)
{
    if (!m_operational || m_state)
        return;

    m_state = true;
    if (shareInformation)
        m_display.displayDeviceState(true);
}

/// @brief Méthode permettant de mettre à jour l'état du périphérique.
/// @param shareInformation Affiche ou non l'animation d'arrêt sur l'écran.
void ConnectedOutput::updateOff(bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    m_state = false;
    if (shareInformation)
        m_display.displayDeviceState(false);
}

/// @brief Définis le périphérique comme opérationnel.
void ConnectedOutput::setAvailable()
{
    m_operational = true;
}

/// @brief Définis le périphérique comme indisponible.
void ConnectedOutput::setUnavailable()
{
    m_operational = false;
}

/// @brief Objet représentant un périphériques contrôlé depuis le réseau.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param minimalColorTemperature La température de couleur minimale que la lampe accepte.
/// @param maximalColorTemperature La température de couleur maximale que la lampe accepte.
ConnectedTemperatureVariableLight::ConnectedTemperatureVariableLight(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int minimalColorTemperature, unsigned int maximalColorTemperature) : ConnectedOutput(friendlyName, ID, connection, display), m_minimalColorTemperature(minimalColorTemperature), m_maximalColorTemperature(maximalColorTemperature), m_colorTemperature(m_minimalColorTemperature), m_luminosity(255) {}

/// @brief Méthode permettant de définir la température de couleur de la lampe.
/// @param temperature La température de couleur à définir.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedTemperatureVariableLight::setColorTemperature(unsigned int temperature, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (temperature < m_minimalColorTemperature)
        temperature = m_minimalColorTemperature;

    if (temperature > m_maximalColorTemperature)
        temperature = m_maximalColorTemperature;

    m_connection.setConnectedTemperatureVariableLightTemperature(m_ID, temperature);
}

/// @brief Méthode permettant de définir la luminosité de la lampe
/// @param luminosity La luminosité à définir.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedTemperatureVariableLight::setLuminosity(unsigned int luminosity, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (luminosity > 255)
        luminosity = 255;

    m_connection.setConnectedTemperatureVariableLightLuminosity(m_ID, luminosity);
}

/// @brief Méthode retournant la température de couleur actuelle de la lampe.
/// @return La température de couleur actuelle de la lampe.
unsigned int ConnectedTemperatureVariableLight::getColorTemperature() const
{
    return m_colorTemperature;
}

/// @brief Méthode retournant la luminosité actuelle de la lampe.
/// @return La luminosité actuelle de la lampe.
unsigned int ConnectedTemperatureVariableLight::getLuminosity() const
{
    return m_luminosity;
}

/// @brief Méthode permettant de mettre à jour la température de couleur de la lampe.
/// @param temperature La température de couleur.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedTemperatureVariableLight::updateColorTemperature(unsigned int temperature, bool shareInformation)
{
    if (!m_operational || !m_state || m_colorTemperature == temperature)
        return;

    m_colorTemperature = temperature;
    if (shareInformation)
        m_display.displayLightColorTemperature(m_minimalColorTemperature, m_maximalColorTemperature, m_colorTemperature);
}

/// @brief Méthode permettant de mettre à jour la luminosité de la lampe
/// @param luminosity La luminosité à mettre à jour.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedTemperatureVariableLight::updateLuminosity(unsigned int luminosity, bool shareInformation)
{
    if (!m_operational || !m_state || m_luminosity == luminosity)
        return;

    m_luminosity = luminosity;
    if (shareInformation)
        m_display.displayLuminosity(m_luminosity);
}

/// @brief Objet représentant un périphériques contrôlé depuis le réseau.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param minimalColorTemperature La température de couleur minimale que la lampe accepte.
/// @param maximalColorTemperature La température de couleur maximale que la lampe accepte.
ConnectedColorVariableLight::ConnectedColorVariableLight(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int minimalColorTemperature, unsigned int maximalColorTemperature) : ConnectedTemperatureVariableLight(friendlyName, ID, connection, display, minimalColorTemperature, maximalColorTemperature), m_RColor(0), m_GColor(0), m_BColor(0) {}

/// @brief Méthode définissant la couleur de la lampe.
/// @param r L'intensité de la composante rouge, de `0`, à `255`.
/// @param g L'intensité de la composante vert, de `0`, à `255`.
/// @param b L'intensité de la composante bleu, de `0`, à `255`.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedColorVariableLight::setColor(unsigned int r, unsigned int g, unsigned int b, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (r > 255)
        r = 255;

    if (g > 255)
        g = 255;

    if (b > 255)
        b = 255;

    m_connection.setConnectedColorVariableLightColor(m_ID, r, g, b);
}

/// @brief Méthode permettant de définir la température de couleur de la lampe.
/// @param temperature La température de couleur à définir.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedColorVariableLight::setColorTemperature(unsigned int temperature, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (temperature < m_minimalColorTemperature)
        temperature = m_minimalColorTemperature;

    if (temperature > m_maximalColorTemperature)
        temperature = m_maximalColorTemperature;

    m_connection.setConnectedColorVariableLightTemperature(m_ID, temperature);
}

/// @brief Méthode permettant de définir la luminosité de la lampe
/// @param luminosity La luminosité à définir.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedColorVariableLight::setLuminosity(unsigned int luminosity, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (luminosity > 255)
        luminosity = 255;

    m_connection.setConnectedColorVariableLightLuminosity(m_ID, luminosity);
}

/// @brief Permet d'obtenir l'intensité actuelle de la composante rouge de la lampe.
/// @return L'intensité actuelle de la composante rouge de la lampe.
unsigned int ConnectedColorVariableLight::getRLuminosity() const
{
    return m_RColor;
}

/// @brief Permet d'obtenir l'intensité actuelle de la composante vert de la lampe.
/// @return L'intensité actuelle de la composante vert de la lampe.
unsigned int ConnectedColorVariableLight::getGLuminosity() const
{
    return m_GColor;
}

/// @brief Permet d'obtenir l'intensité actuelle de la composante bleu de la lampe.
/// @return L'intensité actuelle de la composante bleu de la lampe.
unsigned int ConnectedColorVariableLight::getBLuminosity() const
{
    return m_BColor;
}

/// @brief Méthode mettant à jour la couleur de la lampe.
/// @param r L'intensité de la composante rouge, de `0`, à `255`.
/// @param g L'intensité de la composante vert, de `0`, à `255`.
/// @param b L'intensité de la composante bleu, de `0`, à `255`.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void ConnectedColorVariableLight::updateColor(unsigned int r, unsigned int g, unsigned int b, bool shareInformation)
{
    if (!m_operational || !m_state || (m_RColor == r && m_GColor == g && m_BColor == b))
        return;

    m_RColor = r;
    m_GColor = g;
    m_BColor = b;
    if (shareInformation)
        m_display.displayLEDState(m_RColor, m_GColor, m_BColor);
}
