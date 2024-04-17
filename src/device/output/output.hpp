#ifndef OUTPUT_DEFINITIONS
#define OUTPUT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/device.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

// Classe commune à tous les périphériques de sortie (actionneurs).
class Output : public Device
{
public:
    Output(String friendlyName, int ID, HomeAssistant &connection, Display &display);
    virtual void setup() override;
    virtual void reportState();
    virtual void turnOn(bool shareInformation = false) = 0;
    virtual void turnOff(bool shareInformation = false) = 0;
    virtual void toggle(bool shareInformation = false);
    virtual bool getState() const;
    virtual void lock();
    virtual void unLock();
    virtual bool isLocked() const;

protected:
    Display &m_display;
    HomeAssistant &m_connection;
    bool m_state;
    bool m_locked;
};

#endif