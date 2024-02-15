#ifndef CONNECTED_LIGHT_DEFINITIONS
#define CONNECTED_LIGHT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

// Classe représentant une lampe contrôlée depuis le réseau.
class ConnectedLight
{
public:
    ConnectedLight(String friendlyName);
    virtual void setup();
    virtual void turnOn(bool shareInformation = false);
    virtual void turnOff(bool shareInformation = false);

protected:
    
};

#endif