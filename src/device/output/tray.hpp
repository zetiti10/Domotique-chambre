#ifndef TRAY_DEFINITIONS
#define TRAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Classe gérant un plateau.
class Tray : public Output
{
public:
    Tray(const __FlashStringHelper* friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int motorPin1, unsigned int motorPin2, unsigned int speedPin);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;

protected:
    const unsigned int m_motorPin1;
    const unsigned int m_motorPin2;
    const unsigned int m_speedPin;
};

#endif