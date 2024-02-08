/**
 * @file device/output/RGBLEDStrip.cpp
 * @author Louis L
 * @brief Objet représentant un ruban de DEL RVB.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "RGBLEDStrip.hpp"
#include "../../logger.hpp"

RGBLEDStrip::RGBLEDStrip(String friendlyName, int RPin, int GPin, int BPin) : Output(friendlyName), m_RPin(RPin), m_GPin(GPin), m_BPin(BPin), m_mode(nullptr) {}

void RGBLEDStrip::setup()
{
    pinMode(m_RPin, OUTPUT);
    pinMode(m_GPin, OUTPUT);
    pinMode(m_BPin, OUTPUT);

    m_operational = true;

    sendLogMessage(INFO, "Le ruban de DEL RVB '" + m_friendlyName + "' est initialisé aux broches " + m_RPin + ", " + m_GPin + " et " + m_BPin + ".");
}

void RGBLEDStrip::turnOn(boolean shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    if (m_mode == nullptr)
    {
        sendLogMessage(ERROR, "Le ruban de DEL RVB '" + m_friendlyName + "' n'a pas pu être allumé car aucun mode n'est sélctionné.");
        return;
    }

    m_mode->activate();

    m_state = true;

    if (shareInformation)
    {
        // Affichage de l'animation d'allumage.
    }

    sendLogMessage(INFO, "Le ruban de DEL RVB '" + m_friendlyName + "' est allumé avec le mode '" + m_mode->getFriendlyName() + "'.");
}

void RGBLEDStrip::turnOff(boolean shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    m_mode->desactivate();

    setColor(0, 0, 0);

    m_state = false;

    if (shareInformation)
    {
        // Affichage de l'animation d'allumage.
    }

    sendLogMessage(INFO, "Le ruban de DEL RVB '" + m_friendlyName + "' est désactivé.");
}

void RGBLEDStrip::loop()
{
    if (!m_operational || !m_state)
        return;

    m_mode->loop();
}

void RGBLEDStrip::setMode(RGBLEDStripMode &mode)
{
    if (m_locked)
        return;

    if (m_mode != nullptr && m_operational && m_state)
        m_mode->desactivate();

    m_mode = &mode;

    if (m_operational && m_state)
        m_mode->activate();
}

RGBLEDStripMode &RGBLEDStrip::getMode() const
{
    return *m_mode;
}

int RGBLEDStrip::getR() const
{
    return m_RState;
}

int RGBLEDStrip::getG() const
{
    return m_GState;
}

int RGBLEDStrip::getB() const
{
    return m_BState;
}

void RGBLEDStrip::setColor(int r, int g, int b)
{
    if (m_operational && m_state)
    {
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

        m_RState = r;
        m_GState = g;
        m_BState = b;

        digitalWrite(m_RPin, r);
        digitalWrite(m_GPin, g);
        digitalWrite(m_BPin, b);
    }
}

RGBLEDStripMode::RGBLEDStripMode(String friendlyName, RGBLEDStrip &strip) : m_friendlyName(friendlyName), m_strip(strip) {}

String RGBLEDStripMode::getFriendlyName() const
{
    return m_friendlyName;
}

boolean RGBLEDStripMode::isActivated() const
{
    return m_activated;
}

void RGBLEDStripMode::activate()
{
    m_activated = true;
}

void RGBLEDStripMode::desactivate()
{
    m_activated = false;

    m_strip.setColor(0, 0, 0);
}

ColorMode::ColorMode(String friendlyName, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, strip), m_R(0), m_G(0), m_B(0) {}

void ColorMode::setColor(int r, int g, int b)
{
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

    m_R = r;
    m_G = g;
    m_B = b;

    if (m_activated)
        m_strip.setColor(m_R, m_G, m_B);
}

void ColorMode::activate()
{
    RGBLEDStripMode::activate();

    m_strip.setColor(m_R, m_G, m_B);
}

void ColorMode::desactivate()
{
    m_activated = false;

    m_strip.setColor(0, 0, 0);
}

void ColorMode::loop() {}

AlarmMode::AlarmMode(String friendlyName, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, strip), m_counter(0) {}

void AlarmMode::desactivate()
{
    RGBLEDStripMode::desactivate();

    m_counter = 0;
}

void AlarmMode::loop()
{
    if ((millis() - m_counter) >= 200 && (millis() - m_counter) <= 300)
    {
        m_counter += 100;
        m_strip.setColor(255, 0, 0);
    }

    else if ((millis() - m_counter) >= 400)
    {
        m_counter = millis();
        m_strip.setColor(0, 0, 0);
    }
}