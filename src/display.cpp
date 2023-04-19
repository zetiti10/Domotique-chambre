/**
 * @file display.cpp
 * @author Louis L
 * @brief Fonctions d'affichage d'éléments sur l'écran OLED du boîtier de contrôle.
 * @version 1.1
 * @date 2023-03-01
 */

// Autres fichiers du programme.
#include <main.hpp>
#include <LEDStrips.hpp>
#include <logos.hpp>
#include <display.hpp>
#include <devices.hpp>

int ScreenOnTime = 4000;
int ScreenCurrentOnTime = 0;

// Affichage des pictogrammes.
void printBell()
{
  display.clearDisplay();
  display.drawBitmap(0, 0, logoBell, 128, 32, 1);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

// Paramètre :  0 = diminuer le volume - 1 = augmenter le volume - 2 = couper le son.
void printVolume(int element)
{
  if (element == 0)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoVolumeMinus, 128, 32, 1);
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }

  else if (element == 1)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoVolumePlus, 128, 32, 1);
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }

  else if (element == 2)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoVolumeMute, 128, 32, 1);
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }
}

// Paramètre :  0 = couper l'alarme - 1 = allumer l'alarme - 2 = l'alarme sonne (normal) - 3 = l'alarme sonne (inversé).
void printAlarm(int element)
{
  if (element == 0)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoAlarm, 128, 32, 1);
    display.setTextSize(3);
    display.setCursor(5, 5);
    display.println(F("OFF"));
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }

  else if (element == 1)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoAlarm, 128, 32, 1);
    display.setTextSize(3);
    display.setCursor(5, 5);
    display.println(F("ON"));
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }

  else if (element == 2)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoAlarmTriggered, 128, 32, 1);
    display.invertDisplay(false);
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }

  else if (element == 3)
  {
    display.clearDisplay();
    display.drawBitmap(0, 0, logoAlarmTriggered, 128, 32, 1);
    display.invertDisplay(true);
    display.display();
    ScreenCurrentOnTime = ScreenOnTime;
  }
}

void printAllOFF()
{
  display.clearDisplay();
  display.drawBitmap(0, 0, logoAllOFF, 128, 32, 1);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printPowerSupplyONTime()
{
  display.clearDisplay();
  display.drawBitmap(0, 0, logoChargingStation, 128, 32, 1);
  display.setTextSize(3);
  display.setCursor(5, 5);

  if (powerSupplyDelayON == 0)
  {
    display.println(F("OFF"));
  }

  else
  {
    int timeInMinutes = powerSupplyDelayON / 60000;
    display.println(timeInMinutes);
  }

  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

// Affichage de diverses informations.
void printTemperature()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(25, 0);
  display.println(F("Temperature :"));
  display.setTextSize(3);
  display.setCursor(15, 10);
  display.println(temperature);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printLEDState()
{
  display.clearDisplay();
  display.setTextSize(1);
  
  display.setCursor(23, 0);
  display.println(F("R"));
  int rrr = int(map(RLEDValue, 0, 255, 14, 32));
  display.drawLine(25, 14, 25, rrr, SSD1306_WHITE);
  display.setCursor(63, 0);

  display.println(F("V"));
  int ggg = int(map(GLEDValue, 0, 255, 14, 32));
  display.drawLine(65, 14, 65, ggg, SSD1306_WHITE);
  display.setCursor(102, 0);

  display.println(F("B"));
  int bbb = int(map(BLEDValue, 0, 255, 14, 32));
  display.drawLine(102, 14, 102, bbb, SSD1306_WHITE);

  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printDeviceState(String element, boolean on)
{
  display.clearDisplay();

  display.setTextSize(1);
  if (element == "cube")
  {
    display.setCursor(47, 0);
    display.println(F("Cube :"));
  }

  else if (element == "ordi")
  {
    display.setCursor(47, 0);
    display.println(F("Ordi :"));
  }

  else if (element == "fan")
  {
    display.setCursor(40, 0);
    display.println(F("Ventilo :"));
  }

  else if (element == "armoire")
  {
    display.setCursor(42, 0);
    display.println(F("Armoire :"));
  }

  else if (element == "multicolor")
  {
    display.setCursor(30, 0);
    display.println(F("Multicolor :"));
  }

  else if (element == "SoundReact")
  {
    display.setCursor(25, 0);
    display.println(F("Music react :"));
  }

  else if (element == "tv")
  {
    display.setCursor(50, 0);
    display.println(F("TV :"));
  }

  display.setTextSize(3);
  if (on == true)
  {
    display.setCursor(46, 10);
    display.println(F("ON"));
  }

  else
  {
    display.setCursor(37, 10);
    display.println(F("OFF"));
  }

  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printKeypadMode(String mode)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(50, 0);
  display.println(F("Mode :"));
  display.setTextSize(3);
  display.setCursor(56, 10);

  if (mode == "A")
  {
    display.println(F("A"));
  }

  else if (mode == "B")
  {
    display.println(F("B"));
  }

  else if (mode == "C")
  {
    display.println(F("C"));
  }

  else if (mode == "D")
  {
    display.println(F("D"));
  }

  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

// Affichage des valeurs pour le menu de configuration.
void printMicroSensibility()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Sensibilite du microphone :"));
  display.setTextSize(2);
  display.setCursor(50, 18);
  display.println(microSensibility);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printMulticolorSpeed()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Vitesse de l'animation multicolore :"));
  display.setTextSize(2);
  display.setCursor(50, 18);
  display.println(multicolorSpeed);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printVolumePrecision()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Precision du changement de volume :"));
  display.setTextSize(2);
  display.setCursor(50, 18);
  display.println(volumePrecision);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}