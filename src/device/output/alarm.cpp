/**
 * @file device/output/alarm.cpp
 * @author Louis L
 * @brief Classe représentant l'alarme du système de domotique.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <missileLauncher.hpp>
#include <EEPROM.h>

// Autres fichiers du programme.
#include "device/output/alarm.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/buzzer.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "EEPROM.hpp"
#include "alarm.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param serial Le port série pour communiquer avec le lecteur de carte NFC.
/// @param doorLED La DEL rouge de la porte.
/// @param beacon Le gyrophare.
/// @param strip Le ruban de DEL RVB.
/// @param alarmMode Le mode de couleur de l'alarme pour le ruban de DEL RVB.
/// @param missileLauncherSerial Le port série connecté au lance-missile.
/// @param buzzer Le buzzer.
/// @param alarmRelayPin La broche connectée au relais de l'alarme.
/// @param EEPROMBuzzerState L'emplacement du stockage de l'état du buzzer dans la mémoire EEPROM.
/// @param EEPROMCardNumber L'emplacement du stockage du nombre de cartes enregistrées dans la mémoire EEPROM.
/// @param EEPROMCards L'emplacement du stockage initial des cartes enregistrées dans la mémoire EEPROM.
Alarm::Alarm(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, HardwareSerial &serial, BinaryOutput &doorLED, BinaryOutput &beacon, RGBLEDStrip &strip, AlarmMode &alarmMode, HardwareSerial &missileLauncherSerial, Buzzer &buzzer, unsigned int alarmRelayPin, unsigned int EEPROMBuzzerState, unsigned int EEPROMCardNumber, unsigned int EEPROMCards) : Output(friendlyName, ID, connection, display), m_pn532hsu(serial), m_nfcReader(m_pn532hsu), m_doorLED(doorLED), m_beacon(beacon), m_strip(strip), m_alarmStripMode(alarmMode), m_previousMode(nullptr), m_missileLauncher(&missileLauncherSerial), m_buzzer(buzzer), m_alarmRelayPin(alarmRelayPin), m_isRinging(false), m_lastTimeAutoTriggerOff(0), m_lastTimeCardChecked(0), m_cardToStoreState(false), m_EEPROMBuzzerState(EEPROMBuzzerState), m_EEPROMCardNumber(EEPROMCardNumber), m_EEPROMCards(EEPROMCards) {}

/// @brief Initialise l'objet.
void Alarm::setup()
{
    if (m_operational)
        return;

    Output::setup();

    pinMode(m_alarmRelayPin, OUTPUT);

    if (!m_missileLauncher.begin(200))
        return;

    m_doorLED.setup();
    m_beacon.setup();
    m_strip.setup();
    m_buzzer.setup();

    m_nfcReader.begin();
    m_nfcReader.SAMConfig();
    uint32_t versiondata = m_nfcReader.getFirmwareVersion();
    if (!versiondata)
        return;

    m_operational = true;
    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void Alarm::reportState()
{
    if (!m_operational)
        return;

    m_connection.updateAlarmTriggeredState(m_ID, m_isRinging);

    int baseAngle = -1;
    int angleAngle = -1;
    int firstMissile = -1;
    int secondMissile = -1;
    int thirdMissile = -1;

    m_missileLauncher.getPosition(baseAngle, angleAngle);
    m_missileLauncher.getMissileStates(firstMissile, secondMissile, thirdMissile);

    if (baseAngle != -1)
    {
        m_connection.updateAlarmMissileLauncherBaseAngle(m_ID, baseAngle);
        m_connection.updateAlarmMissileLauncherAngleAngle(m_ID, angleAngle);
    }

    if (firstMissile != -1)
        m_connection.updateAlarmMissileLauncherMissilesState(m_ID, firstMissile, secondMissile, thirdMissile);

    Output::reportState();
}

/// @brief Mise en marche l'alarme.
/// @param shareInformation Affiche ou non l'animation d'allumage.
void Alarm::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state || m_cardToStoreState || m_strip.isLocked() || m_beacon.isLocked())
        return;

    m_previousMode = m_strip.getMode();

    m_doorLED.turnOn();
    m_beacon.turnOff();
    m_strip.turnOff();

    m_doorLED.lock();
    m_beacon.lock();
    m_strip.lock();

    m_state = true;
    m_connection.updateOutputDeviceState(m_ID, true);

    if (shareInformation)
        m_display.displayDeviceState(true);
}

/// @brief Arrête l'alarme.
/// @param shareInformation Affiche ou non l'animation d'arrêt.
void Alarm::turnOff(bool shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    if (m_isRinging)
        this->stopRinging();

    m_doorLED.unLock();
    m_beacon.unLock();
    m_strip.unLock();

    m_strip.setMode(m_previousMode);

    m_doorLED.turnOff();

    m_state = false;
    m_connection.updateOutputDeviceState(m_ID, false);

    if (shareInformation)
        m_display.displayDeviceState(false);
}

/// @brief Méthode d'exécution des tâches liées à l'alarme.
void Alarm::loop()
{
    if (!m_operational || m_locked)
        return;

    // Vérification des cartes NFC une fois par seconde.
    if ((millis() - m_lastTimeCardChecked) >= 1000)
    {
        uint8_t uid[] = {0, 0, 0, 0, 0};
        uint8_t uidLength;

        if (m_nfcReader.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100))
        {
            if (m_cardToStoreState)
            {
                this->storeCard(uid);
                m_cardToStoreState = false;
            }

            else if (checkCard(uid))
            {
                m_buzzer.yesSound();

                if (m_isRinging)
                    this->stopRinging();

                else
                    this->toggle();
            }
        }

        m_lastTimeCardChecked = millis();
    }

    int baseAngle = -1;
    int angleAngle = -1;
    int firstMissile = -1;
    int secondMissile = -1;
    int thirdMissile = -1;
    m_missileLauncher.update(baseAngle, angleAngle, firstMissile, secondMissile, thirdMissile);

    if (baseAngle != -1)
    {
        m_connection.updateAlarmMissileLauncherBaseAngle(m_ID, baseAngle);
        m_connection.updateAlarmMissileLauncherAngleAngle(m_ID, angleAngle);
    }

    if (firstMissile != -1)
        m_connection.updateAlarmMissileLauncherMissilesState(m_ID, firstMissile, secondMissile, thirdMissile);

    // Gestion de l'arrêt automatique de l'alarme.
    if (m_isRinging && (millis() - m_lastTimeAutoTriggerOff) >= 5000)
        this->stopRinging();
}

/// @brief Démarre le mode enregistrement de carte.
void Alarm::storeCard()
{
    if (!m_operational || m_locked || m_state || m_cardToStoreState)
    {
        m_display.displayMessage("Action impossible.", "Erreur");
        m_buzzer.noSound();

        return;
    }

    m_cardToStoreState = true;

    m_display.displayMessage("Présentez la carte à enregistrer.");
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

    m_display.displayMessage("Les cartes enregistrées ont été supprimées.");
    m_buzzer.yesSound();
}

/// @brief Déclenche l'alarme.
void Alarm::trigger()
{
    if (m_isRinging)
    {
        m_lastTimeAutoTriggerOff = millis();
        return;
    }

    if (!m_state)
    {
        this->turnOn();

        if (!m_state)
            return;
    }

    if (this->getBuzzerState())
        digitalWrite(m_alarmRelayPin, HIGH);

    m_beacon.unLock();
    m_strip.unLock();

    m_beacon.turnOn();
    m_strip.setMode(&m_alarmStripMode);
    m_strip.turnOn();

    m_beacon.lock();
    m_strip.lock();

    m_isRinging = true;

    m_display.displayAlarmTriggered(false);

    m_connection.updateAlarmTriggeredState(m_ID, true);
    m_connection.sayMessage("Une intrusion à été détectée ! Vous êtes prié de quitter les lieux immédiatement.");

    m_lastTimeAutoTriggerOff = millis();

    m_missileLauncher.absoluteMove(BASE, 150);
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

    if (this->getBuzzerState())
        digitalWrite(m_alarmRelayPin, LOW);

    m_beacon.unLock();
    m_strip.unLock();

    m_beacon.turnOff();
    m_strip.turnOff();

    m_beacon.lock();
    m_strip.lock();

    m_isRinging = false;

    m_connection.updateAlarmTriggeredState(m_ID, false);
}

/// @brief Méthode permettant de récupérer l'objet du lance-missile.
/// @return Le lance-missile.
MissileLauncher &Alarm::getMissileLauncher()
{
    return m_missileLauncher;
}

/// @brief Méthode permettant de récupérer l'objet du mode alarme pour le ruban de DEL RVB.
/// @return Le mode de l'alarme.
AlarmMode &Alarm::getAlarmStripMode() const
{
    return m_alarmStripMode;
}

/// @brief Désactive le son de l'alarme.
void Alarm::disableBuzzer()
{
    if (m_locked)
        return;

    EEPROM.update(m_EEPROMBuzzerState, false);
}

/// @brief Active le son de l'alarme.
void Alarm::enableBuzzer()
{
    if (m_locked)
        return;

    EEPROM.update(m_EEPROMBuzzerState, true);
}

/// @brief Bascule l'état du son de l'alarme.
void Alarm::toggleBuzzer()
{
    if (this->getBuzzerState())
        disableBuzzer();

    else
        enableBuzzer();
}

/// @brief Méthode permettant de savoir si le buzzer de l'alarme est activé ou non.
/// @return L'état d'activation du buzzer.
bool Alarm::getBuzzerState() const
{
    return EEPROM.read(m_EEPROMBuzzerState);
}

bool Alarm::checkCard(uint8_t card[4]) const
{
    int storedCardsNumber = EEPROM.read(m_EEPROMCardNumber);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        if (EEPROM.read(i * 5 + m_EEPROMCards) == card[0] && EEPROM.read(i * 5 + m_EEPROMCards + 1) == card[1] && EEPROM.read(i * 5 + m_EEPROMCards + 2) == card[2] && EEPROM.read(i * 5 + m_EEPROMCards + 3) == card[3])
            return true;
    }

    return false;
}

/// @brief Enregistre une carte dans l'EEPROM, avec les vérifications nécessaires.
/// @param card L'identifiant de la carte à enregistrer.
void Alarm::storeCard(uint8_t card[4])
{
    if (this->checkCard(card))
    {
        m_display.displayMessage("Cette carte a déjà été enregistrée.", "Erreur");
        m_buzzer.noSound();

        return;
    }

    int storeLocation = EEPROM.read(m_EEPROMCardNumber) * 5 + m_EEPROMCards;

    for (int i = 0; i < 4; i++)
        EEPROM.write(storeLocation + i, card[i]);

    EEPROM.write(m_EEPROMCardNumber, EEPROM.read(m_EEPROMCardNumber) + 1);

    m_display.displayMessage("La carte a été enregistrée dans le système.");
    m_buzzer.yesSound();
}