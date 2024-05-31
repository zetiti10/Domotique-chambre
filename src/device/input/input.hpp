#ifndef INPUT_DEFINITIONS
#define INPUT_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/device.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Classe mère à tous les capteurs du système de domotique.
class Input : public Device
{
public:
    Input(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection);
    virtual void setup() override;
    virtual void reportState() = 0;
    virtual void loop() = 0;

protected:
    HomeAssistant &m_connection;
};

#endif