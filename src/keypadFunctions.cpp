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
#include <pinDefinitions.hpp>
#include <display.hpp>

int keypadMenu = LIGHTS_MENU;

unsigned long pressedKeypadTouchTime = 0;

void keypadButtonPressed(char key, boolean longPress)
{

  if (key == 'A')
  {
    keypadMenu = LIGHTS_MENU;
    printKeypadMode("A");
    return;
  }

  if (keypadMenu == LIGHTS_MENU)
  {
    switch (key)
    {
    case '1':
      // Basculer lumière plafond et afficher à l'écran.
      break;

    case '2':
      if (longPress == false)
      {
        // Basculer lumière canapé et afficher à l'écran.
      }

      else
      {
        keypadMenu = SOFA_LIGHT_CONTROL_SUBMENU;
        // Affichage à l'écran des options.
      }

      break;

    case '3':
      if (longPress == false)
      {
        // Basculer lampe de chevet et afficher à l'écran.
      }

      else
      {
        keypadMenu = BEDSIDE_LIGHT_CONTROL_SUBMENU;
        // Affichage à l'écran des options.
      }

      break;

    case '4':
      // Basculer lampe de bureau et afficher à l'écran.
      break;

    case '5':
      if (longPress == false)
      {
        switchRGBStrip(TOGGLE);
      }

      else
      {
        keypadMenu = RGB_STRIP_CONTROL_SUBMENU;
        // Affichage à l'écran des options.
      }
      break;

    case 'D':
      keypadMenu = DEVICES_MENU;
      printKeypadMode("B");
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == SOFA_LIGHT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      keypadMenu = SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case '2':
      keypadMenu = SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case '3':
      keypadMenu = SOFA_LIGHT_EFFECT_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case 'B':
      keypadMenu = LIGHTS_MENU;
      printKeypadMode("A");
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Diminuer la température de couleur.
      break;

    case '3':
      // Augmenter la température de couleur.
      break;

    case 'B':
      keypadMenu = SOFA_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Diminuer la luminosité.
      break;

    case '3':
      // Augmenter la luminosité.
      break;

    case 'B':
      keypadMenu = SOFA_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == SOFA_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case 'B':
      keypadMenu = SOFA_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == BEDSIDE_LIGHT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      keypadMenu = BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case '2':
      keypadMenu = BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case '3':
      keypadMenu = BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case '4':
      keypadMenu = BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case 'B':
      keypadMenu = LIGHTS_MENU;
      printKeypadMode("A");
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Diminuer la température de couleur.
      break;

    case '3':
      // Augmenter la température de couleur.
      break;

    case 'B':
      keypadMenu = BEDSIDE_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Diminuer la couleur.
      break;

    case '3':
      // Augmenter la couleur.
      break;

    case 'B':
      keypadMenu = BEDSIDE_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Diminuer la luminosité.
      break;

    case '3':
      // Augmenter la luminosité.
      break;

    case 'B':
      keypadMenu = BEDSIDE_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case 'B':
      keypadMenu = BEDSIDE_LIGHT_CONTROL_SUBMENU;
      // Affichage du menu.
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == RGB_STRIP_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      keypadMenu = RGB_STRIP_COLOR_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case '2':
      keypadMenu = RGB_STRIP_EFFECT_CONTROL_SUBMENU;
      // Affichage à l'écran.
      break;

    case 'B':
      keypadMenu = LIGHTS_MENU;
      printKeypadMode("A");
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == RGB_STRIP_COLOR_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      controlRGBStrip(RLEDValue + RGBStripPrecision, GLEDValue, BLEDValue);
      break;

    case '2':
      controlRGBStrip(RLEDValue, GLEDValue + RGBStripPrecision, BLEDValue);
      break;

    case '3':
      controlRGBStrip(RLEDValue, GLEDValue, BLEDValue + RGBStripPrecision);
      break;

    case '4':
      controlRGBStrip(RLEDValue - RGBStripPrecision, GLEDValue, BLEDValue);
      break;

    case '5':
      controlRGBStrip(RLEDValue, GLEDValue - RGBStripPrecision, BLEDValue);
      break;

    case '6':
      controlRGBStrip(RLEDValue, GLEDValue, BLEDValue - RGBStripPrecision);
      break;

    case 'B':
      keypadMenu = RGB_STRIP_CONTROL_SUBMENU;
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == RGB_STRIP_EFFECT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      switchMulticolor(TOGGLE);
      break;

    case '2':
      switchSoundReact(TOGGLE);
      break;

    case '4':
      multicolorSpeed++;
      break;

    case '5':
      if (soundReactSensibility <= 0.95)
      {
        soundReactSensibility = soundReactSensibility + 0.05;
      }
      break;

    case '7':
      if (multicolorSpeed >= 1)
      {
        multicolorSpeed = multicolorSpeed -1;
      }
      break;

    case '8':
      if (soundReactSensibility >= 0.05)
      {
        soundReactSensibility = soundReactSensibility - 0.05;
      }
      break;

    case 'B':
      keypadMenu = RGB_STRIP_CONTROL_SUBMENU;
      break;

    default:
      break;
    }
  }
}

