/**
 * @file device/output/alarm.cpp
 * @author Louis L
 * @brief Classe représentant l'alarme du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "alarm.hpp"
#include "../../logger.hpp"

Alarm::Alarm(String friendlyName, HardwareSerial &serial, int alarmRelayPin, int beaconRelayPin, boolean buzzerState) : Output(friendlyName), m_pn532hsu(serial), m_nfcReader(m_pn532hsu), m_alarmRelayPin(alarmRelayPin), m_beaconRelayPin(beaconRelayPin), m_isRinging(false), m_buzzerState(buzzerState), m_autoTriggerOffCounter(0), m_cardToStoreState(false) {}

void Alarm::setup()
{
    pinMode(m_alarmRelayPin, OUTPUT);
    pinMode(m_beaconRelayPin, OUTPUT);

    m_nfcReader.begin();
    m_nfcReader.SAMConfig();
    uint32_t versiondata = m_nfcReader.getFirmwareVersion();
    
    if (versiondata)
    {
        m_operational = true;

        sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' est initialisée.");
    }

    else
        sendLogMessage(INFO, "L'alarme '" + m_friendlyName + "' n'a pas pu être initialisée.");
}