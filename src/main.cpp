/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>
#include <IRremote.hpp> // Ne pas enlever.

// Autres fichiers du programme.
#include "pinDefinitions.hpp"
#include "deviceID.hpp"
#include "EEPROM.hpp"

#include "device/device.hpp"
#include "device/interface/display.hpp"
#include "device/interface/buzzer.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/output/output.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/alarm.hpp"
#include "device/output/tray.hpp"
#include "device/output/television.hpp"
#include "device/output/connectedOutput.hpp"
#include "device/input/input.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/analogInput.hpp"
#include "device/input/airSensor.hpp"
#include "device/input/IRSensor.hpp"

// Instanciation des périphériques du système.
// Interfaces.
Display display("Écran", ID_DISPLAY);
Buzzer buzzer("Buzzer", ID_BUZZER, PIN_BUZZER);
HomeAssistant HomeAssistantConnection("Connexion à HA", ID_HA, Serial1, display);

// Périphériques de sortie.
Tray tray("Plateau", ID_TRAY, display, HomeAssistantConnection, PIN_MOTOR_TRAY_1, PIN_MOTOR_TRAY_2, PIN_TRAY_MOTOR_SPEED);
BinaryOutput LEDCube("Cube de DEL", ID_LED_CUBE, display, HomeAssistantConnection, PIN_LED_CUBE_RELAY);
BinaryOutput disco("Lampes discothèque", ID_DISCO, display, HomeAssistantConnection, PIN_DISCO_RELAY);
BinaryOutput beacon("Gyrophare", ID_BEACON, display, HomeAssistantConnection, PIN_BEACON_RELAY);
BinaryOutput wardrobeLights("Lumière armoire", ID_WARDROBE_LIGHTS, display, HomeAssistantConnection, PIN_WARDROBE_LIGHTS_RELAY);
BinaryOutput street("Maquette de rue", ID_STREET, display, HomeAssistantConnection, PIN_STREET_RELAY);
BinaryOutput deskLight("Lampe du bureau", ID_DESK_LIGHT, display, HomeAssistantConnection, PIN_DESK_LIGHT_RELAY);
BinaryOutput doorLED("DEL de la porte", ID_DOOR_LED, display, HomeAssistantConnection, PIN_DOOR_LED);
RGBLEDStrip LEDStrip("Ruban de DEL", ID_LED_STRIP, display, HomeAssistantConnection, PIN_RED_LED, PIN_GREEN_LED, PIN_BLUE_LED);
Alarm alarm("Alarme", ID_ALARM, display, HomeAssistantConnection, Serial2, doorLED, beacon, LEDStrip, Serial3, buzzer, PIN_ALARM_RELAY, EEPROM.read(EEPROM_ALARM_BUZZER_STATE));
Television television("Télévision", ID_TELEVISION, display, HomeAssistantConnection, PIN_SCREEN_SERVO, PIN_IR_LED, EEPROM.read(EEPROM_VOLUME));

// Périphériques de sortie à distance.
ConnectedOutput mainLights("Lumières du plafond", ID_MAIN_LIGHTS, display, HomeAssistantConnection);
ConnectedTemperatureVariableLight sofaLight("Lampe du canapé", ID_SOFA_LIGHT, display, HomeAssistantConnection, 2000, 6000); // Changer les températures.
ConnectedColorVariableLight bedLight("Lampe de chevet", ID_BED_LIGHT, display, HomeAssistantConnection, 2000, 5000); // Changer les températures.
ConnectedOutput cameraLight("Lumière de la caméra", ID_CAMERA_LIGHT, display, HomeAssistantConnection);

// Création d'une liste contenant des références vers tous les actionneurs.
Output *outputList[] = {&tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television, &mainLights, &sofaLight, &bedLight, &cameraLight};
int outputsNumber = 15;

// Périphériques d'entrée.
WardrobeDoorSensor wardrobeDoorSensor("Capteur des portes de l'armoire", ID_WARDROBE_DOOR_SENSOR, HomeAssistantConnection, PIN_WARDROBE_DOOR_SENSOR, true, true, wardrobeLights);
DoorSensor doorSensor("Capteur de la porte de la chambre", ID_DOOR_SENSOR, HomeAssistantConnection, PIN_BEDROOM_DOOR_SENSOR, false, false, alarm);
BinaryInput presenceSensor("Capteur de présence", ID_PRESENCE_SENSOR, HomeAssistantConnection, PIN_MOTION_SENSOR, false, false);
Doorbell doorbell("Sonnette", ID_DOORBELL, HomeAssistantConnection, PIN_DOORBELL_BUTTON, false, false, display, buzzer);
AnalogInput lightSensor("Capteur de luminosité", ID_LIGHT_SENSOR, HomeAssistantConnection, PIN_LIGHT_SENSOR);
AnalogInput microphone("Microphone", ID_MICROPHONE, HomeAssistantConnection, PIN_MICROPHONE);
AirSensor airSensor("Capteur de l'air", ID_AIR_SENSOR, HomeAssistantConnection, PIN_AIR_SENSOR);
IRSensor iRSensor("Capteur infrarouge", ID_IR_SENSOR, HomeAssistantConnection, PIN_IR_SENSOR);

// Création d'une liste contenant des références vers tous les capteurs.
Input *inputList[] = {&wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &lightSensor, &microphone, &airSensor, &iRSensor};
int inputsNumber = 9;

// Modes du ruban de DEL.
ColorMode colorMode("Mode couleur unique", ID_COLOR_MODE, LEDStrip, HomeAssistantConnection);
RainbowMode rainbowMode("Mode arc-en-ciel", ID_RAINBOW_MODE, LEDStrip, EEPROM.read(EEPROM_RAINBOW_ANIMATION_SPEED));
// Mode son-réaction.

// Création d'une liste contenant des références vers tous les périphériques du système.
Device *deviceList[] = {&display, &buzzer, &HomeAssistantConnection, &tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television, &mainLights, &sofaLight, &bedLight, &cameraLight, &wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &microphone, &airSensor, &iRSensor};
int devicesNumber = 26;

// Liste des périphériques connectés à Home Assistant.
Output *HADeviceList[] = {&tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &LEDStrip, &alarm, &television};
int HADevicesNumber = 10;

// Liste des périphériques importés de Home Assistant.
Output *HARemoteDeviceList[] = {&mainLights, &sofaLight, &bedLight, &cameraLight};
int HARemoteDevicesNumber = 4;

// Initialisation du système.
void setup()
{
    // Démarrage de la communication avec l'ordinateur.
    Serial.begin(115200);

    // Définition des périphériques utilisés dans la connextion à Home Assistant.
    HomeAssistantConnection.setDevices(HADeviceList, HADevicesNumber, HARemoteDeviceList, HADevicesNumber, colorMode, rainbowMode);

    // Initialisation de tous les périphériques de la liste.
    for (int i = 0; i < devicesNumber; i++)
        deviceList[i]->setup();

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