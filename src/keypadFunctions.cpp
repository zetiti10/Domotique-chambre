/**
 * @file keypadFunctions.cpp
 * @author Louis L
 * @brief Fonctions de gestion du clavier du boîtier de contrôle.
 * @version 1.1
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
#include <display.hpp>

String keypadMode = "A";

void keypadButton1()
{
  if (keypadMode == "A")
  {
    switchFan(2);
  }

  else if (keypadMode == "B")
  {
    if (RLEDValue <= (255 - LEDPowerSteps))
    {
      RLEDValue = RLEDValue + LEDPowerSteps;
      analogWrite(PIN_RED_LED, RLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "C")
  {
    volumeSono(0);
  }

  else if (keypadMode == "D")
  {
    if (microSensibility < 200)
    {
      microSensibility = microSensibility + 5;
      EEPROM.update(3, microSensibility);
      printMicroSensibility();
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
    if (GLEDValue <= (255 - LEDPowerSteps))
    {
      GLEDValue = GLEDValue + LEDPowerSteps;
      analogWrite(PIN_GREEN_LED, GLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "C")
  {
    volumeSono(2);
  }

  else if (keypadMode == "D")
  {
    if (multicolorSpeed < 30)
    {
      multicolorSpeed++;
      EEPROM.update(4, multicolorSpeed);
      printMulticolorSpeed();
    }
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
    if (BLEDValue <= (255 - LEDPowerSteps))
    {
      BLEDValue = BLEDValue + LEDPowerSteps;
      analogWrite(PIN_BLUE_LED, BLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "C")
  {
    volumeSono(1);
  }

  else if (keypadMode == "D")
  {
    if (volumePrecision < 20)
    {
      volumePrecision++;
      EEPROM.update(5, volumePrecision);
      printVolumePrecision();
    }
  }
}

void keypadButton4()
{
  if (keypadMode == "A")
  {
    if (button4Timer != 0)
    {
      powerSupplyDelayON = 0;
      printPowerSupplyONTime();
    }

    else
    {
      powerSupplyDelayON = powerSupplyDelayON + 3600000;
      // Si l'alimentation était éteinte précédemment, on évide de détecter un double-clique accidentel.
      if (!(powerSupplyDelayON == 3600000))
      {
        button4Timer = 1;
      }
    }
    printPowerSupplyONTime();
  }

  else if (keypadMode == "B")
  {
    if (RLEDValue >= LEDPowerSteps)
    {
      RLEDValue = RLEDValue - LEDPowerSteps;
      analogWrite(PIN_RED_LED, RLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "C")
  {
    switchTV(2);
  }

  else if (keypadMode == "D")
  {
    if (microSensibility >= 5)
    {
      microSensibility = microSensibility - 5;
      EEPROM.update(3, microSensibility);
      printMicroSensibility();
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
    if (GLEDValue >= LEDPowerSteps)
    {
      GLEDValue = GLEDValue - LEDPowerSteps;
      analogWrite(PIN_GREEN_LED, GLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "D")
  {
    if (multicolorSpeed >= 1)
    {
      multicolorSpeed--;
      EEPROM.update(4, multicolorSpeed);
      printMulticolorSpeed();
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
    if (BLEDValue >= LEDPowerSteps)
    {
      BLEDValue = BLEDValue - LEDPowerSteps;
      analogWrite(PIN_BLUE_LED, BLEDValue);
      printLEDState();
    }
  }

  else if (keypadMode == "D")
  {
    if (volumePrecision >= 1)
    {
      volumePrecision--;
      EEPROM.update(5, volumePrecision);
      printVolumePrecision();
    }
  }
}

void keypadButton7()
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
      stopSoundReact();
    }
  }
}

void keypadButton8()
{
  if (keypadMode == "B")
  {
    switchMulticolor(2);
  }
}

void keypadButton9() {}

void keypadButton0()
{
  if (keypadMode == "A")
  {
    stopEverything();
    printAllOFF();
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

void keypadButtonHash() {}

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