#ifndef OUTPUT_DEFINITIONS
#define OUTPUT_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "../device.hpp"

class Output : public Device
{
public:
    Output(String friendlyName);
    virtual void setup() = 0;
    virtual void turnOn(boolean shareInformation = false) = 0;
    virtual void turnOff(boolean shareInformation = false) = 0;
    virtual void toggle(boolean shareInformation = false) = 0;
    virtual void setOperational();
    virtual void setUnavailable();
    virtual boolean getAvailability() const;
    virtual void lock();
    virtual void unLock();

protected:
    boolean m_state;
    boolean m_operational;
    boolean m_locked;
};

#endif