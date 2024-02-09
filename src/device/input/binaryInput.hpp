#ifndef BINARY_INPUT_DEFINITIONS
#define BINARY_INPUT_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "input.hpp"
#include "../output/binaryOutput.hpp"
#include "../output/alarm.hpp"
#include "../display.hpp"
#include "../buzzer.hpp"

class BinaryInput : public Input
{
public:
    BinaryInput(String friendlyName, int pin, boolean revert = false, boolean pullup = false);
    virtual void setup() override;
    virtual void loop() override;
    virtual boolean getState();

protected:
    boolean m_state;
    int m_pin;
    boolean m_reverted;
    boolean m_pullup;
};

class WardrobeDoorSensor : public BinaryInput
{
public:
    WardrobeDoorSensor(String friendlyName, int pin, boolean revert, boolean pullup, BinaryOutput &output);
    virtual void loop() override;
    virtual void activate();
    virtual void desactivate();
    virtual boolean getActivation() const;

protected:
    BinaryOutput &m_output;
    boolean m_activated;
};

class DoorSensor : public BinaryInput
{
public:
    DoorSensor(String friendlyName, int pin, boolean revert, boolean pullup, Alarm &alarm);
    virtual void loop();

protected:
    Alarm &m_alarm;
};

class Doorbell : public BinaryInput
{
public:
    Doorbell(String friendlyName, int pin, boolean revert, boolean pullup, Display &display, Buzzer &buzzer);
    virtual void setup() override;
    virtual void loop() override;

protected:
    Display &m_display;
    Buzzer &m_buzzer;
    unsigned long m_delay;
};

#endif