#ifndef ALARM_DEFINITIONS
#define ALARM_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <PN532_HSU.h>
#include <PN532.h>
#include <missileLauncher.hpp>

// Autres fichiers du programme.
#include "output.hpp"
#include "binaryOutput.hpp"
#include "../display.hpp"
#include "RGBLEDStrip.hpp"
#include "../buzzer.hpp"

class Alarm : public Output
{
public:
    Alarm(String friendlyName, Display &display, HardwareSerial &serial, BinaryOutput &doorLED, BinaryOutput &beacon, RGBLEDStrip &strip, MissileLauncher &missileLauncher, Buzzer &buzzer, int alarmRelayPin, bool buzzerState);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void loop();
    virtual void storeCard();
    virtual void removeCards();
    virtual void trigger();
    virtual void stopRinging();

protected:
    virtual bool checkCard(uint8_t card[4]);
    virtual void storeCard(uint8_t card[4]);

    PN532_HSU m_pn532hsu;
    PN532 m_nfcReader;
    BinaryOutput &m_doorLED;
    BinaryOutput &m_beacon;
    RGBLEDStrip &m_strip;
    AlarmMode m_alarmStripMode;
    RGBLEDStripMode *m_previousMode;
    MissileLauncher &m_missileLauncher;
    Buzzer &m_buzzer;
    const int m_alarmRelayPin;
    bool m_isRinging;
    bool m_buzzerState;
    unsigned long m_autoTriggerOffCounter;
    unsigned long m_cardCounter;
    bool m_cardToStoreState;
};

#endif