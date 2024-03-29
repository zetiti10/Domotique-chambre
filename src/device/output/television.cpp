/**
 * @file device/output/television.cpp
 * @author Louis L
 * @brief Classe gérant une télévision.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include "device/output/television.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "EEPROM.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param servomotorPin La broche associée à celle du servomoteur.
/// @param IRLEDPin La broche associée à celle de la DEL infrarouge.
/// @param volume Le volume récupéré de l'EEPROM.
Television::Television(String friendlyName, int ID, HomeAssistant &connection, Display &display, int servomotorPin, int IRLEDPin, int volume) : Output(friendlyName, ID, connection, display), m_servomotorPin(servomotorPin), m_IRLEDPin(IRLEDPin), m_IRSender(), m_volume(volume), m_volumeMuted(false), m_lastTime(0) {}

/// @brief Initialise l'objet.
void Television::setup()
{
    if (m_operational)
        return;

    Output::setup();

    pinMode(m_servomotorPin, OUTPUT);
    m_IRSender.begin(m_IRLEDPin);

    m_lastTime = millis();

    m_operational = true;

    m_connection.updateDeviceAvailability(m_ID, true);
}

void Television::loop()
{
    if ((millis() - m_lastTime) >= 60000)
    {
        EEPROM.update(EEPROM_VOLUME, m_volume);

        m_lastTime = millis();
    }
}

/// @brief Met en marche la télévision.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void Television::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    m_connection.updateOutputDeviceState(m_ID, true);

    switchDisplay();
    m_IRSender.sendNEC(0x44C1, 0x87, 3);

    m_state = true;

    if (shareInformation)
        m_display.displayDeviceState(true);
}

/// @brief Arrête la télévision.
/// @param shareInformation Affiche ou non l'animation d'arrêt sur l'écran.
void Television::turnOff(bool shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    m_connection.updateOutputDeviceState(m_ID, false);

    switchDisplay();
    m_IRSender.sendNEC(0x44C1, 0x87, 3);

    m_state = false;

    if (shareInformation)
        m_display.displayDeviceState(false);
}

/// @brief Synchronise le volume virtuel avec celui de la sono.
/// @param shareInformation Affiche ou non les messages sur l'écran.
void Television::syncVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    if (shareInformation)
        m_display.displayMessage("Calibration du son...");

    IrSender.sendNEC(0x44C1, 0xC7, 50);

    for (int i = 0; i < m_volume; i++)
        increaseVolume();

    if (shareInformation)
        m_display.displayMessage("Calibration terminee !");
}

/// @brief Augmente le volume de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::increaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 25))
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x47, 2);
    m_volume++;

    m_connection.updateTelevisionVolume(m_ID, 0, m_volume);

    if (shareInformation)
        m_display.displayVolume(INCREASE, m_volume);
}

/// @brief Diminue le volume de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::decreaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 0))
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0xC7, 2);
    m_volume--;

    m_connection.updateTelevisionVolume(m_ID, 0, m_volume);

    if (shareInformation)
        m_display.displayVolume(DECREASE, m_volume);
}

/// @brief Méthode permettant de récupérer le volume actuel de la télévision.
/// @return Le volume actuel de la télévision.
int Television::getVolume()
{
    return m_volume;
}

/// @brief Désactive le son de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::mute(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x77, 3);
    m_volumeMuted = true;

    m_connection.updateTelevisionVolume(m_ID, 1);

    if (shareInformation)
        m_display.displayVolume(MUTE, m_volume);
}

/// @brief Réetablie le son de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::unMute(bool shareInformation)
{
    if (!m_state || m_locked || !m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x77, 3);
    m_volumeMuted = false;

    m_connection.updateTelevisionVolume(m_ID, 2);

    if (shareInformation)
        m_display.displayVolume(UNMUTE, m_volume);
}

/// @brief Bascule le son de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::toggleMute(bool shareInformation)
{
    if (m_volumeMuted)
        unMute();

    else
        mute();
}

/// @brief Méthode permettant de récupérer l'état actuel du mode sourdinne de la télévision.
/// @return L'état actuel du mode sourdinne de la télévision.
bool Television::getMute()
{
    return m_volumeMuted;
}

void Television::moveDisplayServo(int angle)
{
    int pulseWidth = map(angle, 0, 180, 600, 2400);
    digitalWrite(m_servomotorPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(m_servomotorPin, LOW);
    delay(2);
}

void Television::switchDisplay()
{
    for (int pos = 80; pos <= 130; pos++)
    {
        moveDisplayServo(pos);
        delay(1);
    }

    for (int i = 0; i < 100; i++)
        moveDisplayServo(130);

    for (int pos = 130; pos >= 80; pos--)
    {
        moveDisplayServo(pos);
        delay(1);
    }
}