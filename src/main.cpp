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
#include "pinDefinitions.hpp"
#include "device/device.hpp"
#include "device/display.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/tray.hpp"
#include "logger.hpp"

// Instanciation des périphériques du système.
Display display("Ecran");
Tray tray("Plateau", PIN_MOTOR_TRAY_1, PIN_MOTOR_TRAY_2);
BinaryOutput LEDCube("Cube de DEL", PIN_LED_CUBE_RELAY);
BinaryOutput disco("Lampes discothèque", PIN_DISCO_RELAY);
BinaryOutput beacon("Gyrophare", PIN_BEACON_RELAY);
BinaryOutput street("Maquette de rue", PIN_STREET_RELAY);

// Création d'une liste contenant des références vers tous les périphériques du système.
Device *deviceList[] = {&display, &tray, &LEDCube, &disco, &beacon, &street};
int devicesNumber = 6;

// Initialisation du système.
void setup()
{
    // Démarrage de la communication avec l'ordinateur.
    Serial.begin(115200);

    // Activation du mode journalisation si un ordinateur est connecté.
    if (Serial)
        loggerEnabled = true;

    sendLogMessage(INFO, "Journalisation activée.");
    sendLogMessage(INFO, "Démarrage du système...");

    // Initialisation de tous les périphériques de la liste.
    for (int i = 0; i < devicesNumber; i++)
        deviceList[i]->setup();

    sendLogMessage(INFO, "Initialisation terminée.");
}

// Boucle d'exécution des tâches du système.
void loop()
{
}