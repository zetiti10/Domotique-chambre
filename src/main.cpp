/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du système de domotique.
 * @version 1.1
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Keypad.h>
#include <IRremote.hpp>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include <PN532_HSU.h>
#include <PN532.h>

// Autres fichiers du programme.
#include <main.hpp>
#include <pinDefinitions.hpp>
#include <keypadFunctions.hpp>
#include <display.hpp>
#include <devices.hpp>
#include <ESP.hpp>
#include <buzzer.hpp>
#include <bitmaps.hpp>

// Variables globales.
unsigned long IRRemoteCounter = 0;
unsigned long doorbellCounter = 0;
unsigned long airSensorsCounter = 0;
unsigned long cardCounter = 0;
unsigned long wardrobeCounter = 0;

boolean wardrobeState = false;
boolean debugMode = false;
float temperature = 0;
float humidity = 0;

// Création du clavier pour contrôler le système.
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

// Création de l'écran OLED qui affiche des informations sur le système.
Adafruit_SSD1306 display(128, 64, &Wire, -1);

// Création du récepteur infrarouge pour utiliser la télécommande du chromecast avec le système.
IRrecv IRSensor(PIN_IR_SENSOR);
decode_results result;

// Création du capteur de l'air (température et humidité).
DHT_Unified airSensor(PIN_AIR_SENSOR, DHT22);

// Mise en place de la communication avec le lecteur NFC.
PN532_HSU pn532hsu(Serial2);
PN532 nfcReader(pn532hsu);

