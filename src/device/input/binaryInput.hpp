#ifndef BINARY_INPUT_DEFINITIONS
#define BINARY_INPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/input/input.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/alarm.hpp"
#include "device/interface/display.hpp"
#include "device/interface/buzzer.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Classe représentant un capteur dont la valeur mesurée est binaire.
class BinaryInput : public Input
{
public:
    BinaryInput(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert = false, bool pullup = false);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void loop() override;
    virtual bool getState();

protected:
    bool m_state;
    const unsigned int m_pin;
    const bool m_reverted;
    const bool m_pullup;
};

/// @brief Classe représentant un capteur de porte d'armoire.
class WardrobeDoorSensor : public BinaryInput
{
public:
    WardrobeDoorSensor(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup, BinaryOutput &output);
    virtual void setup() override;
    virtual void loop() override;
    virtual void activate();
    virtual void desactivate();
    virtual void toggleActivation();
    virtual bool getActivation() const;

protected:
    BinaryOutput &m_output;
    bool m_activated;
};

/// @brief Classe représentant un capteur de porte de chambre.
class DoorSensor : public BinaryInput
{
public:
    DoorSensor(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup, Alarm &alarm);
    virtual void setup() override;
    virtual void loop() override;

protected:
    Alarm &m_alarm;
};

/// @brief Classe représentant un bouton de sonnette.
class Doorbell : public BinaryInput
{
public:
    Doorbell(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup, Display &display, Buzzer &buzzer);
    virtual void setup() override;
    virtual void loop() override;

protected:
    Display &m_display;
    Buzzer &m_buzzer;
    unsigned long m_lastTime;
};

#endif