/**
 * @file device/output/RGBLEDStrip.cpp
 * @author Louis L
 * @brief Objet représentant un ruban de DEL RVB.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>

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
RGBLEDStrip::RGBLEDStrip(const __FlashStringHelper *friendlyName, int ID, HomeAssistant &connection, Display &display, int RPin, int GPin, int BPin) : Output(friendlyName, ID, connection, display), m_RPin(RPin), m_GPin(GPin), m_BPin(BPin), m_RState(0), m_GState(0), m_BState(0), m_mode(nullptr) {}

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

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void RGBLEDStrip::reportState()
{
    if (!m_operational)
        return;

    if (m_mode != nullptr)
        m_connection.updateRGBLEDStripMode(m_ID, m_mode->getID(), m_RState, m_GState, m_BState);

    else
        m_connection.updateRGBLEDStripMode(m_ID, 0, m_RState, m_GState, m_BState);

    Output::reportState();
}

/// @brief Met en marche le ruban de DEL RVB.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void RGBLEDStrip::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    if (m_mode == nullptr)
        return;

    m_state = true;

    m_mode->activate();

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
RGBLEDStripMode::RGBLEDStripMode(const __FlashStringHelper *friendlyName, int ID, RGBLEDStrip &strip) : m_friendlyName(friendlyName), m_ID(ID), m_strip(strip), m_activated(false) {}

/// @brief Méthode permettant d'obtenir le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @return Le nom formaté pour être présenté à l'utilisateur du périphérique.
const __FlashStringHelper* RGBLEDStripMode::getFriendlyName() const
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

/// @brief Méthode permettant d'arrêter proprement un périphérique avant l'arrêt du système.
void RGBLEDStripMode::shutdown()
{
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
ColorMode::ColorMode(const __FlashStringHelper *friendlyName, int ID, RGBLEDStrip &strip, HomeAssistant &connection) : RGBLEDStripMode(friendlyName, ID, strip), m_connection(connection), m_R(255), m_G(255), m_B(255) {}

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

int ColorMode::getR() const
{
    return m_R;
}

int ColorMode::getG() const
{
    return m_G;
}

int ColorMode::getB() const
{
    return m_B;
}

void ColorMode::activate()
{
    RGBLEDStripMode::activate();

    m_strip.setColor(m_R, m_G, m_B);
}

void ColorMode::desactivate()
{
    RGBLEDStripMode::desactivate();
}

void ColorMode::loop() {}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param strip Le ruban de DEL utilisé pour l'animation.
AlarmMode::AlarmMode(const __FlashStringHelper *friendlyName, int ID, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, ID, strip), m_lastTime(0) {}

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
RainbowMode::RainbowMode(const __FlashStringHelper *friendlyName, int ID, RGBLEDStrip &strip, int EEPROMSpeed) : RGBLEDStripMode(friendlyName, ID, strip), m_lastTime(0), m_lastSave(0), m_speedToSave(false), m_step(0), m_increment(1), m_delay(10), m_speed(EEPROM.read(EEPROMSpeed)), m_EEPROMSpeed(EEPROMSpeed) {}

/// @brief Définit la vitesse de l'animation arc-en-ciel.
/// @param speed La vitesse, de `0` (lent) à `100` (très rapide).
void RainbowMode::setAnimationSpeed(int speed)
{
    if (speed == m_speed)
        return;

    if (speed < 0)
        speed = 0;

    if (speed > 100)
        speed = 100;

    m_speed = speed;
    m_speedToSave = true;

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

    if (m_speedToSave && ((actualTime - m_lastSave) >= 600000))
    {
        m_speedToSave = false;

        EEPROM.update(m_EEPROMSpeed, m_speed);
    }

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

SoundreactMode::SoundreactMode(const __FlashStringHelper *friendlyName, int ID, RGBLEDStrip &strip, AnalogInput &microphone, int EEPROMSensitivity) : RGBLEDStripMode(friendlyName, ID, strip), m_microphone(microphone), m_lastSave(0), m_sensitivityToSave(false), m_sensitivity(EEPROM.read(EEPROMSensitivity)), m_lastColorChange(0), m_lastTime(0), m_maxSound(0), m_EEPROMSensitivity(EEPROMSensitivity) {}

/// @brief Méthode permettant de définir la sensibilité de l'animation son-réaction.
/// @param sensitivity La sensibilité à définir, en pourcent (de `0` : peu sensible à `100` : très sensible).
void SoundreactMode::setSensitivity(int sensitivity)
{
    if (sensitivity == m_sensitivity)
        return;

    if (sensitivity < 0)
        sensitivity = 0;

    if (sensitivity > 100)
        sensitivity = 100;

    m_sensitivity = sensitivity;
    m_sensitivityToSave = true;
}

int SoundreactMode::getSensitivity()
{
    return m_sensitivity;
}

void SoundreactMode::activate()
{
    RGBLEDStripMode::activate();

    m_lastColorChange = millis();
    m_lastTime = millis();
}

void SoundreactMode::desactivate()
{
    RGBLEDStripMode::desactivate();

    m_lastColorChange = 0;
    m_lastTime = 0;
    m_maxSound = 0;
}

void SoundreactMode::loop()
{
    if (m_sensitivityToSave && ((millis() - m_lastSave) >= 600000))
    {
        m_sensitivityToSave = false;

        EEPROM.update(m_EEPROMSensitivity, m_sensitivity);
    }

    int sound = m_microphone.getValue();
    sound -= 287;
    sound = abs(sound);

    bool maxChanged = false;
    if (sound > m_maxSound)
    {
        m_maxSound = (m_maxSound + (4 * sound)) / 5;
        maxChanged = true;
    }

    unsigned long time = millis();

    bool colorChanged = false;

    if ((time - m_lastColorChange) >= 200)
    {
        if (sound >= ((1.0 - (double(m_sensitivity) / 100.0)) * m_maxSound))
        {
            m_lastColorChange = time;

            bool rSelected = random(2);
            bool gSelected = random(2);
            bool bSelected = random(2);

            int rValue = 0;
            int gValue = 0;
            int bValue = 0;

            if (rSelected)
            {
                if (maxChanged)
                    rValue = 255;

                else
                    rValue = random(100, 256);
            }

            if (gSelected)
            {
                if (maxChanged)
                    gValue = 255;

                else
                    gValue = random(100, 256);
            }

            if (bSelected)
            {
                if (maxChanged)
                    bValue = 255;

                else
                    bValue = random(100, 256);
            }

            m_strip.setColor(rValue, gValue, bValue);
        }
    }

    if ((time - m_lastTime) >= 300)
    {
        m_lastTime = time;

        if (!maxChanged)
            m_maxSound--;

        if ((time - m_lastColorChange) >= 1000)
            m_maxSound -= 2;

        if (!colorChanged)
        {
            int rValue = 0;
            int gValue = 0;
            int bValue = 0;

            if ((m_strip.getR() - 5) >= 0)
                rValue = m_strip.getR() - 5;

            if ((m_strip.getG() - 5) >= 0)
                gValue = m_strip.getG() - 5;

            if ((m_strip.getB() - 5) >= 0)
                bValue = m_strip.getB() - 5;

            m_strip.setColor(rValue, gValue, bValue);
        }
    }
}

MusicsAnimationsMode::MusicsAnimationsMode(const __FlashStringHelper *friendlyName, int ID, RGBLEDStrip &strip) : RGBLEDStripMode(friendlyName, ID, strip), m_currentEffect(SINGLE_COLOR), m_smoothTransitionInitialR(0), m_smoothTransitionInitialG(0), m_smoothTransitionInitialB(0), m_smoothTransitionFinalR(0), m_smoothTransitionFinalG(0), m_smoothTransitionFinalB(0), m_smoothTransitionInitialMillis(0), m_smoothTransitionDuration(0), m_strobeEffectR(0), m_strobeEffectG(0), m_strobeEffectB(0), m_strobeEffectSpeed(0), m_strobeEffectStep(false), m_strobeEffectLastMillis(0) {}

void MusicsAnimationsMode::singleColor(int r, int g, int b)
{
    m_currentEffect = SINGLE_COLOR;

    m_strip.setColor(r, g, b);
}

void MusicsAnimationsMode::smoothTransition(int initialR, int initialG, int initialB, int finalR, int finalG, int finalB, unsigned long duration, MusicsAnimationsEasing type)
{
    if (initialR > 255)
        initialR = 255;

    if (initialR < 0)
        initialR = 0;

    if (initialG > 255)
        initialG = 255;

    if (initialG < 0)
        initialG = 0;

    if (initialB > 255)
        initialB = 255;

    if (initialB < 0)
        initialB = 0;

    if (finalR > 255)
        finalR = 255;

    if (finalR < 0)
        finalR = 0;

    if (finalG > 255)
        finalG = 255;

    if (finalG < 0)
        finalG = 0;

    if (finalB > 255)
        finalB = 255;

    if (finalB < 0)
        finalB = 0;

    m_currentEffect = SMOOTH_TRANSITION;
    m_smoothTransitionType = type;
    m_smoothTransitionInitialR = initialR;
    m_smoothTransitionInitialG = initialG;
    m_smoothTransitionInitialB = initialB;
    m_smoothTransitionFinalR = finalR;
    m_smoothTransitionFinalG = finalG;
    m_smoothTransitionFinalB = finalB;
    m_smoothTransitionInitialMillis = millis();
    m_smoothTransitionDuration = duration;

    m_strip.setColor(initialR, initialG, initialB);
}

void MusicsAnimationsMode::strobeEffect(int r, int g, int b, int speed)
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

    m_strobeEffectR = r;
    m_strobeEffectG = g;
    m_strobeEffectB = b;
    m_strobeEffectSpeed = speed;
    m_strobeEffectLastMillis = millis();
    m_strobeEffectStep = false;
    m_currentEffect = STROBE_EFFECT;

    m_strip.setColor(r, g, b);
}

void MusicsAnimationsMode::activate()
{
    if (m_activated)
        return;

    RGBLEDStripMode::activate();

    m_currentEffect = SINGLE_COLOR;
}

void MusicsAnimationsMode::desactivate()
{
    RGBLEDStripMode::desactivate();
}

void MusicsAnimationsMode::loop()
{
    switch (m_currentEffect)
    {
    case SINGLE_COLOR:
        break;

    case SMOOTH_TRANSITION:
    {
        float progression = float(millis() - m_smoothTransitionInitialMillis) / float(m_smoothTransitionDuration);

        if (progression > 1.0f)
        {
            m_currentEffect = SINGLE_COLOR;
            break;
        }

        switch (m_smoothTransitionType)
        {
        case LINEAR:
            break;

        case IN_CUBIC:
            progression = progression * progression * progression;
            break;

        case OUT_CUBIC:
        {
            float f = (progression - 1.0f);
            progression = f * f * f + 1.0f;
            break;
        }

        case IN_OUT_CUBIC:
        {
            if (progression < 0.5f)
                progression = 4.0f * progression * progression * progression;

            else
            {
                float f = ((2.0f * progression) - 2.0f);
                progression = 0.5f * f * f * f + 1.0f;
            }

            break;
        }
        }

        int newR = m_smoothTransitionInitialR + int(progression * float(m_smoothTransitionFinalR - m_smoothTransitionInitialR));
        int newG = m_smoothTransitionInitialG + int(progression * float(m_smoothTransitionFinalG - m_smoothTransitionInitialG));
        int newB = m_smoothTransitionInitialB + int(progression * float(m_smoothTransitionFinalB - m_smoothTransitionInitialB));

        m_strip.setColor(newR, newG, newB);

        break;
    }

    case STROBE_EFFECT:
    {
        if (m_strobeEffectLastMillis <= (millis() - m_strobeEffectSpeed))
        {
            if (m_strobeEffectStep)
                m_strip.setColor(m_strobeEffectR, m_strobeEffectG, m_strobeEffectB);

            else
                m_strip.setColor(0, 0, 0);

            m_strobeEffectLastMillis = millis();
            m_strobeEffectStep = !m_strobeEffectStep;
        }

        break;
    }
    }
}
