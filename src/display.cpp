/**
 * @file display.cpp
 * @author Louis L
 * @brief Fonctions d'affichage d'éléments sur l'écran OLED du boîtier de contrôle.
 * @version 1.1
 * @date 2023-03-01
 */

// Autres fichiers du programme.
#include <main.hpp>
#include <bitmaps.hpp>
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

// Paramètre :  DECREASE = diminuer le volume - INCREASE = augmenter le volume - MUTE = couper le son - UNMUTE = réetablir le son.
void printVolume(int action)
{
  display.clearDisplay();

  if (action == DECREASE)
  {
    display.drawBitmap(0, 0, volumeDecreaseBitmap, 128, 64, WHITE);
  }

  else if (action == INCREASE)
  {
    display.drawBitmap(0, 0, volumeIncreaseBitmap, 128, 64, WHITE);
  }

  else if (action == MUTE)
  {
    display.drawBitmap(0, 0, muteBitmap, 128, 64, WHITE);
  }

  else if (action == UNMUTE)
  {
    display.drawBitmap(0, 0, unmuteBitmap, 128, 64, WHITE);
  }

  if (action == DECREASE || action == INCREASE || action == UNMUTE)
  {
    display.drawRect(50, 52, 27, 3, WHITE);

    if (volume > 0)
    {
      display.drawLine(51, 53, 51 + volume, 53, WHITE);
    }

    display.setTextSize(1);
    display.setCursor(80, 52);
    display.print(volume);
  }

  display.display();
  ScreenCurrentOnTime = millis();
}

// Paramètre :  0 = l'alarme sonne (normal) - 1 = l'alarme sonne (inversé).
void printAlarm(int element)
{
  display.clearDisplay();

  if (element == 0)
  {
    display.drawBitmap(0, 0, alarmTriggeredBitmap, 128, 64, 1);
    display.invertDisplay(false);
  }

  else if (element == 1)
  {
    display.drawBitmap(0, 0, alarmTriggeredBitmap, 128, 64, 1);
    display.invertDisplay(true);
  }

  display.display();
  ScreenCurrentOnTime = millis();
}

// Affichage de diverses informations.
void printAir()
{
  display.clearDisplay();
  display.drawBitmap(0, 0, airBitmap, 128, 64, WHITE);
  display.setTextSize(2);
  display.setCursor(40, 42);
  display.print(temperature);
  display.write(0xF8);
  display.print("C");
  display.setCursor(40, 10);
  display.print(humidity);
  display.write(0x25);
  display.display();
  ScreenCurrentOnTime = millis();
}

void printLEDState()
{
  display.clearDisplay();
  display.setTextSize(2);

  display.setCursor(22, 0);
  display.print("R");
  int rGauge = int(map(RLEDValue, 0, 255, 14, 64));
  display.drawLine(25, 14, 25, rGauge, WHITE);

  display.setCursor(62, 0);
  display.print("V");
  int gGauge = int(map(GLEDValue, 0, 255, 14, 64));
  display.drawLine(65, 14, 65, gGauge, WHITE);

  display.setCursor(101, 0);
  display.print("B");
  int bGauge = int(map(BLEDValue, 0, 255, 14, 64));
  display.drawLine(102, 14, 102, bGauge, WHITE);

  display.display();
  ScreenCurrentOnTime = millis();
}

void printDeviceState(boolean on)
{
  display.clearDisplay();

  if (on == true)
  {
    for (int i = 51; i < 75; i += 4)
    {
      display.clearDisplay();
      display.drawRoundRect(41, 14, 46, 24, 12, WHITE);
      display.fillCircle(i, 26, 8, WHITE);
      display.display();
    }
  }

  else
  {
    for (int i = 75; i > 51; i -= 4)
    {
      display.clearDisplay();
      display.drawRoundRect(41, 14, 46, 24, 12, WHITE);
      display.drawCircle(i, 26, 8, WHITE);
      display.display();
    }
  }

  ScreenCurrentOnTime = millis();
}

