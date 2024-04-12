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
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "gammaCorrection/gammaCorrection.hpp"
#include "RGBLEDStrip.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param RPin La broche liée à l'alimentation du rouge des rubans de DEL.
/// @param GPin La broche liée à l'alimentation du vert des rubans de DEL.
/// @param BPin La broche liée à l'alimentation du bleu des rubans de DEL.
RGBLEDStrip::RGBLEDStrip(String friendlyName, int ID, HomeAssistant &connection, Display &display, int RPin, int GPin, int BPin) : Output(friendlyName, ID, connection, display), m_RPin(RPin), m_GPin(GPin), m_BPin(BPin), m_RState(0), m_GState(0), m_BState(0), m_mode(nullptr) {}

/// @brief Initialise l'objet.
void RGBLEDStrip::setup()
{
    if (m_operational)
        return;

    Output::setup();

    pinMode(m_RPin, OUTPUT);
    pinMode(m_GPin, OUTPUT);
    pinMode(m_BPin, OUTPUT);

    m_operational = true;

    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Met en marche le ruban de DEL RVB.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void RGBLEDStrip::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    if (m_mode == nullptr)
        return;

    m_mode->activate();

    m_state = true;

    m_connection.updateOutputDeviceState(m_ID, true);

    if (shareInformation)
        m_display.displayDeviceState(true);
}

/// @brief Arrête le ruban de DEL RVB.
/// @param shareInformation Affiche ou non l'animation d'arrêt sur l'écran.
void RGBLEDStrip::turnOff(bool shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    m_mode->desactivate();

    setColor(0, 0, 0);

    m_state = false;

    m_connection.updateOutputDeviceState(m_ID, false);

    if (shareInformation)
        m_display.displayDeviceState(false);
}

/// @brief Méthode d'exécution des tâches liées au ruban de DEL RVB.
void RGBLEDStrip::loop()
{
    if (!m_operational || !m_state)
        return;

    m_mode->loop();
}

/// @brief Change le mode actuel du ruban de DEL RVB.
/// @param mode Le mode à sélectionner.
/// @param shareInformation Affichage ou non de l'animation sur l'écran.
void RGBLEDStrip::setMode(RGBLEDStripMode *mode, bool shareInformation)
{
    if (m_locked || mode == m_mode)
        return;

    if (m_mode != nullptr && m_operational && m_state)
        m_mode->desactivate();

    m_mode = mode;

    if (m_mode == nullptr)
        return;

    if (shareInformation)
        m_display.displayMessage(m_mode->getFriendlyName(), "Mode");

    if (m_operational && m_state)
    {
        m_mode->activate();

        m_connection.updateRGBLEDStripMode(m_ID, m_mode->getID(), m_RState, m_GState, m_BState);
    }
}

/// @brief Méthode renvoyant le mode actuel du ruban de DEL RVB.
/// @return Le mode actuel du ruban de DEL RVB.
RGBLEDStripMode *RGBLEDStrip::getMode() const
{
    return m_mode;
}

/// @brief Méthode renvoyant l'intensité actuelle du rouge du ruban de DEL RVB.
/// @return L'intensité actuelle du rouge du ruban de DEL RVB.
int RGBLEDStrip::getR() const
{
    return m_RState;
}

/// @brief Méthode renvoyant l'intensité actuelle du vert du ruban de DEL RVB.
/// @return L'intensité actuelle du vert du ruban de DEL RVB.
int RGBLEDStrip::getG() const
{
    return m_GState;
}

/// @brief Méthode renvoyant l'intensité actuelle du bleu du ruban de DEL RVB.
/// @return L'intensité actuelle du bleu du ruban de DEL RVB.
int RGBLEDStrip::getB() const
{
    return m_BState;
}

void RGBLEDStrip::setColor(int r, int g, int b)
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

    m_RState = r;
    m_GState = g;
    m_BState = b;

    analogWrite(m_RPin, gammaCorrection(r));
    analogWrite(m_GPin, gammaCorrection(g));
    analogWrite(m_BPin, gammaCorrection(b));
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param strip Le ruban de DEL utilisé pour l'animation.
RGBLEDStripMode::RGBLEDStripMode(String friendlyName, int ID, RGBLEDStrip &strip) : m_friendlyName(friendlyName), m_ID(ID), m_strip(strip), m_activated(false) {}

/// @brief Méthode permettant d'obtenir le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @return Le nom formaté pour être présenté à l'utilisateur du périphérique.
String RGBLEDStripMode::getFriendlyName() const
{
    return m_friendlyName;
}

/// @brief Méthode permettant de savoir si le mode est actuellement activé (en fonctionnement).
/// @return L'état du mode.
bool RGBLEDStripMode::isActivated() const
{
    return m_activated;
}

int RGBLEDStripMode::getID() const
{
    return m_ID;
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

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param strip Le ruban de DEL utilisé pour l'animation.
ColorMode::ColorMode(String friendlyName, int ID, RGBLEDStrip &strip, HomeAssistant &connection) : RGBLEDStripMode(friendlyName, ID, strip), m_connection(connection), m_R(0), m_G(0), m_B(0) {}

/// @brief Défini la couleur du ruban de DEL RVB.
/// @param r L'intensité du rouge.
/// @param g L'intensité du vert.
/// @param b L'intesité du bleu.
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
    {
        m_strip.setColor(m_R, m_G, m_B);

        m_connection.updateRGBLEDStripMode(m_strip.getID(), m_ID, m_R, m_G, m_B);
    }
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

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param strip Le ruban de DEL utilisé pour l'animation.
AlarmMode::AlarmMode(String friendlyName, int ID, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, ID, strip), m_lastTime(0) {}

void AlarmMode::desactivate()
{
    RGBLEDStripMode::desactivate();

    m_lastTime = 0;
}

void AlarmMode::loop()
{
    if ((millis() - m_lastTime) >= 500 && (millis() - m_lastTime) <= 600)
    {
        m_lastTime -= 100;
        m_strip.setColor(255, 0, 0);
    }

    else if ((millis() - m_lastTime) >= 1000)
    {
        m_lastTime = millis();
        m_strip.setColor(0, 0, 0);
    }
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param strip Le ruban de DEL utilisé pour l'animation.
/// @param speed La vitesse de l'animation.
RainbowMode::RainbowMode(String friendlyName, int ID, RGBLEDStrip &strip, int speed) : RGBLEDStripMode(friendlyName, ID, strip), m_lastTime(0), m_step(0), m_increment(1), m_delay(10), m_speed(speed) {}

/// @brief Définit la vitesse de l'animation arc-en-ciel.
/// @param speed La vitesse, de `0` (lent) à `100` (très rapide).
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

/// @brief Méthode permettant de connaître la vitesse actuelle du mode arc-en-ciel.
/// @return
int RainbowMode::getAnimationSpeed()
{
    return m_speed;
}

void RainbowMode::activate()
{
    RGBLEDStripMode::activate();

    m_strip.setColor(0, 0, 255);

    m_lastTime = millis();
}

void RainbowMode::desactivate()
{
    RGBLEDStripMode::desactivate();

    m_lastTime = 0;
    m_step = 0;
}

void RainbowMode::loop()
{

    unsigned long actualTime = millis();

    if ((actualTime - m_lastTime) < (unsigned long)m_delay)
        return;

    m_lastTime = actualTime;

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
SoundreactMode::SoundreactMode(String friendlyName, int ID, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, ID, strip) {}

void SoundreactMode::loop() {}
