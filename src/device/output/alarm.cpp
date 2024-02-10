/**
 * @file device/output/alarm.cpp
 * @author Louis L
 * @brief Classe représentant l'alarme du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include "alarm.hpp"
#include "../../logger.hpp"
#include "../buzzer.hpp"

Alarm::Alarm(String friendlyName, Display &display, HardwareSerial &serial, BinaryOutput &doorLED, BinaryOutput &beacon, RGBLEDStrip &strip, MissileLauncher &missileLauncher, Buzzer &buzzer, int alarmRelayPin, boolean buzzerState) : Output(friendlyName, display), m_pn532hsu(serial), m_nfcReader(m_pn532hsu), m_doorLED(doorLED), m_beacon(beacon), m_strip(strip), m_alarmStripMode("Mode alarme de l'alarme '" + m_friendlyName + "'", m_strip), m_previousMode(nullptr), m_missileLauncher(missileLauncher), m_buzzer(buzzer), m_alarmRelayPin(alarmRelayPin), m_isRinging(false), m_buzzerState(buzzerState), m_autoTriggerOffCounter(0), m_cardCounter(0), m_cardToStoreState(false) {}

void Alarm::setup()
{
    Output::setup();

    pinMode(m_alarmRelayPin, OUTPUT);

    m_doorLED.setup();
    m_beacon.setup();
    m_strip.setup();
    m_buzzer.setup();

    if (!m_missileLauncher.begin())
        sendLogMessage(WARN, "Le lance-missile n'a pas pu être initialisé lors de l'initialisation de l'alarme '" + m_friendlyName + "'.");

    m_nfcReader.begin();
    m_nfcReader.SAMConfig();
    uint32_t versiondata = m_nfcReader.getFirmwareVersion();

    if (versiondata)
    {
        m_operational = true;

        sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est initialisée.");
    }

    else
        sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' n'a pas pu être initialisée car le lecteur NFC ne répond pas.");
}

void Alarm::turnOn(boolean shareInformation)
{
    if (!m_operational || m_locked || m_state || m_cardToStoreState || m_strip.isLocked() || m_beacon.isLocked())
        return;

    m_previousMode = &m_strip.getMode();

    m_doorLED.turnOn();
    m_beacon.turnOff();
    m_strip.turnOff();

    m_doorLED.lock();
    m_beacon.lock();
    m_strip.lock();

    m_state = true;

    if (shareInformation)
        m_display.displayDeviceState(true);

    sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est allumée.");
}

void Alarm::turnOff(boolean shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    if (m_isRinging)
        stopRinging();

    m_doorLED.unLock();
    m_beacon.unLock();
    m_strip.unLock();

    m_strip.setMode(*m_previousMode);

    m_doorLED.turnOff();

    m_state = false;

    if (shareInformation)
        m_display.displayDeviceState(false);

    sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est éteinte.");
}

boolean Alarm::checkCard(uint8_t card[4])
{
    int storedCardsNumber = EEPROM.read(0);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        if (EEPROM.read(i * 5 + 11) == card[0] && EEPROM.read(i * 5 + 12) == card[1] && EEPROM.read(i * 5 + 13) == card[2] && EEPROM.read(i * 5 + 14) == card[3])
            return true;
    }

    return false;
}

void Alarm::loop()
{
    if ((millis() - m_cardCounter) >= 1000)
    {
        uint8_t uid[] = {0, 0, 0, 0, 0};
        uint8_t uidLength;

        if (m_nfcReader.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100))
        {
            if (m_cardToStoreState)
            {
                storeCard(uid);
                m_cardToStoreState = false;
            }

            else if (checkCard(uid))
            {
                m_buzzer.yesSound();

                if (m_isRinging)
                    stopRinging();

                else
                    toggle();
            }
        }

        m_cardCounter = millis();
    }

    if (!m_isRinging)
        return;

    if ((millis() - m_autoTriggerOffCounter) >= 5000)
        stopRinging();
}

void Alarm::storeCard()
{
    if (!m_operational || m_locked || m_state || m_cardToStoreState)
        return;

    m_cardToStoreState = true;

    sendLogMessage(INFO, "Le système de domotique est prêt a enregistrer une nouvelle carte.");
    m_display.displayMessage("Presentez la carte a enregistrer.");
    m_buzzer.yesSound();
}

void Alarm::storeCard(uint8_t card[4])
{
    if (checkCard(card))
    {
        sendLogMessage(ERROR, "Cette carte est déjà enregistrée dans le système.");
        m_display.displayMessage("Cette carte a deja ete enregistree.", "Erreur");
        m_buzzer.noSound();
        return;
    }

    int storeLocation = EEPROM.read(0) * 5 + 11;

    for (int i = 0; i < 4; i++)
        EEPROM.write(storeLocation + i, card[i]);

    EEPROM.write(0, EEPROM.read(0) + 1);

    sendLogMessage(INFO, "La carte a été enregistrée dans le système.");
    m_display.displayMessage("La carte a ete enregistree dans le systeme.");
    m_buzzer.yesSound();
}

void Alarm::removeCards()
{
    int storedCardsNumber = EEPROM.read(0);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        int storeLocation = i * 5 + 11;

        for (int j = 0; j < 5; j++)
            EEPROM.write(storeLocation + j, 0);
    }

    sendLogMessage(INFO, "Les cartes enregistrées ont été supprimées.");
    m_display.displayMessage("Les cartes enregistrees ont ete supprimees.");
    m_buzzer.yesSound();
}

void Alarm::trigger()
{
    if (m_isRinging)
        return;

    if (!m_state)
    {
        turnOn();

        if (!m_state)
        {
            sendLogMessage(ERROR, "Impossible de déclencher l'alarme car il est impossible de l'allumer.");
            return;
        }
    }

    if (m_buzzerState)
        digitalWrite(m_alarmRelayPin, HIGH);

    m_beacon.unLock();
    m_strip.unLock();

    m_beacon.turnOn();
    m_strip.setMode(m_alarmStripMode);
    m_strip.turnOn();

    m_beacon.lock();
    m_strip.lock();

    m_isRinging = true;
    m_autoTriggerOffCounter = millis();

    m_missileLauncher.absoluteMove(BASE, 110);
    m_missileLauncher.absoluteMove(ANGLE, 10);

    int baseAngle = 0;
    int angleAngle = 0;
    m_missileLauncher.getPosition(baseAngle, angleAngle);

    if (baseAngle < 45)
        delay(2000);

    m_missileLauncher.launchMissile(3);

    sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' a été déclenchée.");
    m_display.displayAlarmTriggered(false);
}

void Alarm::stopRinging()
{
    if (!m_isRinging)
        return;

    if (m_buzzerState)
        digitalWrite(m_alarmRelayPin, LOW);

    m_beacon.unLock();
    m_strip.unLock();

    m_beacon.turnOff();
    m_strip.turnOff();

    m_beacon.lock();
    m_strip.lock();

    m_isRinging = true;
}