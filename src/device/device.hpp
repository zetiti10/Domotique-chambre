#ifndef DEVICE_DEFINITIONS
#define DEVICE_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

class Device
{
public:
    Device(String friendlyName);
    virtual void setup() = 0;
    virtual String getFriendlyName() const;
    virtual bool getAvailability() const;

protected:
    String m_friendlyName;
    bool m_operational;
};

#endif