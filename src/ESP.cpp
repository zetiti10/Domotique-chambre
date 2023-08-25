/**
 * @file ESP.cpp
 * @author Louis L
 * @brief Gestion de la communication avec l'ESP8266-01.
 * @version 1.1
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include <ESP.hpp>
#include <devices.hpp>
#include <main.hpp>
#include <pinDefinitions.hpp>

// Fonction complète de communication à l'ESP8266-01 qui st connecté en série. Il permet au système d'être connecté à Home Assistant.
void receivedData()
{

  delay(50);

  // Décodage du message : la variable "receivedMessage" contient le message envoyé par l'ESP8266-01.
  String receivedMessage;
  while (Serial1.available() > 0)
  {
    byte letter = Serial1.read();
    letter = letter - 48;
    receivedMessage += letter;
  }

  // Analyse du message reçu. Pour en savoir plus sur la communication, se réferer à la documentation du projet.

  // Si on reçoit un message de type action à effectuer.
  if (receivedMessage.charAt(0) == '0')
  {
    if (receivedMessage.charAt(1) == '0')
    {
      if (receivedMessage.charAt(2) == '0')
      {
      }

      else
      {
      }
    }

    else if (receivedMessage.charAt(1) == '1')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        switchLEDCube(0);
      }

      else
      {
        switchLEDCube(1);
      }
    }

    else if (receivedMessage.charAt(1) == '2')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        switchTray(0);
      }

      else if (receivedMessage.charAt(2) == '1')
      {
        switchTray(1);
      }
    }

    else if (receivedMessage.charAt(1) == '3')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        switchTV(0);
      }

      else
      {
        switchTV(1);
      }
    }

    else if (receivedMessage.charAt(1) == '4')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        switchAlarm(0);
      }

      else if (receivedMessage.charAt(2) == '1')
      {
        switchAlarm(1);
      }
    }

    else if (receivedMessage.charAt(1) == '5')
    {
      if (receivedMessage.charAt(2) == '1')
      {
      }
    }

    else if (receivedMessage.charAt(1) == '6')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        switchMulticolor(0);
      }

      else
      {
        switchMulticolor(1);
      }
    }

    else if (receivedMessage.charAt(1) == '7')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        switchSoundReact(0);
      }

      else
      {
        switchSoundReact(1);
      }
    }

    else if (receivedMessage.charAt(1) == '8')
    {
      if (receivedMessage.charAt(2) == '0')
      {
        volumeSono(1);
      }

      else if (receivedMessage.charAt(2) == '1')
      {
        volumeSono(0);
      }
    }
  }

  // Si on reçoit un message de type demande de l'état des périphériques du système.
  else if (receivedMessage.charAt(0) == '2')
  {
    if (receivedMessage.charAt(1) == '0')
    {

      String messageToSend = "";

      if (cardToStoreState == true) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (digitalRead(PIN_LED_CUBE_RELAY) == HIGH)
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (alarmState)
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 9;
      }

      else
      {
      }

      int truc = temperature * 100;
      messageToSend += truc;

      if (multicolorState)
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      if (cardToStoreState) // Veut rien dire juste pour enlever l'erreur.
      {
        messageToSend += 1;
      }

      else
      {
        messageToSend += 0;
      }

      Serial1.println(messageToSend);
    }

    else if (receivedMessage.charAt(1) == '1')
    {
      String messageToSend = "";
      int truc = temperature * 100;
      messageToSend += truc;
      Serial1.println(messageToSend);
    }

    else if (receivedMessage.charAt(1) == '2')
    {
      String messageToSend = "";
      if (RLEDValue < 10)
      {
        messageToSend += 00;
        messageToSend += RLEDValue;
      }

      else if (RLEDValue < 100)
      {
        messageToSend += 0;
        messageToSend += RLEDValue;
      }

      else
      {
        messageToSend += RLEDValue;
      }

      if (GLEDValue < 10)
      {
        messageToSend += 00;
        messageToSend += GLEDValue;
      }

      else if (GLEDValue < 100)
      {
        messageToSend += 0;
        messageToSend += GLEDValue;
      }

      else
      {
        messageToSend += GLEDValue;
      }

      if (BLEDValue < 10)
      {
        messageToSend += 00;
        messageToSend += BLEDValue;
      }

      else if (BLEDValue < 100)
      {
        messageToSend += 0;
        messageToSend += BLEDValue;
      }

      else
      {
        messageToSend += BLEDValue;
      }
      Serial1.println(messageToSend);
    }
  }

  // Si on reçoit un message de type changement de la couleur des rubans de DEL.
  else if (receivedMessage.charAt(0) == '3' && receivedMessage.length() == 10)
  {
    int rv = 0;
    int vv = 0;
    int bv = 0;

    rv = (receivedMessage.charAt(1) - 48) * 100;
    rv = rv + (receivedMessage.charAt(2) - 48) * 10;
    rv = rv + (receivedMessage.charAt(3) - 48);
    rv = map(rv, 0, 100, 0, 255);

    vv = (receivedMessage.charAt(4) - 48) * 100;
    vv = vv + (receivedMessage.charAt(5) - 48) * 10;
    vv = vv + (receivedMessage.charAt(6) - 48);
    vv = map(vv, 0, 100, 0, 255);

    bv = (receivedMessage.charAt(7) - 48) * 100;
    bv = bv + (receivedMessage.charAt(8) - 48) * 10;
    bv = bv + (receivedMessage.charAt(9) - 48);
    bv = map(bv, 0, 100, 0, 255);

    analogWrite(PIN_RED_LED, rv);
    RLEDValue = rv;
    analogWrite(PIN_GREEN_LED, vv);
    GLEDValue = vv;
    analogWrite(PIN_BLUE_LED, bv);
    BLEDValue = bv;
  }
}