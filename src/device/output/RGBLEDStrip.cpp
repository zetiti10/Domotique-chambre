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

RGBLEDStrip::RGBLEDStrip(String friendlyName, Display &display, int RPin, int GPin, int BPin) : Output(friendlyName, display), m_RPin(RPin), m_GPin(GPin), m_BPin(BPin), m_mode(nullptr) {}

void RGBLEDStrip::setup()
{
    Output::setup();

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
        sendLogMessage(ERROR, "Le ruban de DEL RVB '" + m_friendlyName + "' n'a pas pu être allumé car aucun mode n'est sélectionné.");
        return;
    }

    m_mode->activate();

    m_state = true;

    if (shareInformation)
        m_display.displayDeviceState(true);

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
        m_display.displayDeviceState(false);

    sendLogMessage(INFO, "Le ruban de DEL RVB '" + m_friendlyName + "' est désactivé.");
}

void RGBLEDStrip::loop()
{
    if (!m_operational || !m_state)
        return;

    m_mode->loop();
}

void RGBLEDStrip::setMode(RGBLEDStripMode &mode, boolean shareInformation)
{
    if (m_locked)
        return;

    if (m_mode != nullptr && m_operational && m_state)
        m_mode->desactivate();

    m_mode = &mode;

    sendLogMessage(INFO, "Le mode du ruban de DEL RVB '" + m_friendlyName + "' a été défini sur '" + m_mode->getFriendlyName() + "'.");
    m_display.displayMessage(m_mode->getFriendlyName(), "Mode");

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

        analogWrite(m_RPin, r);
        analogWrite(m_GPin, g);
        analogWrite(m_BPin, b);
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

RainbowMode::RainbowMode(String friendlyName, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, strip), m_counter(0), m_step(0), m_increment(1), m_delay(10), m_speed(0) {}

void RainbowMode::setAnimationSpeed(int speed)
{
    if (speed < 0)
        speed = 0;

    if (speed > 100)
        speed = 100;

    m_speed = speed;

    m_increment = map(speed, 0, 100, 1, 10);
    m_delay = map(speed, 0, 100, 100, 5);
}

int RainbowMode::getAnimationSpeed()
{
    return m_speed;
}

void RainbowMode::activate()
{
    RGBLEDStripMode::activate();

    m_strip.setColor(0, 0, 255);

    m_counter = millis();
}

void RainbowMode::desactivate()
{
    RGBLEDStripMode::desactivate();

    m_counter = 0;
    m_step = 0;
}

void RainbowMode::loop()
{

    unsigned long actualTime = millis();

    if ((actualTime - m_counter) < m_delay)
        return;

    m_counter = actualTime;

    if (m_step == 0)
    {
        m_strip.setColor(m_strip.getR() + m_increment, m_strip.getG(), m_strip.getB() - m_increment);

        if (m_strip.getR() >= 255 - m_increment)
            m_step = 1;
    }

    else if (m_step == 1)
    {
        m_strip.setColor(m_strip.getR() - m_increment, m_strip.getG() + m_increment, m_strip.getB());

        if (m_strip.getG() >= 255 - m_increment)
            m_step = 2;
    }

    else if (m_step == 2)
    {
        m_strip.setColor(m_strip.getR(), m_strip.getG() - m_increment, m_strip.getB() + m_increment);

        if (m_strip.getB() >= 255 - m_increment)
            m_step = 0;
    }
}