void printKeypadMenu(int menu)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (menu == LIGHTS_MENU)
  {
    display.drawBitmap(0, 0, lightsMenuBitmap, 128, 64, WHITE);
    display.setCursor(20, 55);
    display.print("Menu : lumieres");
  }

  else if (menu == SOFA_LIGHT_CONTROL_SUBMENU)
  {
    display.setCursor(0, 0);
    display.print("Lampe canape");

    display.setCursor(0, 15);
    display.setTextSize(1);
    display.println("1. Temperature");
    display.println("2. Luminosite");
    display.println("3. Effet");
  }

  else if (menu == SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU)
  {
    display.println(F("Temperature : PAS CONFIGURE"));
    // Afficher rectangle temperature.
  }

  else if (menu == SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU)
  {
    display.println(F("Luminosite : PAS CONFIGURE"));
    // Afficher rectangle luminosite.
  }

  else if (menu == SOFA_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    display.println(F("1. Test - 2. Test - 3. Test PAS CONFIGURE"));
  }

  else if (menu == BEDSIDE_LIGHT_CONTROL_SUBMENU)
  {
    display.setCursor(0, 0);
    display.print("Lampe de chevet");

    display.setCursor(0, 15);
    display.setTextSize(1);
    display.println("1. Temperature");
    display.println("2. Couleur");
    display.println("3. Luminosite");
    display.println("4. Effet");
  }

  else if (menu == BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU)
  {
    display.println(F("Temperature : PAS CONFIGURE"));
    // Afficher rectangle temperature.
  }

  else if (menu == BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU)
  {
    display.println(F("Couleur : PAS CONFIGURE"));
    // Afficher les 3 rectangles de la couleur.
  }

  else if (menu == BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU)
  {
    display.println(F("Luminosite : PAS CONFIGURE"));
    // Afficher rectangle luminosite.
  }

  else if (menu == BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU)
  {
    display.println(F("1. Test - 2. Test - 3. Test PAS CONFIGURE"));
  }

  else if (menu == RGB_STRIP_CONTROL_SUBMENU)
  {
    display.setCursor(0, 0);
    display.print("Rubans de DEL");

    display.setCursor(0, 15);
    display.setTextSize(1);
    display.println("1. Couleur");
    display.println("2. Effet");
  }

  else if (menu == RGB_STRIP_COLOR_CONTROL_SUBMENU)
  {
    printLEDState();
  }

  else if (menu == RGB_STRIP_EFFECT_CONTROL_SUBMENU)
  {
    display.println(F("1. Multicolore - 2. Son reaction"));
  }

  else if (menu == DEVICES_MENU)
  {
    display.drawBitmap(0, 0, devicesMenuBitmap, 128, 64, WHITE);
    display.setCursor(12, 55);
    display.print("Menu : peripheriques");
  }

  else if (menu == TV_MENU)
  {
    display.drawBitmap(0, 0, TVMenuBitmap, 128, 64, WHITE);
    display.setCursor(18, 55);
    display.print("Menu : television");
  }

  else if (menu == CONFIGURATION_MENU)
  {
    display.drawBitmap(0, 0, configurationMenuBitmap, 128, 64, WHITE);
    display.setCursor(15, 55);
    display.print("Menu : configuration");
  }

  else if (menu == ALARM_CODE_CONFIGURATION_MENU)
  {
    display.setCursor(0, 0);
    display.print("Mot de passe");
    display.print(alarmCode);
  }

  else if (menu == ALARM_CONFIGURATION_MENU)
  {
    display.println(F("1. Son alarme - 2. Ajouter carte - 3. Supprimer cartes"));
  }

  display.display();
  ScreenCurrentOnTime = millis();
}

void printMulticolorSpeed()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Vitesse de l'animation multicolore : ");
  display.println(multicolorSpeed);
  display.display();
  ScreenCurrentOnTime = millis();
}

void displaySheduler()
{
  if ((ScreenCurrentOnTime != 0) && ((millis() - ScreenCurrentOnTime) >= 8000))
  {
    ScreenCurrentOnTime = 0;
    display.clearDisplay();
    display.display();
  }
}