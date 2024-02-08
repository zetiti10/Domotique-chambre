#ifndef OUTPUT_DEFINITIONS
#define OUTPUT_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "../device.hpp"
#include "../display.hpp"

class Output : public Device
{
public:
    Output(String friendlyName, Display &display);
    virtual void setup() override;
    virtual void turnOn(boolean shareInformation = false) = 0;
    virtual void turnOff(boolean shareInformation = false) = 0;
    virtual void toggle(boolean shareInformation = false);
    virtual boolean getState() const;
    virtual void lock();
    virtual void unLock();
    virtual boolean isLocked() const;

protected:
    Display &m_display;
    boolean m_state;
    boolean m_locked;
};

#endif