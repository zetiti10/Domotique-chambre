#ifndef DEVICE_DEFINITIONS
#define DEVICE_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

/// @brief Classe commune à tous les périphériques du système.
class Device
{
public:
    Device(const __FlashStringHelper *friendlyName, unsigned int ID);
    virtual void setup() = 0;
    virtual const __FlashStringHelper *getFriendlyName() const;
    virtual const unsigned int getID() const;
    virtual bool getAvailability() const;
    virtual void loop();
    virtual void shutdown();

protected:
    const __FlashStringHelper *m_friendlyName;
    const unsigned int m_ID;
    bool m_operational;
};

#endif