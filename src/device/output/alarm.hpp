#ifndef ALARM_DEFINITIONS
#define ALARM_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <missileLauncher.hpp>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/interface/buzzer.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/RGBLEDStrip.hpp"

/// @brief Classe intégrant toutes les fonctionnalités nécessaires au fonctionnement d'une alarme.
class Alarm : public Output
{
public:
    Alarm(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, HardwareSerial &serial, BinaryOutput &doorLED, BinaryOutput &beacon, RGBLEDStrip &strip, AlarmMode &alarmMode, HardwareSerial &missileLauncherSerial, Buzzer &buzzer, unsigned int alarmRelayPin, unsigned int EEPROMBuzzerState, unsigned int EEPROMCardNumber, unsigned int EEPROMCards);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void loop();
    virtual void storeCard();
    virtual void removeCards();
    virtual void trigger();
    virtual void stopRinging();
    virtual MissileLauncher &getMissileLauncher() const;
    virtual AlarmMode &getAlarmStripMode() const;
    virtual void disableBuzzer();
    virtual void enableBuzzer();
    virtual void toggleBuzzer();
    virtual bool getBuzzerState() const;

protected:
    virtual bool checkCard(uint8_t card[4]) const;
    virtual void storeCard(uint8_t card[4]);

    PN532_HSU m_pn532hsu;
    PN532 m_nfcReader;
    BinaryOutput &m_doorLED;
    BinaryOutput &m_beacon;
    RGBLEDStrip &m_strip;
    AlarmMode &m_alarmStripMode;
    RGBLEDStripMode *m_previousMode;
    MissileLauncher m_missileLauncher;
    Buzzer &m_buzzer;
    const unsigned int m_alarmRelayPin;
    bool m_isRinging;
    unsigned long m_lastTimeAutoTriggerOff;
    unsigned long m_lastTimeCardChecked;
    bool m_cardToStoreState;
    const unsigned int m_EEPROMBuzzerState;
    const unsigned int m_EEPROMCardNumber;
    const unsigned int m_EEPROMCards;
};

#endif