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
#include "../interface/buzzer.hpp"
#include "../../EEPROM.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param serial Le port série pour communiquer avec le lecteur de carte NFC.
/// @param doorLED La DEL rouge de la porte.
/// @param beacon Le gyrophare.
/// @param strip Le ruban de DEL RVB.
/// @param missileLauncherSerial Le port série connecté au lance-missile.
/// @param buzzer Le buzzer.
/// @param alarmRelayPin La broche connectée au relais de l'alarme.
/// @param buzzerState L'activation ou non du son de l'alarme.
Alarm::Alarm(String friendlyName, Display &display, HardwareSerial &serial, BinaryOutput &doorLED, BinaryOutput &beacon, RGBLEDStrip &strip, HardwareSerial &missileLauncherSerial, Buzzer &buzzer, int alarmRelayPin, bool buzzerState) : Output(friendlyName, display), m_pn532hsu(serial), m_nfcReader(m_pn532hsu), m_doorLED(doorLED), m_beacon(beacon), m_strip(strip), m_alarmStripMode("Mode alarme de l'alarme '" + m_friendlyName + "'", m_strip), m_previousMode(nullptr), m_missileLauncher(&missileLauncherSerial), m_buzzer(buzzer), m_alarmRelayPin(alarmRelayPin), m_isRinging(false), m_buzzerState(buzzerState), m_lastTimeAutoTriggerOff(0), m_lastTimeCardChecked(0), m_cardToStoreState(false) {}

/// @brief Initialise l'objet.
void Alarm::setup()
{
    if (m_operational)
        return;

    Output::setup();

    pinMode(m_alarmRelayPin, OUTPUT);

    m_doorLED.setup();
    m_beacon.setup();
    m_strip.setup();
    m_buzzer.setup();

    if (!m_missileLauncher.begin())
    {
        // sendLogMessage(ERROR, "L'alarme '" + m_friendlyName + "' n'a pas pu être initialisée car le lance-missile ne répond pas.");
        return;
    }

    m_nfcReader.begin();
    m_nfcReader.SAMConfig();
    uint32_t versiondata = m_nfcReader.getFirmwareVersion();

    if (versiondata)
    {
        m_operational = true;

        // sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est initialisée.");
    }

    else
    {
        // sendLogMessage(ERROR, "L'alarme '" + m_friendlyName + "' n'a pas pu être initialisée car le lecteur NFC ne répond pas.");
        return;
    }
}

/// @brief Mise en marche l'alarme.
/// @param shareInformation Affiche ou non l'animation d'allumage.
void Alarm::turnOn(bool shareInformation)
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

    // sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est allumée.");
}

/// @brief Arrête l'alarme.
/// @param shareInformation Affiche ou non l'animation d'arrêt.
void Alarm::turnOff(bool shareInformation)
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

    // sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est éteinte.");
}

/// @brief Méthode d'exécution des tâches liées à l'alarme.
void Alarm::loop()
{
    // Vérification des cartes NFC une fois par seconde.
    if ((millis() - m_lastTimeCardChecked) >= 1000)
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

        m_lastTimeCardChecked = millis();
    }

    if (!m_isRinging)
        return;

    // Gestion de l'arrêt automatique de l'alarme.
    if ((millis() - m_lastTimeAutoTriggerOff) >= 5000)
        stopRinging();
}

/// @brief Démarre le mode enregistrement de carte.
void Alarm::storeCard()
{
    if (!m_operational || m_locked || m_state || m_cardToStoreState)
    {
        // sendLogMessage(ERROR, "Impossible d'enregistrer une nouvelle carte pour le moment.");
        m_display.displayMessage("Action impossible.", "Erreur");
        m_buzzer.noSound();

        return;
    }

    m_cardToStoreState = true;

    // sendLogMessage(INFO, "Le système de domotique est prêt a enregistrer une nouvelle carte.");
    m_display.displayMessage("Presentez la carte a enregistrer.");
    m_buzzer.yesSound();
}

