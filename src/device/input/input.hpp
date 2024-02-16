#ifndef INPUT_DEFINITIONS
#define INPUT_DEFINITIONS

// Autres fichiers du programme.
#include "../device.hpp"

// Classe mère à tous les capteurs du système de domotique.
class Input : public Device
{
public:
    Input(String friendlyName, int ID);
    virtual void loop() = 0;
};

#endif