void setup()
{
  // Lancement mode de résolution des problèmes.
  Serial.begin(115200);
  if (Serial)
  {
    debugMode = true;
    Serial.println("[INFO] [SETUP] Mode de résolution de problèmes activé.");
    Serial.println("[INFO] [SETUP] Début de l'initialisation du système.");
  }

  // Compteur permettant d'afficher le nombre d'erreurs à l'écran.
  int errorCounter = 0;

  // Définition des modes des broches des capteurs.
  pinMode(PIN_WARDROBE_DOOR_SENSOR, INPUT_PULLUP);
  pinMode(PIN_DOORBELL_BUTTON, INPUT);
  pinMode(PIN_BEDROOM_DOOR_SENSOR, INPUT);
  pinMode(PIN_MOTION_SENSOR, INPUT);
  pinMode(PIN_MICROPHONE, INPUT);
  pinMode(PIN_LIGHT_SENSOR, INPUT);

  // Définition des modes des broches des actionneurs.
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_SCREEN_SERVO, OUTPUT);
  pinMode(PIN_DOOR_LED, OUTPUT);
  pinMode(PIN_TRAY_MOTOR_SPEED, OUTPUT);
  pinMode(PIN_WARDROBE_LIGHTS_RELAY, OUTPUT);
  pinMode(PIN_ALARM_RELAY, OUTPUT);
  pinMode(PIN_DISCO_RELAY, OUTPUT);
  pinMode(PIN_LED_CUBE_RELAY, OUTPUT);
  pinMode(PIN_RELAY_6, OUTPUT);
  pinMode(PIN_STREET_RELAY, OUTPUT);
  pinMode(PIN_DESK_LIGHT_RELAY, OUTPUT);
  pinMode(PIN_RELAY_7, OUTPUT);
  pinMode(PIN_MOTOR_TRAY_1, OUTPUT);
  pinMode(PIN_MOTOR_TRAY_2, OUTPUT);

  if (debugMode)
    Serial.println("[INFO] [SETUP] Broches de l'Arduino initialisése.");

  // Communication avec l'ESP8266-01.
  Serial1.begin(9600);
  if (!Serial1) // Ne fonctionne pas. Il faudra trouver autre chose.
  {
    errorCounter++;

    if (debugMode)
      Serial.println("[ERREUR] [SETUP] La communication avec l'ESP8266-01 n'a pas pu être établie. La connexion à Home Assistant n'est pas effectuée.");
  }

  IrSender.begin(PIN_IR_LED);
  IRSensor.enableIRIn();

  keypad.begin();

  // Communication avec le capteur de température et d'humidité.
  airSensor.begin();
  sensors_event_t event;
  airSensor.temperature().getEvent(&event);
  if (!isnan(event.temperature))
  {
    temperature = event.temperature;

    airSensor.humidity().getEvent(&event);
    if (!isnan(event.relative_humidity))
      humidity = event.relative_humidity;
  }

  else
  {
    errorCounter++;

    if (debugMode)
      Serial.println("[ERREUR] [SETUP] La communication avec le capteur de température et d'humidité n'a pas pu être établie. Il est possible que le boîtier des capteurs ne soit pas opérationnel.");
  }

  // Communication avec le lecteur NFC.
  nfcReader.begin();
  nfcReader.SAMConfig();
  uint32_t versiondata = nfcReader.getFirmwareVersion();
  if (!versiondata)
  {
    errorCounter++;

    if (debugMode)
      Serial.println("[ERREUR] [SETUP] La communication avec le lecteur NFC n'a pas pu être établie. Il est possible que le boîtier de la porte ne soit pas opérationnel.");
  }

  if (debugMode)
    Serial.println("[INFO] [SETUP] Les communications avec les divers périphériques ont été établies.");

  // Récupération des informations stockées dans la mémoire persistante.
  alarmBuzzerState = EEPROM.read(ALARM_BUZZER_STATE_STORAGE_LOCATION);
  volume = EEPROM.read(VOLUME_STORAGE_LOCATION);
  multicolorSpeed = EEPROM.read(MULTICOLOR_ANIMATION_SPEED_STORAGE_LOCATION);
  soundReactSensibility = EEPROM.read(SOUND_REACT_ANIMATION_SENSIBILITY_STORAGE_LOCATION);

  if (debugMode)
    Serial.println("[INFO] [SETUP] Les valeurs stockées dans la mémoire persistante ont été récupérées.");

  // Démarrage de l'écran OLED.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
  {
    errorCounter++;

    if (debugMode)
      Serial.println("[ERREUR] [SETUP] La communication avec l'écran n'a pas pu être établie. Aucun affichage ne sera effectué.");
  }

  else
  {
    display.clearDisplay();
    display.cp437(true);
    display.setTextColor(WHITE);

    if (errorCounter == 0)
    {
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.println("Systeme demarre sans erreur.");
      display.display();
      ScreenCurrentOnTime = millis();
    }

    else
    {
      display.drawBitmap(0, 0, errorBitmap, 128, 64, WHITE);
      display.setTextSize(1);
      display.setCursor(24, 52);
      display.print(errorCounter);
      display.print(" erreurs detectee(s).");
      display.display();
      ScreenCurrentOnTime = millis();
      noSound();
    }

    if (debugMode)
      Serial.println("[INFO] [SETUP] La communication avec l'écran a été établie.");
  }

  if (debugMode)
  {
    float elapsedTime = millis() / 1000;
    Serial.print("[INFO] [SETUP] L'initialisation du système de domotique a été effectuée en ");
    Serial.print(elapsedTime);
    Serial.print(" seconde(s) et a détecté ");
    Serial.print(errorCounter);
    Serial.println(" erreur(s).");
  }
}

