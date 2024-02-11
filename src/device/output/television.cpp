/**
 * @file device/output/television.cpp
 * @author Louis L
 * @brief Classe gérant une télévision.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "television.hpp"
#include "../../logger.hpp"

Television::Television(String friendlyName, Display &display, int servomotorPin, int IRLEDPin) : Output(friendlyName, display), m_servomotorPin(servomotorPin), m_IRLEDPin(IRLEDPin), m_IRSender(), m_volume(0), m_volumeMuted(false) {}

void Television::setup()
{
    if (m_operational)
        return;

    Output::setup();

    pinMode(m_servomotorPin, OUTPUT);
    m_IRSender.begin(m_IRLEDPin);

    m_operational = true;

    sendLogMessage(INFO, "La télévision '" + m_friendlyName + "' est initialisé.");
}

void Television::turnOn(bool shareInformation)
{
    if (m_operational && !m_locked && !m_state)
    {
        switchDisplay();
        m_IRSender.sendNEC(0x44C1, 0x87, 3);

        m_state = true;

        if (shareInformation)
            m_display.displayDeviceState(true);

        sendLogMessage(INFO, "La télévision '" + m_friendlyName + "' est allumée.");
    }
}

void Television::turnOff(bool shareInformation)
{
    if (m_operational && !m_locked && m_state)
    {
        switchDisplay();
        m_IRSender.sendNEC(0x44C1, 0x87, 3);

        m_state = false;

        if (shareInformation)
            m_display.displayDeviceState(false);

        sendLogMessage(INFO, "La télévision '" + m_friendlyName + "' est éteinte.");
    }
}

void Television::syncVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        sendLogMessage(ERROR, "La calibration du son de la télévision '" + m_friendlyName + "' n'a pas pu être effectuée.");

        return;
    }

    if (shareInformation)
        m_display.displayMessage("Calibration du son...");

    IrSender.sendNEC(0x44C1, 0xC7, 50);

    for (int i = 0; i < m_volume; i ++)
        increaseVolume();

    if (shareInformation)
        m_display.displayMessage("Calibration terminee !");
}

void Television::increaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 25))
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        sendLogMessage(ERROR, "L'augmentation du volume de la télévision '" + m_friendlyName + "' n'a pas pu être effectuée.");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x47, 2);
    m_volume++;

    if (shareInformation)
        m_display.displayVolume(INCREASE, m_volume);
}

void Television::decreaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 0))
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        sendLogMessage(ERROR, "La diminution du volume de la télévision '" + m_friendlyName + "' n'a pas pu être effectuée.");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0xC7, 2);
    m_volume--;

    if (shareInformation)
        m_display.displayVolume(DECREASE, m_volume);
}

int Television::getVolume()
{
    return m_volume;
}

void Television::mute(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        sendLogMessage(ERROR, "La coupure du son de la télévision '" + m_friendlyName + "' n'a pas pu être effectuée.");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x77, 3);
    m_volumeMuted = true;

    if (shareInformation)
        m_display.displayVolume(MUTE, m_volume);
}

void Television::unMute(bool shareInformation)
{
    if (!m_state || m_locked || !m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        sendLogMessage(ERROR, "Le réetablissement du son de la télévision '" + m_friendlyName + "' n'a pas pu être effectuée.");

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x77, 3);
    m_volumeMuted = false;

    if (shareInformation)
        m_display.displayVolume(UNMUTE, m_volume);
}

void Television::toggleMute(bool shareInformation)
{
    if (m_volumeMuted)
        unMute();

    else
        mute();
}

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