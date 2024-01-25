/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "pinDefinitions.hpp"
#include "device/device.hpp"
#include "device/display.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/tray.hpp"
#include "device/input/airSensor.hpp"
#include "device/input/analogInput.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/IRSensor.hpp"
#include "logger.hpp"

// Instanciation des périphériques du système.
Display display("Ecran");
Tray tray("Plateau", PIN_MOTOR_TRAY_1, PIN_MOTOR_TRAY_2);
BinaryOutput LEDCube("Cube de DEL", PIN_LED_CUBE_RELAY);
BinaryOutput disco("Lampes discothèque", PIN_DISCO_RELAY);
BinaryOutput beacon("Gyrophare", PIN_BEACON_RELAY);
BinaryOutput wardrobeLights("Lumière armoire", PIN_WARDROBE_LIGHTS_RELAY);
BinaryOutput street("Maquette de rue", PIN_STREET_RELAY);
BinaryOutput deskLight("Lampe du bureau", PIN_DESK_LIGHT_RELAY);
WardrobeDoorSensor wardrobeDoorSensor("Capteur des portes de l'armoire", PIN_WARDROBE_DOOR_SENSOR, true, true, wardrobeLights);
// DoorSensor.
BinaryInput presenceSensor("Capteur de présence", PIN_MOTION_SENSOR, false, false);
Doorbell doorbell("Sonnette", PIN_DOORBELL_BUTTON, false, false);
AnalogInput lightSensor("Capteur de luminosité", PIN_LIGHT_SENSOR);
AirSensor airSensor("Capteur de l'air", PIN_AIR_SENSOR);
IRSensor iRSensor("Capteur infrarouge", PIN_IR_SENSOR);

// Création d'une liste contenant des références vers tous les périphériques du système.
Device *deviceList[] = {&display, &tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &wardrobeDoorSensor, &presenceSensor, &doorbell, &lightSensor, &airSensor, &iRSensor};
int devicesNumber = 14;

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