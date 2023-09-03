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

void setKeypadMenu(int menu)
{
  if (keypadMenu == menu)
    return;

  keypadMenu = menu;
  displayKeypadMenu();

  if (keypadMenu == LIGHTS_MENU || keypadMenu == DEVICES_MENU || keypadMenu == TV_MENU || keypadMenu == CONFIGURATION_MENU)
    keypadSubMenuTimer = 0;

  else
    keypadSubMenuTimer = millis();
}

void keypadButtonPressed(char key, boolean longPress)
{
  if (key == 'A')
  {
    if (keypadMenu == ALARM_CODE_CONFIGURATION_SUBMENU)
      alarmCode = "b";

    setKeypadMenu(LIGHTS_MENU);
    return;
  }

  if (keypadMenu != LIGHTS_MENU && keypadMenu != DEVICES_MENU && keypadMenu != TV_MENU && keypadMenu != CONFIGURATION_MENU)
    keypadSubMenuTimer = millis();

  if (keypadMenu == LIGHTS_MENU)
  {
    switch (key)
    {
    case '1':
      // Basculer lumière plafond et afficher à l'écran.
      break;

    case '2':
      if (!longPress)
      {
        // Basculer lumière canapé et afficher à l'écran.
      }

      else
        setKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);

      break;

    case '3':
      if (!longPress)
      {
        // Basculer lampe de chevet et afficher à l'écran.
      }

      else
        setKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);

      break;

    case '4':
      switchDeskLight(TOGGLE, true);
      break;

    case '5':
      if (!longPress)
        switchRGBStrip(TOGGLE, true);

      else
        setKeypadMenu(RGB_STRIP_CONTROL_SUBMENU);

      break;

    case 'D':
      setKeypadMenu(DEVICES_MENU);
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
      setKeypadMenu(SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU);
      break;

    case '2':
      setKeypadMenu(SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU);
      break;

    case '3':
      setKeypadMenu(SOFA_LIGHT_EFFECT_CONTROL_SUBMENU);
      break;

    case 'B':
      setKeypadMenu(LIGHTS_MENU);
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
      setKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
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
      setKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == SOFA_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Effet 1.
      break;

    case '2':
      // Effet 2.
      break;

    case 'B':
      setKeypadMenu(SOFA_LIGHT_CONTROL_SUBMENU);
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
      setKeypadMenu(BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU);
      break;

    case '2':
      setKeypadMenu(BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU);
      break;

    case '3':
      setKeypadMenu(BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU);
      break;

    case '4':
      setKeypadMenu(BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU);
      break;

    case 'B':
      setKeypadMenu(LIGHTS_MENU);
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
      setKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
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
      setKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
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
      setKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    switch (key)
    {
    case '1':
      // Effet 1.
      break;

    case '2':
      // Effet 2.
      break;

    case 'B':
      setKeypadMenu(BEDSIDE_LIGHT_CONTROL_SUBMENU);
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
      setKeypadMenu(RGB_STRIP_COLOR_CONTROL_SUBMENU);
      break;

    case '2':
      setKeypadMenu(RGB_STRIP_EFFECT_CONTROL_SUBMENU);
      break;

    case 'B':
      setKeypadMenu(LIGHTS_MENU);
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
      displayLEDState();
      break;

    case '2':
      controlRGBStrip(RLEDValue, GLEDValue + RGBStripPrecision, BLEDValue);
      displayLEDState();
      break;

    case '3':
      controlRGBStrip(RLEDValue, GLEDValue, BLEDValue + RGBStripPrecision);
      displayLEDState();
      break;

    case '4':
      controlRGBStrip(RLEDValue - RGBStripPrecision, GLEDValue, BLEDValue);
      displayLEDState();
      break;

    case '5':
      controlRGBStrip(RLEDValue, GLEDValue - RGBStripPrecision, BLEDValue);
      displayLEDState();
      break;

    case '6':
      controlRGBStrip(RLEDValue, GLEDValue, BLEDValue - RGBStripPrecision);
      displayLEDState();
      break;

    case 'B':
      setKeypadMenu(RGB_STRIP_CONTROL_SUBMENU);
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
      switchMulticolor(TOGGLE, true);
      break;

    case '2':
      switchSoundReact(TOGGLE, true);
      break;

    case '4':
      multicolorSpeed++;
      displayMulticolorSpeed();
      break;

    case '5':
      if (soundReactSensibility <= 0.95)
        soundReactSensibility = soundReactSensibility + 0.05;

      displaySoundReactSensibility();
      break;

    case '7':
      if (multicolorSpeed >= 1)
        multicolorSpeed = multicolorSpeed - 1;

      displayMulticolorSpeed();
      break;

    case '8':
      if (soundReactSensibility >= 0.05)
        soundReactSensibility = soundReactSensibility - 0.05;

      displaySoundReactSensibility();
      break;

    case 'B':
      setKeypadMenu(RGB_STRIP_CONTROL_SUBMENU);
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
      switchLEDCube(TOGGLE, true);
      break;

    case '2':
      switchStreet(TOGGLE, true);
      break;

    case '3':
      switchDisco(TOGGLE, true);
      break;

    case '4':
      switchTray(TOGGLE, true);
      break;

    case '5':
      displayAirValues();
      break;

    case 'C':
      setKeypadMenu(LIGHTS_MENU);
      break;

    case 'D':
      setKeypadMenu(TV_MENU);
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
      switchTV(TOGGLE, true);
      break;

    case '2':
      syncVolume(true);
      break;

    case '4':
      volumeSono(DECREASE, true);
      break;

    case '5':
      volumeSono(TOGGLE_MUTE, true);
      break;

    case '6':
      volumeSono(INCREASE, true);
      break;

    case 'C':
      setKeypadMenu(DEVICES_MENU);
      break;

    case 'D':
      setKeypadMenu(CONFIGURATION_MENU);
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
      setKeypadMenu(ALARM_CODE_CONFIGURATION_SUBMENU);
      break;

    case 'C':
      setKeypadMenu(TV_MENU);
      break;

    default:
      break;
    }
  }

  else if (keypadMenu == ALARM_CODE_CONFIGURATION_SUBMENU)
  {

    if (key == '1' || key == '2' || key == '3' || key == '4' || key == '5' || key == '6' || key == '7' || key == '8' || key == '9' || key == '0')
    {
      if (alarmCode == "b")
        alarmCode = key;

      else
        alarmCode += key;

      if (alarmCode.length() == 4)
      {
        if (alarmCode == "2023")
          setKeypadMenu(ALARM_CONFIGURATION_SUBMENU);

        else
        {
          setKeypadMenu(CONFIGURATION_MENU);
          displayMessage("ERREUR", "Mot de passe incorrect.");
        }

        alarmCode = "b";
      }

      return;
    }

    if (key == 'b')
    {
      setKeypadMenu(CONFIGURATION_MENU);
      alarmCode = "b";
    }
  }

  else if (keypadMenu == ALARM_CONFIGURATION_SUBMENU)
  {
    switch (key)
    {
    case '1':
      if (alarmState == true)
        return;

      if (alarmBuzzerState == false)
      {
        alarmBuzzerState = true;
        displayMessage("INFO", "Le buzzer de l'alarme est active.");
      }

      else
      {
        alarmBuzzerState = false;
        displayMessage("INFO", "Le buzzer de l'alarme est desactive.");
      }

      break;

    case '2':
      if (alarmState == true)
        return;

      if (cardToStoreState == false)
      {
        cardToStoreState = true;
        displayMessage("INFO", "Presentez au lecteur NFC la carte à enregistrer.");
      }

      else
      {
        cardToStoreState = false;
        displayMessage("INFO", "Annule.");
      }
      break;

    case '3':
      if (alarmState == true)
        return;

      removeCards();
      break;

    case 'b':
      setKeypadMenu(CONFIGURATION_MENU);
      break;

    default:
      break;
    }
  }
}