void loop()
{
  // Lecture de la température et de l'humidité toutes les 10 secondes.
  if ((millis() - airSensorsCounter) >= 10000)
  {
    sensors_event_t event;
    airSensor.temperature().getEvent(&event);
    if (!isnan(event.temperature))
    {
      temperature = event.temperature;

      airSensor.humidity().getEvent(&event);
      if (!isnan(event.relative_humidity))
        humidity = event.relative_humidity;
    }

    else if (debugMode)
      Serial.println("[ERREUR] [LOOP] Une erreur est survenue lors de la communication avec le capteur de température et de d'humidité. Il est possible que le boîtier des capteurs ne soit pas opérationnel.");

    airSensorsCounter = millis();
  }

  // Gestion de l'armoire. D'éviter un problème dû au INPUT_PULLUP, l'armoire ne s'allume qu'après un certain temps.
  boolean wardrobeDoorState = digitalRead(PIN_WARDROBE_DOOR_SENSOR);
  if (wardrobeDoorState == HIGH && wardrobeState == true)
  {
    digitalWrite(PIN_WARDROBE_LIGHTS_RELAY, LOW);
    displayDeviceState(false);
    wardrobeState = false;
  }

  else if (wardrobeDoorState == LOW && wardrobeState == false)
  {
    if (wardrobeCounter == 0)
      wardrobeCounter = millis();

    else if ((millis() - wardrobeCounter) >= 50)
    {
      digitalWrite(PIN_WARDROBE_LIGHTS_RELAY, HIGH);
      displayDeviceState(true);
      wardrobeState = true;
    }
  }

  else if (wardrobeDoorState == HIGH && wardrobeCounter != 0)
    wardrobeCounter = 0;

  // Gestion de la sonnette.
  if (digitalRead(PIN_DOORBELL_BUTTON) == HIGH && alarmState == false && (millis() - doorbellCounter) >= 250)
  {
    displayBell();
    digitalWrite(PIN_DOOR_LED, HIGH);
    doorbellMusic();
    digitalWrite(PIN_DOOR_LED, LOW);
    doorbellCounter = millis();
  }

  // Gestion de l'alarme.
  if (cardToStoreState == true)
  {
    uint8_t uid[] = {0, 0, 0, 0, 0, 0, 0};
    uint8_t uidLength;

    if (nfcReader.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100))
    {
      if (uidLength == 4)
      {
        uint8_t validUid[] = {uid[0], uid[1], uid[2], uid[3]};

        storeCard(validUid);
      }

      cardToStoreState = false;
    }

    cardCounter = millis();
  }

  else
    alarmSheduler();

  // Gestion du mode des rubans de DEL multicolor.
  multicolorScheduler();

  // Gestion du mode des rubans de DEL son - réaction.
  soundReactScheduler();

  // Gestion de l'arret automatique de l'écran OLED lorsqu'un élement est affiché.
  displaySheduler();

  // Gestion du clavier.
  keypad.tick();
  while (keypad.available())
  {
    keypadEvent e = keypad.read();

    if (e.bit.EVENT == KEY_JUST_PRESSED)
      pressedKeypadTouchTime = millis();

    else if (e.bit.EVENT == KEY_JUST_RELEASED)
    {

      boolean longPress = false;

      if ((millis() - pressedKeypadTouchTime) >= 250)
        longPress = true;

      keypadButtonPressed(e.bit.KEY, longPress);
    }
  }

  if ((keypadSubMenuTimer != 0) && ((millis() - keypadSubMenuTimer) >= 60000))
  {
    keypadSubMenuTimer = 0;

    if (keypadMenu == ALARM_CODE_CONFIGURATION_SUBMENU)
      alarmCode = "b";

    else if (keypadMenu != LIGHTS_MENU)
    {
      keypadMenu = LIGHTS_MENU;
      displayKeypadMenu();
    }
  }

  // Gestion du récepteur infrarouge pour contrôler le système avec la télécommande Google Chromecast.
  if (IRSensor.decode())
  {
    if ((millis() - IRRemoteCounter) >= 250)
    {
      IRRemoteCounter = millis();

      switch (IRSensor.decodedIRData.decodedRawData)
      {

      case 4244768519: // ON/OFF.
        switchTV(TOGGLE, true);
        break;

      case 4261480199: // Source.
        if (LEDCubeState == LOW && multicolorState == false)
        {
          switchMulticolor(SWITCH_ON, false);
          switchLEDCube(SWITCH_ON, false);
          displayDeviceState(true);
        }

        else
        {
          switchMulticolor(SWITCH_OFF, false);
          switchLEDCube(SWITCH_OFF, false);
          displayDeviceState(false);
        }
        break;

      case 4161210119: // Vol+.
        volumeSono(INCREASE, true);
        break;

      case 4094363399: // Vol-.
        volumeSono(DECREASE, true);
        break;

      case 4027516679: // Mute.
        volumeSono(TOGGLE_MUTE, true);
        break;
      }
    }

    IRSensor.resume();
  }

  // Gestion des informations reçues par l'ESP8266-01.
  /*if (ESP8266.available())
  {
    receivedData();
  }*/
}