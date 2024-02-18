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
#include <Adafruit_Keypad.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include "pinDefinitions.hpp"
#include "device/device.hpp"
#include "device/interface/display.hpp"
#include "device/interface/buzzer.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/tray.hpp"
#include "device/output/alarm.hpp"
#include "device/output/television.hpp"
#include "device/input/airSensor.hpp"
#include "device/input/analogInput.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/IRSensor.hpp"
#include "EEPROM.hpp"

// Instanciation des périphériques du système.
// Interfaces.
Display display("Écran", 0);
Buzzer buzzer("Buzzer", 0, PIN_BUZZER);
HomeAssistant HomeAssistantConnection(Serial1);

// Périphériques de sortie.
Tray tray("Plateau", 1, display, PIN_MOTOR_TRAY_1, PIN_MOTOR_TRAY_2, PIN_TRAY_MOTOR_SPEED);
BinaryOutput LEDCube("Cube de DEL", 2, display, PIN_LED_CUBE_RELAY);
BinaryOutput disco("Lampes discothèque", 3, display, PIN_DISCO_RELAY);
BinaryOutput beacon("Gyrophare", 4, display, PIN_BEACON_RELAY);
BinaryOutput wardrobeLights("Lumière armoire", 5, display, PIN_WARDROBE_LIGHTS_RELAY);
BinaryOutput street("Maquette de rue", 6, display, PIN_STREET_RELAY);
BinaryOutput deskLight("Lampe du bureau", 7, display, PIN_DESK_LIGHT_RELAY);
BinaryOutput doorLED("DEL de la porte", 8, display, PIN_DOOR_LED);
RGBLEDStrip LEDStrip("Ruban de DEL", 9, display, PIN_RED_LED, PIN_GREEN_LED, PIN_BLUE_LED);
Alarm alarm("Alarme", 10, display, Serial2, doorLED, beacon, LEDStrip, Serial3, buzzer, PIN_ALARM_RELAY, EEPROM.read(EEPROM_ALARM_BUZZER_STATE));
Television television("Télévision", 11, display, PIN_SCREEN_SERVO, PIN_IR_LED, EEPROM.read(EEPROM_VOLUME));
// Lumières du plafond.
// Lampe canapé.
// Lampe de chevet.
// Caméra (lumière, redémarrer).

// Création d'une liste contenant des références vers tous les actionneurs.
Output *outputList[] = {&tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television};
int outputsNumber = 11;

// Périphériques d'entrée.
WardrobeDoorSensor wardrobeDoorSensor("Capteur des portes de l'armoire", 12, PIN_WARDROBE_DOOR_SENSOR, true, true, wardrobeLights);
DoorSensor doorSensor("Capteur de la porte de la chambre", 13, PIN_BEDROOM_DOOR_SENSOR, false, false, alarm);
BinaryInput presenceSensor("Capteur de présence", 14, PIN_MOTION_SENSOR, false, false);
Doorbell doorbell("Sonnette", 15, PIN_DOORBELL_BUTTON, false, false, display, buzzer);
AnalogInput lightSensor("Capteur de luminosité", 16, PIN_LIGHT_SENSOR);
AnalogInput microphone("Microphone", 17, PIN_MICROPHONE);
AirSensor airSensor("Capteur de l'air", 18, PIN_AIR_SENSOR);
IRSensor iRSensor("Capteur infrarouge", 19, PIN_IR_SENSOR);

// Création d'une liste contenant des références vers tous les capteurs.
Input *inputList[] = {&wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &lightSensor, &microphone, &airSensor, &iRSensor};
int inputsNumber = 9;

// Modes du ruban de DEL.
ColorMode colorMode("Mode couleur unique", LEDStrip);
RainbowMode rainbowMode("Mode arc-en-ciel", LEDStrip, EEPROM.read(EEPROM_RAINBOW_ANIMATION_SPEED));
// Mode son-réaction.

// Création d'une liste contenant des références vers tous les périphériques du système.
Device *deviceList[] = {&display, &buzzer, &tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television, &wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &microphone, &airSensor, &iRSensor};
int devicesNumber = 21;

// Initialisation du système.
void setup()
{
    // Démarrage de la communication avec l'ordinateur.
    Serial.begin(115200);

    // Initialisation de tous les périphériques de la liste, en comptant les erreurs.
    int errors = 0;

    for (int i = 0; i < devicesNumber; i++)
    {
        deviceList[i]->setup();

        if (!deviceList[i]->getAvailability())
            errors++;
    }

    HomeAssistantConnection.setup();

    // Compte rendu des informations de l'initialisation du système.
    display.displayUnavailableDevices(deviceList, devicesNumber);
}

// Boucle d'exécution des tâches du système.
void loop()
{
    // Exécutuon des tâches périodiques des capteurs.
    for (int i = 0; i < inputsNumber; i++)
        inputList[i]->loop();

    // Exécution des tâches périodiques des autres périphériques.
    display.loop();
    alarm.loop();
    LEDStrip.loop();
    HomeAssistantConnection.loop();
}