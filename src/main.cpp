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
// Nécessaire, ne pas enlever.
#include <IRremote.hpp>

// Autres fichiers du programme.
#include "pinDefinitions.hpp"
#include "deviceID.hpp"
#include "EEPROM.hpp"
#include "device/interface/display.hpp"
#include "device/interface/buzzer.hpp"
#include "device/interface/keypad.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/output/tray.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/alarm.hpp"
#include "device/output/television.hpp"
#include "device/output/connectedOutput.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/analogInput.hpp"
#include "device/input/airSensor.hpp"
#include "device/input/IRSensor.hpp"
#include "musics.hpp"

bool systemToShutdown = false;

// Initialisation du système.
void setup()
{
    // Configuration du clavier.
    const byte KEYPAD_ROWS = 4;
    const byte KEYPAD_COLS = 4;
    char keypadKeys[KEYPAD_ROWS][KEYPAD_COLS] PROGMEM = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}};
    byte keypadRowPins[KEYPAD_ROWS] PROGMEM = {31, 33, 35, 37};
    byte keypadColPins[KEYPAD_COLS] PROGMEM = {30, 32, 34, 36};

    // Instanciation des périphériques du système.
    // Interfaces.
    Display display(F("Écran"), ID_DISPLAY);
    Buzzer buzzer(F("Buzzer"), ID_BUZZER, PIN_BUZZER);
    HomeAssistant HomeAssistantConnection(F("Connexion à HA"), ID_HA, Serial1, display);
    Keypad keypad(F("Clavier de contrôle"), ID_KEYPAD, display, makeKeymap(keypadKeys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

    // Périphériques de sortie.
    Tray tray(F("Plateau"), ID_TRAY, HomeAssistantConnection, display, PIN_MOTOR_TRAY_1, PIN_MOTOR_TRAY_2, PIN_TRAY_MOTOR_SPEED);
    BinaryOutput LEDCube(F("Cube de DEL"), ID_LED_CUBE, HomeAssistantConnection, display, PIN_LED_CUBE_RELAY);
    BinaryOutput disco(F("Lampes discothèque"), ID_DISCO, HomeAssistantConnection, display, PIN_DISCO_RELAY);
    BinaryOutput beacon(F("Gyrophare"), ID_BEACON, HomeAssistantConnection, display, PIN_BEACON_RELAY);
    BinaryOutput wardrobeLights(F("Lumière armoire"), ID_WARDROBE_LIGHTS, HomeAssistantConnection, display, PIN_WARDROBE_LIGHTS_RELAY);
    BinaryOutput street(F("Maquette de rue"), ID_STREET, HomeAssistantConnection, display, PIN_STREET_RELAY);
    BinaryOutput deskLight(F("Lampe du bureau"), ID_DESK_LIGHT, HomeAssistantConnection, display, PIN_DESK_LIGHT_RELAY);
    BinaryOutput doorLED(F("DEL de la porte"), ID_DOOR_LED, HomeAssistantConnection, display, PIN_DOOR_LED);
    RGBLEDStrip LEDStrip(F("Ruban de DEL"), ID_LED_STRIP, HomeAssistantConnection, display, PIN_RED_LED, PIN_GREEN_LED, PIN_BLUE_LED);
    AlarmMode alarmMode(F("Mode alarme"), 3, LEDStrip);
    Alarm alarm(F("Alarme"), ID_ALARM, HomeAssistantConnection, display, Serial2, doorLED, beacon, LEDStrip, alarmMode, Serial3, buzzer, PIN_ALARM_RELAY, EEPROM_ALARM_BUZZER_STATE, EEPROM_STORED_CARD_COUNTER, EEPROM_CARDS);
    MusicsAnimationsMode musicsAnimationsMode(F("Mode musique animations"), 4, LEDStrip);
    Television television(F("Télévision"), ID_TELEVISION, HomeAssistantConnection, display, PIN_SCREEN_SERVO, PIN_IR_LED, EEPROM.read(EEPROM_VOLUME), musicsAnimationsMode);

    // Périphériques de sortie à distance.
    ConnectedOutput mainLights(F("Lumières du plafond"), ID_MAIN_LIGHTS, HomeAssistantConnection, display);
    ConnectedTemperatureVariableLight sofaLight(F("Lampe du canapé"), ID_SOFA_LIGHT, HomeAssistantConnection, display, 2000, 5000);
    ConnectedColorVariableLight bedLight(F("Lampe de chevet"), ID_BED_LIGHT, HomeAssistantConnection, display, 2202, 6535);
    ConnectedOutput cameraLight(F("DEL de la caméra"), ID_CAMERA_LIGHT, HomeAssistantConnection, display);

    // Liste des périphériques contrôlés par la télécommande Google.
    Output *remoteDeviceList[] = {&LEDStrip, &LEDCube};
    int remoteDevicesNumber = 2;

    // Périphériques d'entrée.
    WardrobeDoorSensor wardrobeDoorSensor(F("Armoire"), ID_WARDROBE_DOOR_SENSOR, HomeAssistantConnection, PIN_WARDROBE_DOOR_SENSOR, true, true, wardrobeLights);
    DoorSensor doorSensor(F("Porte"), ID_DOOR_SENSOR, HomeAssistantConnection, PIN_BEDROOM_DOOR_SENSOR, false, false, alarm);
    BinaryInput presenceSensor(F("Présence"), ID_PRESENCE_SENSOR, HomeAssistantConnection, PIN_MOTION_SENSOR, false, false);
    Doorbell doorbell(F("Sonnette"), ID_DOORBELL, HomeAssistantConnection, PIN_DOORBELL_BUTTON, false, false, display, buzzer);
    AnalogInput lightSensor(F("Luminosité"), ID_LIGHT_SENSOR, HomeAssistantConnection, PIN_LIGHT_SENSOR, false);
    AnalogInput microphone(F("Microphone"), ID_MICROPHONE, HomeAssistantConnection, PIN_MICROPHONE, false);
    television.setMicrophone(microphone);
    AirSensor airSensor(F("Air"), ID_AIR_SENSOR, HomeAssistantConnection, PIN_AIR_SENSOR);
    IRSensor iRSensor(F("Capteur infrarouge"), ID_IR_SENSOR, HomeAssistantConnection, PIN_IR_SENSOR, television, remoteDeviceList, remoteDevicesNumber);

    // Modes du ruban de DEL.
    ColorMode colorMode(F("Mode couleur unique"), ID_COLOR_MODE, LEDStrip, HomeAssistantConnection);
    RainbowMode rainbowMode(F("Mode arc-en-ciel"), ID_RAINBOW_MODE, LEDStrip, EEPROM_RAINBOW_ANIMATION_SPEED);
    SoundreactMode soundreactMode(F("Mode son-réaction"), ID_SOUND_REACT_MODE, LEDStrip, microphone, EEPROM_SOUND_REACT_ANIMATION_SENSITIVITY);
    LEDStrip.setMode(&colorMode);

    // Liste des musiques.
    const static Music *const musicList[] PROGMEM = {&worldsSmallestViolinMusic};
    int musicsNumber = 1;

    // Création d'une liste contenant des références vers tous les actionneurs utilisés par le système de musique animée.
    Output *outputList[] = {&tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &mainLights, &sofaLight, &bedLight, &cameraLight};
    int outputsNumber = 14;

    // Création d'une liste contenant des références vers tous les capteurs.
    Input *inputList[] = {&wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &microphone, &airSensor, &iRSensor};
    int inputsNumber = 8;

    // Création d'une liste contenant des références vers tous les périphériques du système.
    Device *deviceList[] = {&display, &buzzer, &HomeAssistantConnection, &keypad, &tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television, &mainLights, &sofaLight, &bedLight, &cameraLight, &wardrobeDoorSensor, &doorSensor, &presenceSensor, &doorbell, &lightSensor, &microphone, &airSensor, &iRSensor};
    int devicesNumber = 27;

    // Liste des périphériques connectés à Home Assistant.
    Output *HADeviceList[] = {&tray, &LEDCube, &disco, &beacon, &wardrobeLights, &street, &deskLight, &doorLED, &LEDStrip, &alarm, &television};
    int HADevicesNumber = 11;

    // Liste des périphériques importés de Home Assistant.
    Output *HARemoteDeviceList[] = {&mainLights, &sofaLight, &bedLight, &cameraLight};
    int HARemoteDevicesNumber = 4;

    // Liste des périphériques pour le clavier de contrôle.
    Output *keypadDeviceList[] = {&tray, &LEDCube, &disco, &beacon, &street};
    int keypadDevicesNumber = 5;
    Output *keypadLightList[] = {&wardrobeLights, &deskLight, &doorLED, &cameraLight, &mainLights};
    int keypadLightsNumber = 5;
    RGBLEDStrip *keypadStripList[] = {&LEDStrip};
    ColorMode *keypadStripColorModeList[] = {&colorMode};
    RainbowMode *keypadStripRainbowModeList[] = {&rainbowMode};
    SoundreactMode *keypadStripSoundreactModeList[] = {&soundreactMode};
    AlarmMode *keypadStripAlarmModeList[] = {&alarmMode};
    int keypadStripsNumber = 1;
    ConnectedTemperatureVariableLight *keypadConnectedTemperatureVariableLightList[] = {&sofaLight};
    int keypadConnectedTemperatureVariableLightsNumber = 1;
    ConnectedColorVariableLight *keypadConnectedColorVariableLightList[] = {&bedLight};
    int keypadConnectedColorVariableLightsNumber = 1;
    Television *keypadTelevisionList[] = {&television};
    int keypadTelevisionsNumber = 1;
    Alarm *keypadAlarmList[] = {&alarm};
    int keypadAlarmsNumber = 1;
    BinaryInput *keypadBinaryInputList[] = {&doorSensor, &presenceSensor, &doorbell};
    int keypadBinaryInputsNumber = 3;
    AnalogInput *keypadAnalogInputList[] = {&lightSensor};
    int keypadAnalogInputsNumber = 1;
    AirSensor *keypadAirSensorList[] = {&airSensor};
    int keypadAirSensorsNumber = 1;
    WardrobeDoorSensor *keypadWardrobeDoorSensorList[] = {&wardrobeDoorSensor};
    int keypadWardrobeDoorSensorsList = 1;

    // Un petit délai de sécurité.
    delay(1000);

    // Démarrage de la communication avec l'ordinateur.
    Serial.begin(115200);

    // Génère une SEED pour la fonction random.
    randomSeed(analogRead(PIN_RANDOM_SEED_GENERATOR));

    // Définition des périphériques utilisés dans la connextion à Home Assistant.
    HomeAssistantConnection.setDevices(HADeviceList, HADevicesNumber, inputList, inputsNumber, HARemoteDeviceList, HARemoteDevicesNumber, colorMode, rainbowMode, soundreactMode, alarmMode);

    // Définition des périphériques contrôlables depuis le clavier de contrôle.
    keypad.setDevices(keypadDeviceList,
                      keypadDevicesNumber,
                      keypadLightList,
                      keypadLightsNumber,
                      keypadStripList,
                      keypadStripColorModeList,
                      keypadStripRainbowModeList,
                      keypadStripSoundreactModeList,
                      keypadStripAlarmModeList,
                      keypadStripsNumber,
                      keypadConnectedTemperatureVariableLightList,
                      keypadConnectedTemperatureVariableLightsNumber,
                      keypadConnectedColorVariableLightList,
                      keypadConnectedColorVariableLightsNumber,
                      keypadTelevisionList,
                      keypadTelevisionsNumber,
                      keypadAlarmList,
                      keypadAlarmsNumber,
                      keypadBinaryInputList,
                      keypadBinaryInputsNumber,
                      keypadAnalogInputList,
                      keypadAnalogInputsNumber,
                      keypadAirSensorList,
                      keypadAirSensorsNumber,
                      keypadWardrobeDoorSensorList,
                      keypadWardrobeDoorSensorsList);

    // Définitions des périphériques et musiques pour la musique sur la télévision.
    television.setMusicsList(musicList, musicsNumber);
    television.setMusicDevices(outputList, outputsNumber);

    // Initialisation de tous les périphériques de la liste.
    for (int i = 0; i < devicesNumber; i++)
        deviceList[i]->setup();

    // Compte rendu des informations de l'initialisation du système.
    display.displayUnavailableDevices(deviceList, devicesNumber);

    // Boucle d'exécution des tâches du système.
    while (1)
    {
        // Exécutuon des tâches périodiques des capteurs.
        for (int i = 0; i < inputsNumber; i++)
            inputList[i]->loop();

        // Exécution des tâches périodiques des autres périphériques.
        display.loop();
        alarm.loop();
        LEDStrip.loop();
        HomeAssistantConnection.loop();
        keypad.loop();
        television.loop();

        if (systemToShutdown)
            break;
    }

    // Arrêt de tous les périphériques de la liste.
    for (int i = 0; i < devicesNumber; i++)
        deviceList[i]->shutdown();
}

void loop()
{
}