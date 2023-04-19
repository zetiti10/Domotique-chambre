/**
 * @file LEDStrips.cpp
 * @author Louis L
 * @brief Fonctions de gestion des rubans de DEL multicolores.
 * @version 1.1
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include <LEDStrips.hpp>
#include <pinDefinitions.hpp>
#include <main.hpp>
#include <devices.hpp>
#include <display.hpp>

// Variables globales
int LEDPowerSteps = 25;

int RLEDValue = 0;
int GLEDValue = 0;
int BLEDValue = 0;
int RLEDValueSave;
int GLEDValueSave;
int BLEDValueSave;
boolean multicolorMode = false;
boolean soundReactMode = false;

int multicolorCounter1 = 0;
int multicolorCounter2 = 0;
int multicolorCounter3 = 0;
int multicolorCounterR;
int multicolorCounterG;
int multicolorCounterB;
boolean multicolorCounterFirst1 = false;
boolean multicolorCounterFirst2 = false;
boolean multicolorCounterFirst3 = false;
int multicolorDelay = 0;
int multicolorSpeed = 10;

void multicolor()
{
  multicolorDelay++;
  if (multicolorDelay >= multicolorSpeed)
  {
    multicolorDelay = 0;
    if (multicolorCounter1 < 255)
    {
      if (multicolorCounterFirst1 == false)
      {
        multicolorCounterFirst3 = false;
        multicolorCounterR = 255;
        multicolorCounterG = 0;
        multicolorCounterB = 0;
        multicolorCounterFirst1 = true;
      }
      multicolorCounter1 = multicolorCounter1 + 1;
      multicolorCounterR -= 1;
      multicolorCounterG += 1;
      RLEDValue = multicolorCounterR;
      analogWrite(PIN_RED_LED, multicolorCounterR);
      GLEDValue = multicolorCounterG;
      analogWrite(PIN_GREEN_LED, multicolorCounterG);
      return;
      /////////////////////////
    }

    else if (multicolorCounter2 < 255)
    {
      if (multicolorCounterFirst2 == false)
      {
        multicolorCounterFirst1 = false;
        multicolorCounterR = 0;
        multicolorCounterG = 255;
        multicolorCounterB = 0;
        multicolorCounterFirst2 = true;
      }

      multicolorCounter2 = multicolorCounter2 + 1;
      multicolorCounterG -= 1;
      multicolorCounterB += 1;

      GLEDValue = multicolorCounterG;
      analogWrite(PIN_GREEN_LED, multicolorCounterG);
      BLEDValue = multicolorCounterB;
      analogWrite(PIN_BLUE_LED, multicolorCounterB);
    }

    else if (multicolorCounter3 < 255)
    {
      if (multicolorCounterFirst3 == false)
      {
        multicolorCounterFirst2 = false;
        multicolorCounterR = 0;
        multicolorCounterG = 0;
        multicolorCounterB = 255;
        multicolorCounterFirst3 = true;
      }

      multicolorCounter3 = multicolorCounter3 + 1;
      multicolorCounterB -= 1;
      multicolorCounterR += 1;

      BLEDValue = multicolorCounterB;
      analogWrite(PIN_BLUE_LED, multicolorCounterB);
      RLEDValue = multicolorCounterR;
      analogWrite(PIN_RED_LED, multicolorCounterR);
    }

    else
    {
      multicolorCounterFirst3 = false;
      multicolorCounter1 = 0;
      multicolorCounter2 = 0;
      multicolorCounter3 = 0;
      multicolorCounterR = 255;
      multicolorCounterG = 0;
      multicolorCounterB = 0;
    }
  }
}

void stopMulticolor()
{
  multicolorMode = false;
  multicolorCounter1 = 0;
  multicolorCounter2 = 0;
  multicolorCounter3 = 0;
  multicolorCounterR = 0;
  multicolorCounterG = 0;
  multicolorCounterB = 0;
  multicolorCounterFirst1 = true;
  multicolorCounterFirst2 = false;
  multicolorCounterFirst3 = false;
  RLEDValue = 0;
  GLEDValue = 0;
  BLEDValue = 0;
  analogWrite(PIN_RED_LED, RLEDValue);
  analogWrite(PIN_GREEN_LED, GLEDValue);
  analogWrite(PIN_BLUE_LED, BLEDValue);
  multicolorDelay = 0;
  printDeviceState("multicolor", false);
}

int altearn = 0;

void soundReact()
{
  if ((microValue >= (287 + microSensibility) || microValue <= (287 - microSensibility)) && microDelay >= 100)
  {
    int r1 = random(255);
    analogWrite(PIN_RED_LED, r1);
    multicolorCounterR = r1;
    int r2 = random(255);
    analogWrite(PIN_GREEN_LED, r2);
    multicolorCounterG = r2;
    int r3 = random(255);
    analogWrite(PIN_BLUE_LED, r3);
    multicolorCounterB = r3;
    microDelay = 0;
  }

  if (altearn >= 5)
  {
    altearn = 0;

    if (multicolorCounterR >= 1)
    {
      multicolorCounterR = multicolorCounterR - 1;
      analogWrite(PIN_RED_LED, multicolorCounterR);
    }

    if (multicolorCounterG >= 1)
    {
      multicolorCounterG = multicolorCounterG - 1;
      analogWrite(PIN_GREEN_LED, multicolorCounterG);
    }

    if (multicolorCounterB >= 1)
    {
      multicolorCounterB = multicolorCounterB - 1;
      analogWrite(PIN_BLUE_LED, multicolorCounterB);
    }
  }

  altearn++;

  if (microDelay < 100)
  {
    microDelay = microDelay + 1;
  }
}

void stopSoundReact()
{
  soundReactMode = false;
  multicolorCounterR = 0;
  multicolorCounterG = 0;
  multicolorCounterB = 0;
  RLEDValueSave = 0;
  GLEDValueSave = 0;
  BLEDValueSave = 0;
  digitalWrite(PIN_RED_LED, RLEDValue);
  digitalWrite(PIN_GREEN_LED, GLEDValue);
  digitalWrite(PIN_BLUE_LED, BLEDValue);
  printDeviceState("soundReactMode", false);
}