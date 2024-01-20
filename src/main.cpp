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
#include "device/output/binaryOutput.hpp"
#include "logger.hpp"

// Instanciation des périphériques du système.
BinaryOutput LEDCube("Cube de DEL", PIN_LED_CUBE_RELAY);
BinaryOutput disco("Lampes discothèque", PIN_DISCO_RELAY);
BinaryOutput beacon("Gyrophare", PIN_BEACON_RELAY);
BinaryOutput street("Maquette de rue", PIN_STREET_RELAY);

Output *deviceList[] = {&LEDCube, &disco, &beacon, &street};
int devicesNumber = 4;

void setup()
{
    Serial.begin(115200);

    if (Serial)
        loggerEnabled = true;

    sendLogMessage(INFO, "Journalisation activée.");
    sendLogMessage(INFO, "Démarrage du système...");

    for (int i = 0; i < devicesNumber; i++)
    {
        deviceList[i]->setup();
    }

    sendLogMessage(INFO, "Initialisation terminée.");
}

void loop()
{
}