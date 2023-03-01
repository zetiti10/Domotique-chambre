/**
 * @file keypadFunctions.cpp
 * @author Louis L
 * @brief Fonctions de gestion du clavier du boîtier de contrôle.
 * @version 1.0
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include <main.hpp>
#include <keypadFunctions.hpp>
#include <devices.hpp>
#include <LEDStrips.hpp>
#include <pinDefinitions.hpp>
#include <screen.hpp>

String keypadMode = "A";

void keypadButton1()
{
  if (keypadMode == "A")
  {
    switchFan(2);
  }

  else if (keypadMode == "B")
  {
    if (RLEDValue <= 245)
    {
      RLEDValue = RLEDValue + 10;
      analogWrite(PIN_RED_LED, RLEDValue);
      printLEDState();
    }
  }
}

void keypadButton2()
{
  if (keypadMode == "A")
  {
    switchLEDCube(2);
  }

  else if (keypadMode == "B")
  {
    if (GLEDValue <= 245)
    {
      GLEDValue = GLEDValue + 10;
      analogWrite(PIN_GREEN_LED, GLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "D")
  {
    microSensibility = microSensibility - 5;
    EEPROM.put(3, microSensibility);
    printMicroSensibility();
  }
}

void keypadButton3()
{
  if (keypadMode == "A")
  {
    switchTray(2);
  }

  else if (keypadMode == "B")
  {
    if (BLEDValue <= 245)
    {
      BLEDValue = BLEDValue + 10;
      analogWrite(PIN_BLUE_LED, BLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "D")
  {
    microSensibility = microSensibility + 5;
    EEPROM.put(3, microSensibility);
    printMicroSensibility();
  }
}

void keypadButton4()
{
  if (keypadMode == "A")
  {
    switchTV(2);
  }

  else if (keypadMode == "B")
  {
    if (RLEDValue >= 10)
    {
      RLEDValue = RLEDValue - 10;
      analogWrite(PIN_RED_LED, RLEDValue);
      printLEDState();
    }
  }
}

void keypadButton5()
{
  if (keypadMode == "A")
  {
    switchAlarm(2);
  }

  else if (keypadMode == "B")
  {
    if (GLEDValue >= 10)
    {
      GLEDValue = GLEDValue - 10;
      analogWrite(PIN_GREEN_LED, GLEDValue);
      printLEDState();
    }
  }
}

void keypadButton6()
{
  if (keypadMode == "A")
  {
    printTemperature();
  }

  else if (keypadMode == "B")
  {
    if (BLEDValue >= 10)
    {
      BLEDValue = BLEDValue - 10;
      analogWrite(PIN_BLUE_LED, BLEDValue);
      printLEDState();
    }
  }
}

void keypadButton7()
{
  if (keypadMode == "A")
  {
    sonoVolume(0);
  }

  else if (keypadMode == "B")
  {
    if (multicolorDelay > 1)
    {
      multicolorDelay--;
    }
  }
}

void keypadButton8()
{
  if (keypadMode == "A")
  {
    sonoVolume(1);
  }

  else if (keypadMode == "B")
  {
    switchMulticolor(2);
  }
}

void keypadButton9()
{
  if (keypadMode == "A")
  {
    if (button9Timer != 0)
    {
      powerSupplyDelayON = 0;
      printPowerSupplyONTime();
    }

    else
    {
      powerSupplyDelayON = powerSupplyDelayON + 3600000;
      //Si l'alimentation était éteinte précédemment, on évide de détecter un double-clique accidentel.
      if (!(powerSupplyDelayON == 3600000))
      {
        button9Timer = 1;
      }
    }
    printPowerSupplyONTime();
  }

  else if (keypadMode == "B")
  {
    multicolorDelay++;
  }
}

void keypadButton0()
{
  if (keypadMode == "A")
  {
    stopEverything();
  }

  else if (keypadMode == "B")
  {
    if (RLEDValue != 0 || GLEDValue != 0 || BLEDValue != 0)
    {
      RLEDValueSave = RLEDValue;
      GLEDValueSave = GLEDValue;
      BLEDValueSave = BLEDValue;
      RLEDValue = 0;
      GLEDValue = 0;
      BLEDValue = 0;
      analogWrite(PIN_RED_LED, RLEDValue);
      analogWrite(PIN_GREEN_LED, GLEDValue);
      analogWrite(PIN_BLUE_LED, BLEDValue);
    }

    else
    {
      RLEDValue = RLEDValueSave;
      GLEDValue = GLEDValueSave;
      BLEDValue = BLEDValueSave;
      analogWrite(PIN_RED_LED, RLEDValue);
      analogWrite(PIN_GREEN_LED, GLEDValue);
      analogWrite(PIN_BLUE_LED, BLEDValue);
    }
  }
}

void keypadButtonHash()
{
  if (keypadMode == "B")
  {
    if (soundReactMode == false)
    {
      soundReactMode = true;
      printDeviceState("soundReactMode", true);
    }

    else
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
  }
}

void keypadButtonStar() {}

void keypadButtonA()
{
  keypadMode = "A";
  printKeypadMode("A");
}

void keypadButtonB()
{
  keypadMode = "B";
  printKeypadMode("B");
}

void keypadButtonC()
{
  keypadMode = "C";
  printKeypadMode("C");
}

void keypadButtonD()
{
  keypadMode = "D";
  printKeypadMode("D");
}