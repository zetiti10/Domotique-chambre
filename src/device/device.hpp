#ifndef DEVICE_DEFINITIONS
#define DEVICE_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Classe commune à tous les périphériques du système.
class Device
{
public:
    Device(const __FlashStringHelper* friendlyName, int ID);
    virtual void setup() = 0;
    virtual const __FlashStringHelper* getFriendlyName() const;
    virtual int getID() const;
    virtual bool getAvailability() const;

protected:
    const __FlashStringHelper* m_friendlyName;
    const int m_ID;
    bool m_operational;
};

#endif