/// @brief Supprime toutes les cartes enregistrées dans le système.
void Alarm::removeCards()
{
    int storedCardsNumber = EEPROM.read(EEPROM_STORED_CARD_COUNTER);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        int storeLocation = i * 5 + EEPROM_CARDS;

        for (int j = 0; j < 5; j++)
            EEPROM.write(storeLocation + j, 0);
    }

    // sendLogMessage(INFO, String(storedCardsNumber) + " cartes enregistrées ont été supprimées.");
    m_display.displayMessage("Les cartes enregistrees ont ete supprimees.");
    m_buzzer.yesSound();
}

/// @brief Déclenche l'alarme.
void Alarm::trigger()
{
    if (m_isRinging)
        return;

    if (!m_state)
    {
        turnOn();

        if (!m_state)
        {
            // sendLogMessage(ERROR, "Impossible de déclencher l'alarme car il est impossible de l'allumer.");
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
    m_lastTimeAutoTriggerOff = millis();

    // sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' a été déclenchée.");
    m_display.displayAlarmTriggered(false);

    m_missileLauncher.absoluteMove(BASE, 110);
    m_missileLauncher.absoluteMove(ANGLE, 10);

    int baseAngle = 0;
    int angleAngle = 0;
    m_missileLauncher.getPosition(baseAngle, angleAngle);

    if (baseAngle < 45)
        delay(2000);

    m_missileLauncher.launchMissile(3);
}

/// @brief Arrête (de sonner) l'alarme.
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

/// @brief Méthode permettant de récupérer l'objet du lance-missile.
/// @return Le lance-missile.
MissileLauncher &Alarm::getMissileLauncher()
{
    return m_missileLauncher;
}

/// @brief Désactive le son de l'alarme.
void Alarm::disableBuzzer()
{
    if (m_locked)
        return;

    m_buzzerState = false;

    EEPROM.update(EEPROM_ALARM_BUZZER_STATE, m_buzzerState);
}

/// @brief Active le son de l'alarme.
void Alarm::enableBuzzer()
{
    if (m_locked)
        return;

    m_buzzerState = true;
    
    EEPROM.update(EEPROM_ALARM_BUZZER_STATE, m_buzzerState);
}

/// @brief Méthode permettant de savoir si le buzzer de l'alarme est activé ou non.
/// @return L'état d'activation du buzzer.
bool Alarm::getBuzzerState() const
{
    return m_buzzerState;
}

bool Alarm::checkCard(uint8_t card[4]) const
{
    int storedCardsNumber = EEPROM.read(EEPROM_STORED_CARD_COUNTER);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        if (EEPROM.read(i * 5 + EEPROM_CARDS) == card[0] && EEPROM.read(i * 5 + EEPROM_CARDS + 1) == card[1] && EEPROM.read(i * 5 + EEPROM_CARDS + 2) == card[2] && EEPROM.read(i * 5 + EEPROM_CARDS + 3) == card[3])
            return true;
    }

    return false;
}

void Alarm::storeCard(uint8_t card[4])
{
    if (checkCard(card))
    {
        // sendLogMessage(ERROR, "Cette carte est déjà enregistrée dans le système.");
        m_display.displayMessage("Cette carte a deja ete enregistree.", "Erreur");
        m_buzzer.noSound();

        return;
    }

    int storeLocation = EEPROM.read(EEPROM_STORED_CARD_COUNTER) * 5 + EEPROM_CARDS;

    for (int i = 0; i < 4; i++)
        EEPROM.write(storeLocation + i, card[i]);

    EEPROM.write(EEPROM_STORED_CARD_COUNTER, EEPROM.read(EEPROM_STORED_CARD_COUNTER) + 1);

    // sendLogMessage(INFO, "La carte a été enregistrée dans le système.");
    m_display.displayMessage("La carte a ete enregistree dans le systeme.");
    m_buzzer.yesSound();
}