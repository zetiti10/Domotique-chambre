#ifndef DEVICE_DEFINITIONS
#define DEVICE_DEFINITIONS

#include <Arduino.h>

class Device
{
public:
    Device(String friendlyName);
    virtual String getFriendlyName() const;

protected:
    String m_friendlyName;
};

#endif