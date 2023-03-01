/**
 * @file alarm.cpp
 * @author Louis L
 * @brief Fonctions de gestion de l'alarme.
 * @version 1.0
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include <main.hpp>
#include <pinDefinitions.hpp>

// Variables globales.
boolean alarmMode = false;
boolean alarmIsRinging = false;
boolean alarmLEDIsON = false;
int alarmLEDCounter1 = 0;
int alarmLEDCounter2 = 2;
int alarmRingTime = 1;
int refusalCounter;
int intrusionCounter;

// Fonction qui arrète de faire sonner l'alarme proprement.
void stopAlarme()
{
  alarmIsRinging = false;
  alarmRingTime = 0;
  digitalWrite(PIN_ALARM_RELAY, LOW);
  alarmRingTime = 0;
  digitalWrite(PIN_RED_LED, LOW);
  alarmLEDCounter1 = 0;
  alarmLEDCounter2 = 2;
}

// Boucle qui s'exécute lorsque l'alarme sonne. Elle permet de l'éteindre après un certain temps si la porte est refermée. De plus, cette boucle gère le clignottement des rubans de DELs en rouge.
void allarmeAutoOff()
{
  if (alarmLEDCounter2 == 200)
  {
    digitalWrite(PIN_RED_LED, HIGH);
    alarmLEDCounter2 = 0;
    alarmLEDIsON = true;
  }

  if (bedroomDoorState == LOW)
  {
    alarmRingTime = alarmRingTime + 1;
    if (alarmRingTime == 10000)
    {
      stopAlarme();
    }
  }

  else
  {
    alarmRingTime = 0;
  }

  if (alarmLEDCounter1 == 200)
  {
    digitalWrite(PIN_RED_LED, LOW);
    alarmLEDCounter1 = 0;
    alarmLEDIsON = false;
  }

  if (alarmLEDIsON == true)
  {
    alarmLEDCounter1++;
  }

  else
  {
    alarmLEDCounter2++;
  }
}

// Permet de communiquer à l'Arduino Nano Every de la porte l'état de l'alarme (pour allumer ou non la DEL).
void infoAlarme()
{
  if (alarmMode == true)
  {
    digitalWrite(PIN_ALARM_SIGNAL, HIGH);
  }
  else
  {
    digitalWrite(PIN_ALARM_SIGNAL, LOW);
  }
}