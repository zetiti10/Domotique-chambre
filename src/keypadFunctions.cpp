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
#include <buzzer.hpp>

int keypadMenu = LIGHTS_MENU;

unsigned long pressedKeypadTouchTime = 0;

unsigned long keypadSubMenuTimer = 0;

String alarmCode = "b";

void keypadButtonPressed(char key, boolean longPress)
{

  clickSound();

  if (key == 'A')
  {
    if (keypadMenu == ALARM_CODE_CONFIGURATION_MENU)
    {
      alarmCode = "b";
    }

    keypadMenu = LIGHTS_MENU;
    printKeypadMenu(LIGHTS_MENU);
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
        printKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
        keypadSubMenuTimer = millis();
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
        printKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
        keypadSubMenuTimer = millis();
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
        printKeypadMenu(RGB_STRIP_CONTROL_SUBMENU);
        keypadSubMenuTimer = millis();
      }
      break;

    case 'D':
      keypadMenu = DEVICES_MENU;
      printKeypadMenu(DEVICES_MENU);
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
      printKeypadMenu(SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case '2':
      keypadMenu = SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU;
      printKeypadMenu(SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case '3':
      keypadMenu = SOFA_LIGHT_EFFECT_CONTROL_SUBMENU;
      printKeypadMenu(SOFA_LIGHT_EFFECT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case 'B':
      keypadMenu = LIGHTS_MENU;
      printKeypadMenu(LIGHTS_MENU);
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
      printKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case '2':
      keypadMenu = BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU;
      printKeypadMenu(BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case '3':
      keypadMenu = BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU;
      printKeypadMenu(BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case '4':
      keypadMenu = BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU;
      printKeypadMenu(BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case 'B':
      keypadMenu = LIGHTS_MENU;
      printKeypadMenu(LIGHTS_MENU);
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
      printKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
      printKeypadMenu(RGB_STRIP_COLOR_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case '2':
      keypadMenu = RGB_STRIP_EFFECT_CONTROL_SUBMENU;
      printKeypadMenu(RGB_STRIP_EFFECT_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    case 'B':
      keypadMenu = LIGHTS_MENU;
      printKeypadMenu(LIGHTS_MENU);
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
      printKeypadMenu(RGB_STRIP_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
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
        multicolorSpeed = multicolorSpeed - 1;
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
      printKeypadMenu(RGB_STRIP_CONTROL_SUBMENU);
      keypadSubMenuTimer = millis();
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == DEVICES_MENU)
  {
    switch (key)
    {
    case '1':
      switchLEDCube(TOGGLE);
      break;

    case '2':
      switchStreet(TOGGLE);
      break;

    case '3':
      switchDisco(TOGGLE);
      break;

    case '4':
      switchTray(TOGGLE);
      break;

    case 'C':
      keypadMenu = LIGHTS_MENU;
      printKeypadMenu(LIGHTS_MENU);
      break;

    case 'D':
      keypadMenu = TV_MENU;
      printKeypadMenu(TV_MENU);
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == TV_MENU)
  {
    switch (key)
    {
    case '1':
      switchTV(TOGGLE);
      break;

    case '4':
      volumeSono(DECREASE);
      break;

    case '5':
      volumeSono(TOGGLE_MUTE);
      break;

    case '6':
      volumeSono(INCREASE);
      break;

    case 'C':
      keypadMenu = DEVICES_MENU;
      printKeypadMenu(DEVICES_MENU);
      break;

    case 'D':
      keypadMenu = CONFIGURATION_MENU;
      printKeypadMenu(CONFIGURATION_MENU);
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == CONFIGURATION_MENU)
  {
    switch (key)
    {
    case '1':
      keypadMenu = ALARM_CODE_CONFIGURATION_MENU;
      printKeypadMenu(ALARM_CODE_CONFIGURATION_MENU);
      keypadSubMenuTimer = millis();
      break;

    case 'C':
      keypadMenu = TV_MENU;
      printKeypadMenu(TV_MENU);
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == ALARM_CODE_CONFIGURATION_MENU)
  {

    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
    {
      if (alarmCode == "b")
      {
        alarmCode = key;
      }

      else
      {
        alarmCode += key;
      }

      if (alarmCode.length() == 3)
      {
        if (alarmCode == "2023")
        {
          keypadMenu = ALARM_CONFIGURATION_MENU;
          printKeypadMenu(ALARM_CONFIGURATION_MENU);
          keypadSubMenuTimer = millis();
        }

        else
        {
          // Refus.
          keypadMenu = CONFIGURATION_MENU;
          printKeypadMenu(CONFIGURATION_MENU);
        }

        alarmCode = "b";
      }

      return;
    }

    switch (key)
    {

    case 'B':
      keypadMenu = CONFIGURATION_MENU;
      printKeypadMenu(CONFIGURATION_MENU);
      break;

    default:
      break;
    }

    alarmCode = "b";
  }

  else if (keypadMenu == ALARM_CONFIGURATION_MENU)
  {
    switch (key)
    {
    case '1':
      if (alarmState == true)
      {
        return;
      }

      if (alarmBuzzerState == false)
      {
        alarmBuzzerState = true;
      }

      else
      {
        alarmBuzzerState = false;
      }

      break;

    case '2':
      if (alarmState == false)
      {
        if (cardToStoreState == false)
        {
          cardToStoreState = true;
        }

        else
        {
          cardToStoreState = false;
        }
      }
      break;

    case '3':
      removeCards();
      break;

    default:
      break;
    }
  }
}