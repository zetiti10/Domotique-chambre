/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du système de domotique.
 * @version 1.0
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>
#include <Keypad.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <IRremote.h>

// Autres fichiers du programme.
#include <pinDefinitions.hpp>
#include <keypadFunctions.hpp>
#include <screen.hpp>
#include <devices.hpp>
#include <alarm.hpp>
#include <LEDStrips.hpp>
#include <ESP.hpp>

// Variables globales.
boolean bedroomDoorState;
boolean doorbellToSend = false;
boolean doorbellMode;
boolean muteMode;
boolean muteModePreviousState;
boolean wardrobeDoorPreviousState;
boolean trayIsOpen;
float temperature;
int microValue;
int microSensibility;
int microDelay;
int IRRemoteCounter = 0;
int button9Timer = 0;
long powerSupplyDelayON = 0;

// Création de la communication à l'ESP8266-01 pour la connexion à Home Assistant.
SoftwareSerial ESP8266(10, 11); // Broches 8 et 9

// Création du clavier pour contrôler le système.
const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;
char keypadHexaKeys[KEYPAD_ROWS][KEYPAD_COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte keypadRowPins[KEYPAD_ROWS] = {30, 32, 34, 36};
byte keypadColPins[KEYPAD_COLS] = {31, 33, 35, 37};
Keypad keypad = Keypad(makeKeymap(keypadHexaKeys), keypadRowPins, keypadColPins, KEYPAD_ROWS, KEYPAD_COLS);

// Création de l'écran OLED qui affiche des informations sur le système.
Adafruit_SSD1306 display(128, 32, &Wire, -1);

// Création de la DEL infrarouge pour contrôler la sono.
// Code ici.

// Création du récepteur infrarouge pour utiliser la télécommande du chromecast avec le système.
IRrecv irrecv(PIN_IR_SENSOR);
decode_results result;

void setup()
{
  // Définition des modes des broches.
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BEDROOM_DOOR_SENSOR, INPUT_PULLUP);
  pinMode(PIN_WARDROBE_DOOR_SENSOR, INPUT);
  pinMode(PIN_CHARGING_STATION_BUTTON, INPUT);
  pinMode(PIN_DOORBELL_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RFID_DATA_1_SENSOR, INPUT_PULLUP);
  pinMode(PIN_RFID_DATA_2_SENSOR, INPUT_PULLUP);
  pinMode(PIN_MUTE_BUTTON, INPUT_PULLUP);
  pinMode(PIN_POWER_SUPPLY, OUTPUT);
  pinMode(PIN_ALARM_RELAY, OUTPUT);
  pinMode(PIN_FAN_RELAY, OUTPUT);
  pinMode(PIN_LED_CUBE_RELAY, OUTPUT);
  pinMode(PIN_TV_RELAY, OUTPUT);
  pinMode(PIN_WARDROBE_LIGHTS_RELAY, OUTPUT);
  pinMode(PIN_RELAY_6, OUTPUT);
  pinMode(PIN_RELAY_7, OUTPUT);
  pinMode(PIN_RELAY_8, OUTPUT);
  pinMode(PIN_MOTOR_TRAY_1, OUTPUT);
  pinMode(PIN_MOTOR_TRAY_2, OUTPUT);
  pinMode(PIN_RED_LED, OUTPUT);
  pinMode(PIN_GREEN_LED, OUTPUT);
  pinMode(PIN_BLUE_LED, OUTPUT);
  pinMode(PIN_IR_LED_SIGNAL, OUTPUT);
  pinMode(PIN_ALARM_SIGNAL, OUTPUT);

  // Lancement des communications.
  ESP8266.begin(9600);
  irrecv.enableIRIn();
  // Serial.begin(115200); // Uniquement pour la résolution de problèmes.

  // Récupération des informations stockées dans la mémoire persistante.
  intrusionCounter = EEPROM.read(1);
  refusalCounter = EEPROM.read(2);
  microSensibility = EEPROM.read(3);

  // Initialisation de la sonnette : comme c'est un bouton ON/OFF, on met un système afin de le "transformer" en bouton poussoir. "doorbellState" correspond à l'état actuel de la sonnette et "dorrbellMode" correspond au mode précédent. Si les deux variables sont différentes, le bouton a été pressé.
  boolean doorbellState = digitalRead(PIN_DOORBELL_BUTTON);
  doorbellMode = doorbellState;

  // Initialisation de l'éclairage de l'armoire.
  boolean wardrobeDoorState = digitalRead(PIN_WARDROBE_DOOR_SENSOR);
  wardrobeDoorPreviousState = wardrobeDoorState;

  // Démarrage de l'écran OLED.
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    noSound();
  }
  display.clearDisplay();
  display.display();
  display.setTextSize(1.7);
  display.setCursor(0, 5);
  display.setTextColor(SSD1306_WHITE);
  display.println(F("Systeme demarre."));
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void loop()
{
  // Lecture des capteurs.
  bedroomDoorState = digitalRead(PIN_BEDROOM_DOOR_SENSOR);
  boolean wardrobeDoorState = digitalRead(PIN_WARDROBE_DOOR_SENSOR);
  boolean doorbellState = digitalRead(PIN_DOORBELL_BUTTON);
  boolean chargingStationButtonState = digitalRead(PIN_CHARGING_STATION_BUTTON);
  boolean muteButtonState = digitalRead(PIN_MUTE_BUTTON);
  microValue = analogRead(PIN_MICRO);

  // Lecture puis traitement de la résistance thermosensible pour récupérer la température.
  int readingTemperature = analogRead(PIN_TEMPERATURE_SENSOR);
  double intermediateTemperature = log(10000.0 * ((1024.0 / readingTemperature - 1)));
  intermediateTemperature = 1 / (0.001129148 + (0.000234125 + (0.0000000876741 * intermediateTemperature * intermediateTemperature)) * intermediateTemperature);
  float FinalTemperature = (intermediateTemperature - 273.15) + 5;
  if (abs(FinalTemperature - temperature) > 0.3) // Pour que le résultat soit plus propre, on ne change la valeur que si une variation significative est présente.
  {
    temperature = FinalTemperature;
  }

  powerSupplyControl();

  if (muteButtonState == HIGH)
  {

    if (muteModePreviousState == true)
    {
      muteModePreviousState = false;
      muteMode = false;
    }

    // Gestion de l'armoire.
    if (wardrobeDoorState == LOW && wardrobeDoorPreviousState == true)
    {
      digitalWrite(PIN_WARDROBE_LIGHTS_RELAY, LOW);
      wardrobeDoorPreviousState = false;
      printDeviceState("armoire", false);
    }

    else if (wardrobeDoorState == HIGH && wardrobeDoorPreviousState == false)
    {
      digitalWrite(PIN_WARDROBE_LIGHTS_RELAY, HIGH);
      wardrobeDoorPreviousState = true;
      printDeviceState("armoire", true);
    }

    // Gestion de la sonnette.
    if (doorbellState != doorbellMode && alarmMode == false)
    {
      printBell();
      musique1();
      doorbellToSend = true;
      doorbellState = digitalRead(PIN_DOORBELL_BUTTON);
      doorbellMode = doorbellState;
    }

    // Vérification de l'alarme.
    if (alarmMode == true && bedroomDoorState == HIGH && alarmIsRinging == false)
    {
      digitalWrite(PIN_ALARM_RELAY, HIGH);
      alarmIsRinging = true;
      intrusionCounter++;
      EEPROM.put(1, intrusionCounter);
    }

    // Gestion des données reçues par l'Arduino Nano Every de la porte.
    if (digitalRead(PIN_RFID_DATA_1_SENSOR) == HIGH)
    {
      if (alarmIsRinging == true)
      {
        stopAlarme();
        yesSound();
      }

      else if (bedroomDoorState == LOW)
      {
        switchAlarm(2);
      }
      delay(500);
    }

    if (digitalRead(PIN_RFID_DATA_2_SENSOR) == HIGH)
    {
      refusalCounter++;
      EEPROM.put(2, refusalCounter);
      noSound();
      delay(500);
    }

    // Gestion de l'arrêt automatique de l'alarme.
    if (alarmIsRinging == true)
    {
      allarmeAutoOff();
    }

    // Gestion du mode des rubans de DELs multicolor.
    if (multicolorMode == true)
    {
      multicolor();
    }

    // Gestion du mode des rubans de DELs son - réaction.
    if (soundReactMode == true)
    {
      soundReact();
    }

    // Gestion de l'arret automatique de l'écran OLED.
    if (ScreenCurrentOnTime >> 0)
    {
      ScreenCurrentOnTime--;
      if (ScreenCurrentOnTime == 0)
      {
        display.clearDisplay();
        display.display();
      }
    }

    // Gestion du clavier.
    char key = keypad.getKey();
    if (key)
    {
      switch (key)
      {
      case '1':
        keypadButton1();
        break;
      case '2':
        keypadButton2();
        break;
      case '3':
        keypadButton3();
        break;
      case '4':
        keypadButton4();
        break;
      case '5':
        keypadButton5();
        break;
      case '6':
        keypadButton6();
        break;
      case '7':
        keypadButton7();
        break;
      case '8':
        keypadButton8();
        break;
      case '9':
        keypadButton9();
        break;
      case '0':
        keypadButton0();
        break;
      case '#':
        keypadButtonHash();
        break;
      case '*':
        keypadButtonStar();
        break;
      case 'A':
        keypadButtonA();
        break;
      case 'B':
        keypadButtonB();
        break;
      case 'C':
        keypadButtonC();
        break;
      case 'D':
        keypadButtonD();
        break;
      }
    }

    // Boucle pour insérer un délais entre de cliques sur la télécommande Google Chromecast.
    if (IRRemoteCounter != 0)
    {
      if (IRRemoteCounter == 500)
      {
        IRRemoteCounter = 0;
      }
      else
      {
        IRRemoteCounter++;
      }
    }

    // Gestion du récepteur infrarouge pour contrôler le système avec la télécommande Google Chromecast.
    if (irrecv.decode())
    {
      if (IRRemoteCounter == 0)
      {
        IRRemoteCounter = 1;
        switch (irrecv.decodedIRData.decodedRawData)
        {
        case 4244768519: // ON/OFF.
          switchTV(2);
          break;
        case 4261480199: // Source.
          if (digitalRead(PIN_LED_CUBE_RELAY) == LOW && multicolorMode == false)
          {
            switchMulticolor(1);
            switchLEDCube(1);
          }

          else
          {
            switchMulticolor(0);
            switchLEDCube(0);
          }

          break;
        case 4161210119: // Vol+.
          delay(1000); // Ce délais permet d'attendre que le bouton de la télécommande soir relachée pour ne pas créer d'interférences avec la DEL qui gère la sono.
          sonoVolume(1);
          break;
        case 4094363399: // Vol-.
          delay(1000); // Ce délais permet d'attendre que le bouton de la télécommande soir relachée pour ne pas créer d'interférences avec la DEL qui gère la sono.
          sonoVolume(0);
          break;
        case 4027516679: // Mute.
          delay(1000); // Ce délais permet d'attendre que le bouton de la télécommande soir relachée pour ne pas créer d'interférences avec la DEL qui gère la sono.
          sonoVolume(2);
        }
      }
      irrecv.resume();
    }

    // Gestion du décompte permettant de garder l'alimentation allumée un certain temps (pour charger des appareils).
    if (powerSupplyDelayON > 0)
    {
      powerSupplyDelayON--;
    }

    // Gestion du double-clique sur la touche 9 pour remettre à zéro le décompte de l'alimentation allumée.
    if (button9Timer > 0)
    {
      button9Timer++;
      if (button9Timer >= 750)
      {
        button9Timer = 0;
      }
    }

    // Gestion du bouton sur la station de charge pour allumer l'alimentation.
    if (chargingStationButtonState == HIGH)
    {
      // On attend que le bouton soit relaché pour annalyser son temps de pression.
      int chargingStationButtonPressedCounter = 0;
      while (digitalRead(PIN_CHARGING_STATION_BUTTON) == HIGH)
      {
        chargingStationButtonPressedCounter++;
        delay(1);
      }

      // Si le bouton a été pressé plus de 0,75 seconde, c'est un appui long et l'alimentation se coupe. Sinon on ajoute une heure (les 5 millisecondes permettent de supprimer les interférences possibles qui pourraient allumer l'alimentation aléatoirement).
      if (chargingStationButtonPressedCounter > 750)
      {
        powerSupplyDelayON = 0;
        printPowerSupplyONTime();
      }

      else if (chargingStationButtonPressedCounter > 5)
      {
        powerSupplyDelayON = powerSupplyDelayON + 3600000;
        printPowerSupplyONTime();
      }
    }
  }
  else if (muteModePreviousState == false)
  {
    delay(10);
    muteButtonState = digitalRead(PIN_MUTE_BUTTON);
    if (muteButtonState == LOW)
    {
      muteModePreviousState = true;
      muteMode = true;
      stopEverything();
    }
  }

  // Gestion des informations reçues par l'ESP8266-01.
  if (ESP8266.available())
  {
    receivedData();
  }

  delay(1);
}