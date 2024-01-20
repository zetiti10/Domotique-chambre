#ifndef DEVICE_DEFINITIONS
#define DEVICE_DEFINITIONS

#include <Arduino.h>

class Device
{
public:
    Device();
    virtual ~Device();
    virtual String getFriendlyName();
    virtual void setup() = 0;
    virtual void turnOn(boolean shareInformation = false) = 0;
    virtual void turnOff(boolean shareInformation = false) = 0;
    virtual void toggle(boolean shareInformation = false) = 0;
    virtual void setOperational();
    virtual void setUnavailable();
    virtual boolean getAvailability();
    virtual void lock();
    virtual void unLock();

private:
    String m_friendlyName;
    boolean m_state;
    boolean m_operational;
    boolean m_locked;
};

#endif