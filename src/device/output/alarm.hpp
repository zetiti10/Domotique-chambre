#ifndef ALARM_DEFINITIONS
#define ALARM_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <PN532_HSU.h>
#include <PN532.h>

// Autres fichiers du programme.
#include "output.hpp"

class Alarm : public Output
{
public:
    Alarm(String friendlyName, HardwareSerial &Serial, int alarmRelayPin, int beaconRelayPin, boolean buzzerState);
    virtual void setup() override;
    virtual void turnOn(boolean shareInformation = false) override;
    virtual void turnOff(boolean shareInformation = false) override;
    virtual void loop();
    virtual void storeCard();
    virtual void removeCards();
    virtual void trigger();

protected:
    PN532_HSU m_pn532hsu;
    PN532 m_nfcReader;
    int m_alarmRelayPin;
    int m_beaconRelayPin;
    boolean m_isRinging;
    boolean m_buzzerState;
    int m_autoTriggerOffCounter;
    boolean m_cardToStoreState;
};

#endif