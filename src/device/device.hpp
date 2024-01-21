#ifndef DEVICE_DEFINITIONS
#define DEVICE_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

class Device
{
public:
    Device(String friendlyName);
    virtual String getFriendlyName() const;
    virtual void setup() = 0;
    virtual boolean getAvailability() const;

protected:
    String m_friendlyName;
    boolean m_operational;
};

#endif