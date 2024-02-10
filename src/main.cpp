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
RainbowMode rainbowMode("Mode arc-en-ciel", LEDStrip);
// Mode son-réaction.

// Création d'une liste contenant des références vers tous les périphériques du système.
Device *deviceList[] = {&display, &buzzer, &tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television, &wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &microphone, &airSensor, &iRSensor};
int devicesNumber = 21;

// Création d'une liste contenant des références vers tous les capteurs.
Input *inputList[] = {&wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &lightSensor, &microphone, &airSensor, &iRSensor};
int inputsNumber = 9;

// Création d'une liste contenant des références vers tous les actionneurs.
Output *outputList[] = {&tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television};
int outputsNumber = 11;

// PROVISOIRE : création du clavier pour contrôler le système.
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte keypadRowPins[KEYPAD_ROWS] = {31, 33, 35, 37};
byte keypadColPins[KEYPAD_COLS] = {30, 32, 34, 36};
Adafruit_Keypad keypad = Adafruit_Keypad(makeKeymap(keypadKeys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

int counter = 0;
unsigned long test = 0;

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

    // PROVISOIRE
    keypad.begin();
    test = millis();
}

// Boucle d'exécution des tâches du système.
void loop()
{
    for (int i = 0; i < inputsNumber; i++)
        inputList[i]->loop();
    
    keypad.tick();
    while (keypad.available())
    {
        keypadEvent e = keypad.read();

        if (e.bit.EVENT == KEY_JUST_PRESSED)
        {
            switch (e.bit.KEY)
            {
            case '1':
                tray.toggle(true);
                break;

            case '2':
                LEDCube.toggle(true);
                break;
            
            case '3':
                disco.toggle(true);
                break;
            
            case '4':
                beacon.toggle(true);
                break;
            
            case '5':
                street.toggle(true);
                break;
            
            case '6':
                deskLight.toggle(true);
                break;
            
            case '7':
                doorLED.toggle(true);
                break;
            
            case '8':
                rainbowMode.setAnimationSpeed(1);
                LEDStrip.setMode(rainbowMode);
                LEDStrip.toggle(true);
                colorMode.setColor(255, 255, 255);
                break;

            case '9':
                rainbowMode.setAnimationSpeed(100);
                break;
            
            default:
                break;
            }
        }
    }

    display.loop();
    alarm.loop();
    LEDStrip.loop();

    /*if ((millis() - 1000) >= test)
    {
        sendLogMessage(HIGHLIGHT, "Itérations / seconde : " + String(counter));
        counter = 0;
        test = millis();
    }

    else
        counter++;*/
}