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
    virtual void turnOn(bool shareInformation = false) = 0;
    virtual void turnOff(bool shareInformation = false) = 0;
    virtual void toggle(bool shareInformation = false);
    virtual bool getState() const;
    virtual void lock();
    virtual void unLock();
    virtual bool isLocked() const;

protected:
    Display &m_display;
    bool m_state;
    bool m_locked;
};

#endif