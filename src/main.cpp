/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "pinDefinition.hpp"
#include "devices/binaryDevice.hpp"
#include "logger.hpp"

// Instanciation des périphériques du système.
BinaryDevice LEDCube("Cube de DEL", PIN_LED_CUBE_RELAY);
BinaryDevice disco("Lampes discothèque", PIN_DISCO_RELAY);
BinaryDevice beacon("Gyrophare", PIN_BEACON_RELAY);
BinaryDevice street("Maquette de rue", PIN_STREET_RELAY);

Device* deviceList[] = {&LEDCube, &disco, &beacon, &street};
int devicesNumber = 4;

void setup()
{
    Serial.begin(115200);

    if(Serial)
        loggerEnabled = true;

    sendLogMessage(INFO, "Journalisation activée.");
    sendLogMessage(INFO, "Démarrage du système...");

    for (int i = 0; i < devicesNumber; i ++)
    {
        deviceList[i]->setup();
    }

    sendLogMessage(INFO, "Initialisation terminée.");
}

void loop()
{

}