/*void keypadButton1(boolean longPress)
{
  if (keypadMenu == MENU_A)
  {
    switchFan(2);
  }

  else if (keypadMenu == MENU_B)
  {
    if (RLEDValue <= (255 - LEDPowerSteps))
    {
      RLEDValue = RLEDValue + LEDPowerSteps;
      analogWrite(PIN_RED_LED, RLEDValue);
      printLEDState();
    }
  }

  else if (keypadMenu == MENU_C)
  {
    volumeSono(0);
  }

  else if (keypadMenu == MENU_D)
  {
    if (microSensibility < 200)
    {
      microSensibility = microSensibility + 5;
      EEPROM.update(3, microSensibility);
      printMicroSensibility();
    }
  }
}

void keypadButton2(boolean longPress)
{
  if (keypadMenu == MENU_A)
  {
    switchLEDCube(2);
  }

  else if (keypadMenu == MENU_B)
  {
    if (GLEDValue <= (255 - LEDPowerSteps))
    {
      GLEDValue = GLEDValue + LEDPowerSteps;
      analogWrite(PIN_GREEN_LED, GLEDValue);
      printLEDState();
    }
  }

  else if (keypadMenu == MENU_C)
  {
    volumeSono(2);
  }

  else if (keypadMenu == MENU_D)
  {
    if (multicolorSpeed < 30)
    {
      multicolorSpeed++;
      EEPROM.update(4, multicolorSpeed);
      printMulticolorSpeed();
    }
  }
}

void keypadButton3(boolean longPress)
{
  if (keypadMenu == MENU_A)
  {
    switchTray(2);
  }

  else if (keypadMenu == MENU_B)
  {
    if (BLEDValue <= (255 - LEDPowerSteps))
    {
      BLEDValue = BLEDValue + LEDPowerSteps;
      analogWrite(PIN_BLUE_LED, BLEDValue);
      printLEDState();
    }
  }

  else if (keypadMenu == MENU_C)
  {
    volumeSono(1);
  }

  else if (keypadMenu == MENU_D)
  {
    if (volumePrecision < 20)
    {
      volumePrecision++;
      EEPROM.update(5, volumePrecision);
      printVolumePrecision();
    }
  }
}

void keypadButton4(boolean longPress)
{
  if (keypadMenu == MENU_A)
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

  else if (keypadMenu == MENU_B)
  {
    if (RLEDValue >= LEDPowerSteps)
    {
      RLEDValue = RLEDValue - LEDPowerSteps;
      analogWrite(PIN_RED_LED, RLEDValue);
      printLEDState();
    }
  }

  else if (keypadMenu == MENU_C)
  {
    switchTV(2);
  }

  else if (keypadMenu == MENU_D)
  {
    if (microSensibility >= 5)
    {
      microSensibility = microSensibility - 5;
      EEPROM.update(3, microSensibility);
      printMicroSensibility();
    }
  }
}

void keypadButton5(boolean longPress)
{
  if (keypadMenu == MENU_A)
  {
    switchAlarm(2);
  }

  else if (keypadMenu == MENU_B)
  {
    if (GLEDValue >= LEDPowerSteps)
    {
      GLEDValue = GLEDValue - LEDPowerSteps;
      analogWrite(PIN_GREEN_LED, GLEDValue);
      printLEDState();
    }
  }

  else if (keypadMenu == MENU_D)
  {
    if (multicolorSpeed >= 1)
    {
      multicolorSpeed--;
      EEPROM.update(4, multicolorSpeed);
      printMulticolorSpeed();
    }
  }
}

void keypadButton6(boolean longPress)
{
  if (keypadMenu == MENU_A)
  {
    printTemperature();
  }

  else if (keypadMenu == MENU_B)
  {
    if (BLEDValue >= LEDPowerSteps)
    {
      BLEDValue = BLEDValue - LEDPowerSteps;
      analogWrite(PIN_BLUE_LED, BLEDValue);
      printLEDState();
    }
  }

  else if (keypadMenu == MENU_D)
  {
    if (volumePrecision >= 1)
    {
      volumePrecision--;
      EEPROM.update(5, volumePrecision);
      printVolumePrecision();
    }
  }
}

void keypadButton7(boolean longPress)
{
  if (keypadMenu == MENU_B)
  {
    if (soundReactState == false)
    {
      soundReactState = true;
      printDeviceState("soundReactState", true);
    }

    else
    {
      stopSoundReact();
    }
  }
}

void keypadButton8(boolean longPress)
{
  if (keypadMenu == MENU_B)
  {
    switchMulticolor(2);
  }
}

void keypadButton9(boolean longPress) {}

void keypadButton0(boolean longPress)
{
  if (keypadMenu == MENU_A)
  {
    stopEverything();
    printAllOFF();
  }

  else if (keypadMenu == MENU_B)
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

void keypadButtonHash(boolean longPress) {}

void keypadButtonStar(boolean longPress) {}

void keypadButtonA(boolean longPress)
{
  keypadMenu = "A";
  printKeypadMode("A");
}

void keypadButtonB(boolean longPress)
{
  keypadMenu = "B";
  printKeypadMode("B");
}

void keypadButtonC(boolean longPress)
{
  keypadMenu = "C";
  printKeypadMode("C");
}

void keypadButtonD(boolean longPress)
{
  keypadMenu = "D";
  printKeypadMode("D");
}*/