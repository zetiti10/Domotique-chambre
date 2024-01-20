#ifndef BINARY_DEVICE_DEFINITIONS
#define BINARY_DEVICE_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device.hpp"

class BinaryDevice : public Device
{
public:
    BinaryDevice(String friendlyName, int relayPin);
    virtual ~BinaryDevice();
    virtual void setup();
    virtual void turnOn(boolean shareInformation = false);
    virtual void turnOff(boolean shareInformation = false);
    virtual void toggle(boolean shareInformation = false);

private:
    String m_friendlyName;
    boolean m_state;
    boolean m_operational;
    boolean m_locked;
    int m_relayPin;
};

#endif