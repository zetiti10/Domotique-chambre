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
#include <missileLauncher.hpp>

// Autres fichiers du programme.
#include "pinDefinitions.hpp"
#include "device/device.hpp"
#include "device/display.hpp"
#include "device/buzzer.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/tray.hpp"
#include "device/output/alarm.hpp"
#include "device/output/television.hpp"
#include "device/input/airSensor.hpp"
#include "device/input/analogInput.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/IRSensor.hpp"
#include "logger.hpp"

MissileLauncher missileLauncher(&Serial3);

// Instanciation des périphériques du système.
Display display("Écran");
Buzzer buzzer("Buzzer", PIN_BUZZER);
Tray tray("Plateau", display, PIN_MOTOR_TRAY_1, PIN_MOTOR_TRAY_2, PIN_TRAY_MOTOR_SPEED);
BinaryOutput LEDCube("Cube de DEL", display, PIN_LED_CUBE_RELAY);
BinaryOutput disco("Lampes discothèque", display, PIN_DISCO_RELAY);
BinaryOutput beacon("Gyrophare", display, PIN_BEACON_RELAY);
BinaryOutput wardrobeLights("Lumière armoire", display, PIN_WARDROBE_LIGHTS_RELAY);
BinaryOutput street("Maquette de rue", display, PIN_STREET_RELAY);
BinaryOutput deskLight("Lampe du bureau", display, PIN_DESK_LIGHT_RELAY);
BinaryOutput doorLED("DEL de la porte", display, PIN_DOOR_LED);
RGBLEDStrip LEDStrip("Ruban de DEL", display, PIN_RED_LED, PIN_GREEN_LED, PIN_BLUE_LED);
Alarm alarm("Alarme", display, Serial2, doorLED, beacon, LEDStrip, missileLauncher, buzzer, PIN_ALARM_RELAY, true);
Television television("Télévision", display, PIN_SCREEN_SERVO, PIN_IR_LED);

WardrobeDoorSensor wardrobeDoorSensor("Capteur des portes de l'armoire", PIN_WARDROBE_DOOR_SENSOR, true, true, wardrobeLights);
DoorSensor doorSensor("Capteur de la porte de la chambre", PIN_BEDROOM_DOOR_SENSOR, false, false, alarm);
BinaryInput presenceSensor("Capteur de présence", PIN_MOTION_SENSOR, false, false);
Doorbell doorbell("Sonnette", PIN_DOORBELL_BUTTON, false, false, display, buzzer);
AnalogInput lightSensor("Capteur de luminosité", PIN_LIGHT_SENSOR);
AnalogInput microphone("Microphone", PIN_MICROPHONE);
AirSensor airSensor("Capteur de l'air", PIN_AIR_SENSOR);
IRSensor iRSensor("Capteur infrarouge", PIN_IR_SENSOR);

ColorMode colorMode("Mode couleur unique", LEDStrip);
// Modes.

// Création d'une liste contenant des références vers tous les périphériques du système.
Device *deviceList[] = {&display, &buzzer, &tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television, &wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &microphone, &airSensor, &iRSensor};
int devicesNumber = 21;

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