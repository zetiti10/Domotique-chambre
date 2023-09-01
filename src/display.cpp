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

// Affichage de la cloche pour la sonnette.
void displayBell()
{
  display.clearDisplay();
  display.drawBitmap(0, 0, bellBitmap, 128, 32, 1);
  display.display();
  ScreenCurrentOnTime = millis();
}

// Affichage d'un message à l'écran.
void displayMessage(String title, String message)
{
  display.clearDisplay();

  display.setCursor(0, 0);
  display.setTextSize(2);
  display.print(title);

  display.setCursor(0, 18);
  display.setTextSize(1);
  display.print(message);

  display.display();
  ScreenCurrentOnTime = millis();
}

// Paramètre :  DECREASE = diminuer le volume - INCREASE = augmenter le volume - MUTE = couper le son - UNMUTE = réetablir le son.
void displayVolume(int action)
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
void displayAlarmTriggered(int element)
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

// Affichage de la température et de l'humidité.
void displayAirValues()
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

// Affiche 3 gauges correspondant à la luminosité du rouge, vert et bleu des rubans de DEL.
void displayLEDState()
{
  display.clearDisplay();
  display.setTextSize(2);

  display.setCursor(22, 0);
  display.print("R");
  int rGauge = int(map(RLEDValue, 0, 255, 64, 16));
  display.drawLine(25, 64, 25, rGauge, WHITE);
  display.drawRect(24, 15, 3, 50, WHITE);

  display.setCursor(62, 0);
  display.print("V");
  int gGauge = int(map(GLEDValue, 0, 255, 64, 16));
  display.drawLine(65, 64, 65, gGauge, WHITE);
  display.drawRect(64, 15, 3, 50, WHITE);

  display.setCursor(101, 0);
  display.print("B");
  int bGauge = int(map(BLEDValue, 0, 255, 64, 16));
  display.drawLine(102, 64, 102, bGauge, WHITE);
  display.drawRect(101, 15, 3, 50, WHITE);

  display.display();
  ScreenCurrentOnTime = millis();
}

// Affiche une animation d'allumage / arrêt d'un périphérique.
void displayDeviceState(boolean on)
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

// Affiche le menu actuel.
void displayKeypadMenu()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);

  switch (keypadMenu)
  {
  case LIGHTS_MENU:
    display.drawBitmap(0, 0, lightsMenuBitmap, 128, 64, WHITE);
    display.setCursor(20, 55);
    display.print("Menu : lumieres");
    break;

  case SOFA_LIGHT_CONTROL_SUBMENU:
    display.print("Lampe canape");

    display.setCursor(0, 15);
    display.println("1. Temperature");
    display.println("2. Luminosite");
    display.print("3. Effet");
    break;

  case SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU:
    display.print("Temperature : PAS CONFIGURE");
    // Afficher rectangle temperature.
    break;

  case SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU:
    display.print("Luminosite : PAS CONFIGURE");
    // Afficher rectangle luminosite.
    break;

  case SOFA_LIGHT_EFFECT_CONTROL_SUBMENU:
    display.print("1. Test - 2. Test - 3. Test PAS CONFIGURE");
    break;

  case BEDSIDE_LIGHT_CONTROL_SUBMENU:
    display.print("Lampe de chevet");

    display.setCursor(0, 15);
    display.println("1. Temperature");
    display.println("2. Couleur");
    display.println("3. Luminosite");
    display.print("4. Effet");
    break;

  case BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU:
    display.print("Temperature : PAS CONFIGURE");
    // Afficher rectangle temperature.
    break;

  case BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU:
    display.print("Couleur : PAS CONFIGURE");
    // Afficher les 3 rectangles de la couleur.
    break;

  case BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU:
    display.print("Luminosite : PAS CONFIGURE");
    // Afficher rectangle luminosite.
    break;

  case BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU:
    display.print("1. Test - 2. Test - 3. Test PAS CONFIGURE");
    break;

  case RGB_STRIP_CONTROL_SUBMENU:
    display.print("Rubans de DEL");

    display.setCursor(0, 15);
    display.println("1. Couleur");
    display.print("2. Effet");
    break;

  case RGB_STRIP_COLOR_CONTROL_SUBMENU:
    displayLEDState();
    break;

  case RGB_STRIP_EFFECT_CONTROL_SUBMENU:
    display.print("Effet");

    display.setCursor(0, 15);
    display.println("1. Multicolore");
    display.print("2. Son reaction");
    break;

  case DEVICES_MENU:
    display.drawBitmap(0, 0, devicesMenuBitmap, 128, 64, WHITE);
    display.setCursor(4, 55);
    display.print("Menu : peripheriques");
    break;

  case TV_MENU:
    display.drawBitmap(0, 0, TVMenuBitmap, 128, 64, WHITE);
    display.setCursor(15, 55);
    display.print("Menu : television");
    break;

  case CONFIGURATION_MENU:
    display.drawBitmap(0, 0, configurationMenuBitmap, 128, 64, WHITE);
    display.setCursor(5, 55);
    display.print("Menu : configuration");
    break;

  case ALARM_CODE_CONFIGURATION_SUBMENU:
    display.print("Entrez le mot de passe pour acceder au menu de gestion de l'alarme.");
    break;

  case ALARM_CONFIGURATION_SUBMENU:
    display.print("Alarme");

    display.setCursor(0, 15);
    display.println("1. Son de l'alarme");
    display.println("2. Ajouter une carte");
    display.print("3. Suprimer les cartes");
    break;

  default:
    break;
  }

  display.display();
  ScreenCurrentOnTime = millis();
}

void displayMulticolorSpeed()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Vitesse de l'animation multicolore : ");
  display.print(multicolorSpeed);
  display.display();
  ScreenCurrentOnTime = millis();
}

void displaySoundReactSensibility()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Sensibilite de l'animation son reaction : ");
  display.print(soundReactSensibility);
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