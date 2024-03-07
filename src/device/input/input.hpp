#ifndef INPUT_DEFINITIONS
#define INPUT_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/device.hpp"
#include "device/interface/HomeAssistant.hpp"

// Classe mère à tous les capteurs du système de domotique.
class Input : public Device
{
public:
    Input(String friendlyName, int ID, HomeAssistant &connection);
    virtual void setup() override;
    virtual void loop() = 0;

protected:
    HomeAssistant &m_connection;
};

#endif