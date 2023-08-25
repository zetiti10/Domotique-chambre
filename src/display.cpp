/**
 * @file display.cpp
 * @author Louis L
 * @brief Fonctions d'affichage d'éléments sur l'écran OLED du boîtier de contrôle.
 * @version 1.1
 * @date 2023-03-01
 */

// Autres fichiers du programme.
#include <main.hpp>
#include <logos.hpp>
#include <display.hpp>
#include <devices.hpp>
#include <keypadFunctions.hpp>

unsigned long ScreenCurrentOnTime = 0UL;

// Affichage des pictogrammes.
void printBell()
{
  display.clearDisplay();
  // display.drawBitmap(0, 0, logoBell, 128, 32, 1);
  display.display();
  ScreenCurrentOnTime = millis();
}

// Paramètre :  0 = diminuer le volume - 1 = augmenter le volume - 2 = couper le son.
void printVolume(int element)
{
  display.clearDisplay();
  display.setCursor(0, 0);

  if (element == 0)
  {
    // display.drawBitmap(0, 0, logoVolumeMinus, 128, 32, 1);
    display.println(F("PLUS"));
  }

  else if (element == 1)
  {
    // display.drawBitmap(0, 0, logoVolumePlus, 128, 32, 1);
    display.println(F("MOINS"));
  }

  else if (element == 2)
  {
    // display.drawBitmap(0, 0, logoVolumeMute, 128, 32, 1);
    display.println(F("MUTE"));
  }

  display.display();
  ScreenCurrentOnTime = millis();
}

// Paramètre :  0 = couper l'alarme - 1 = allumer l'alarme - 2 = l'alarme sonne (normal) - 3 = l'alarme sonne (inversé).
void printAlarm(int element)
{
  display.clearDisplay();
  display.setCursor(0, 0);

  if (element == 0)
  {
    //display.drawBitmap(0, 0, logoAlarm, 128, 32, 1);
    display.println(F("Alarme"));
    display.setTextSize(3);
    display.setCursor(5, 5);
    display.println(F("OFF"));
  }

  else if (element == 1)
  {
    //display.drawBitmap(0, 0, logoAlarm, 128, 32, 1);
    display.println(F("Alerte"));
    display.setTextSize(3);
    display.setCursor(5, 5);
    display.println(F("ON"));
  }

  else if (element == 2)
  {
    //display.drawBitmap(0, 0, logoAlarmTriggered, 128, 32, 1);
    display.println(F("Alerte"));
    display.invertDisplay(false);
  }

  else if (element == 3)
  {
    //display.drawBitmap(0, 0, logoAlarmTriggered, 128, 32, 1);
    display.println(F("Alerte"));
    display.invertDisplay(true);
  }

  display.display();
  ScreenCurrentOnTime = millis();
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
  ScreenCurrentOnTime = millis();
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
  ScreenCurrentOnTime = millis();
}

void printDeviceState(boolean on)
{
  display.clearDisplay();
  display.setCursor(47, 0);

  if (on == true)
  {
    display.println(F("ON"));
  }

  else
  {
    display.println(F("OFF"));
  }

  display.display();
  ScreenCurrentOnTime = millis();
}

void printKeypadMenu(int menu)
{
  display.clearDisplay();
  display.setCursor(0, 0);

  if (menu == LIGHTS_MENU)
  {
    display.println(F("Menu lumieres"));
  }

  else if (menu == SOFA_LIGHT_CONTROL_SUBMENU)
  {
    display.println(F("1. Temperature - 2. Luminosite - 3. Effet"));
  }

  else if (menu == SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU)
  {
    display.println(F("Temperature :"));
    // Afficher rectangle temperature.
  }

  else if (menu == SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU)
  {
    display.println(F("Luminosite :"));
    // Afficher rectangle luminosite.
  }

  else if (menu == SOFA_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    display.println(F("1. Test - 2. Test - 3. Test"));
  }

  else if (menu == BEDSIDE_LIGHT_CONTROL_SUBMENU)
  {
    display.println(F("1. Temperature - 2. Couleur - 3. Luminosite - 4. Effet"));
  }

  else if (menu == BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU)
  {
    display.println(F("Temperature :"));
    // Afficher rectangle temperature.
  }

  else if (menu == BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU)
  {
    display.println(F("Couleur :"));
    // Afficher les 3 rectangles de la couleur.
  }

  else if (menu == BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU)
  {
    display.println(F("Luminosite :"));
    // Afficher rectangle luminosite.
  }

  else if (menu == BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    display.println(F("1. Test - 2. Test - 3. Test"));
  }

  else if (menu == RGB_STRIP_CONTROL_SUBMENU)
  {
    display.println(F("1. Couleur - 2. Effet"));
  }

  else if (menu == RGB_STRIP_COLOR_CONTROL_SUBMENU)
  {
    display.println(F("Couleur :"));
    // Afficher les 3 rectangles de la couleur.
  }

  else if (menu == RGB_STRIP_EFFECT_CONTROL_SUBMENU)
  {
    display.println(F("1. Multicolore - 2. Son reaction"));
  }

  else if (menu == DEVICES_MENU)
  {
    display.println(F("Menu peripheriques"));
  }

  else if (menu == TV_MENU)
  {
    display.println(F("Menu television"));
  }

  else if (menu == CONFIGURATION_MENU)
  {
    display.println(F("Menu configuration"));
  }

  else if (menu == ALARM_CODE_CONFIGURATION_MENU)
  {
    display.println(F("Code :"));
    display.println(alarmCode);
  }

  else if (menu == ALARM_CONFIGURATION_MENU)
  {
    display.println(F("1. Son alarme - 2. Ajouter carte - 3. Suppromer cartes"));
  }

  display.display();
  ScreenCurrentOnTime = millis();
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
  ScreenCurrentOnTime = millis();
}

void displaySheduler()
{
  if ((ScreenCurrentOnTime != 0) && ((millis() - ScreenCurrentOnTime) >= 4000))
  {
    ScreenCurrentOnTime = 0;
    display.clearDisplay();
    display.display();
  }
}