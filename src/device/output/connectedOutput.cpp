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
#include "device/output/connectedOutput.hpp"
#include "device/interface/HomeAssistant.hpp"

ConnectedOutput::ConnectedOutput(const __FlashStringHelper* friendlyName, int ID, HomeAssistant &connection, Display &display) : Output(friendlyName, ID, connection, display) {}

void ConnectedOutput::setup()
{
    if (m_operational)
        return;

    Output::setup();

    m_connection.setup();

    m_operational = true; // Provisoire
}

/// @brief Met à jour l'état du périphérique virtuel.
void ConnectedOutput::reportState()
{
    if (!m_operational)
        return;

    // Changement de l'état de l'appareil pour reçevoir une mise à jour de son état.
    toggle();
    toggle();
}

void ConnectedOutput::turnOn(bool shareInformation)
{
    if (!m_operational || m_state)
        return;

    m_connection.turnOnConnectedDevice(m_ID);
}

void ConnectedOutput::turnOff(bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    m_connection.turnOffConnectedDevice(m_ID);
}

void ConnectedOutput::updateOn(bool shareInformation)
{
    if (!m_operational || m_state)
        return;

    m_state = true;

    if (shareInformation)
        m_display.displayDeviceState(true);
}

void ConnectedOutput::updateOff(bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    m_state = false;

    if (shareInformation)
        m_display.displayDeviceState(false);
}

void ConnectedOutput::setAvailable()
{
    m_operational = true;
}

void ConnectedOutput::setUnavailable()
{
    m_operational = false;
}

ConnectedTemperatureVariableLight::ConnectedTemperatureVariableLight(const __FlashStringHelper* friendlyName, int ID, HomeAssistant &connection, Display &display, int minimalColorTemperature, int maximalColorTemperature) : ConnectedOutput(friendlyName, ID, connection, display), m_minimalColorTemperature(minimalColorTemperature), m_maximalColorTemperature(maximalColorTemperature), m_colorTemperature(m_minimalColorTemperature), m_luminosity(255) {}

void ConnectedTemperatureVariableLight::setColorTemperature(int temperature, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (temperature < m_minimalColorTemperature)
        temperature = m_minimalColorTemperature;

    if (temperature > m_maximalColorTemperature)
        temperature = m_maximalColorTemperature;

    m_connection.setConnectedTemperatureVariableLightTemperature(m_ID, temperature);
}

void ConnectedTemperatureVariableLight::setLuminosity(int luminosity, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (luminosity < 0)
        luminosity = 0;

    if (luminosity > 255)
        luminosity = 255;

    m_connection.setConnectedTemperatureVariableLightLuminosity(m_ID, luminosity);
}

int ConnectedTemperatureVariableLight::getColorTemperature()
{
    return m_colorTemperature;
}

int ConnectedTemperatureVariableLight::getLuminosity()
{
    return m_luminosity;
}

void ConnectedTemperatureVariableLight::updateColorTemperature(int temperature, bool shareInformation)
{
    if (!m_operational || !m_state || m_colorTemperature == temperature)
        return;

    m_colorTemperature = temperature;

    if (shareInformation)
        m_display.displayLightColorTemperature(m_minimalColorTemperature, m_maximalColorTemperature, m_colorTemperature);
}

void ConnectedTemperatureVariableLight::updateLuminosity(int luminosity, bool shareInformation)
{
    if (!m_operational || !m_state || m_luminosity == luminosity)
        return;

    m_luminosity = luminosity;

    if (shareInformation)
        m_display.displayLuminosity(m_luminosity);
}

ConnectedColorVariableLight::ConnectedColorVariableLight(const __FlashStringHelper* friendlyName, int ID, HomeAssistant &connection, Display &display, int minimalColorTemperature, int maximalColorTemperature) : ConnectedTemperatureVariableLight(friendlyName, ID, connection, display, minimalColorTemperature, maximalColorTemperature), m_RColor(0), m_GColor(0), m_BColor(0) {}

void ConnectedColorVariableLight::setColor(int r, int g, int b, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (r < 0)
        r = 0;

    if (r > 255)
        r = 255;

    if (g < 0)
        g = 0;

    if (g > 255)
        g = 255;

    if (b < 0)
        b = 0;

    if (b > 255)
        b = 255;

    m_connection.setConnectedColorVariableLightColor(m_ID, r, g, b);
}

void ConnectedColorVariableLight::setColorTemperature(int temperature, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (temperature < m_minimalColorTemperature)
        temperature = m_minimalColorTemperature;

    if (temperature > m_maximalColorTemperature)
        temperature = m_maximalColorTemperature;

    m_connection.setConnectedColorVariableLightLuminosity(m_ID, temperature);
}

void ConnectedColorVariableLight::setLuminosity(int luminosity, bool shareInformation)
{
    if (!m_operational || !m_state)
        return;

    if (luminosity < 0)
        luminosity = 0;

    if (luminosity > 255)
        luminosity = 255;

    m_connection.setConnectedColorVariableLightLuminosity(m_ID, luminosity);
}

int ConnectedColorVariableLight::getRLuminosity()
{
    return m_RColor;
}

int ConnectedColorVariableLight::getGLuminosity()
{
    return m_GColor;
}

int ConnectedColorVariableLight::getBLuminosity()
{
    return m_BColor;
}

void ConnectedColorVariableLight::updateColor(int r, int g, int b, bool shareInformation)
{
    if (!m_operational || !m_state || (m_RColor == r && m_GColor == g && m_BColor == b))
        return;

    m_RColor = r;
    m_GColor = g;
    m_BColor = b;

    if (shareInformation)
        m_display.displayLEDState(m_RColor, m_GColor, m_BColor);
}
