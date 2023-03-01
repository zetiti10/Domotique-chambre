/**
 * @file screen.cpp
 * @author Louis L
 * @brief Fonctions d'affichage d'éléments sur l'écran OLED du boîtier de contrôle.
 * @version 1.0
 * @date 2023-03-01
 */

// Autres fichiers du programme.
#include <main.hpp>
#include <LEDStrips.hpp>

int ScreenOnTime = 4000;
int ScreenCurrentOnTime = 0;

#define LOGO2_HEIGHT 16
#define LOGO2_WIDTH 16
static const unsigned char PROGMEM logoBell[] = {0b00000000, 0b00000000,
                                                 0b00000001, 0b10000000,
                                                 0b00000111, 0b11100000,
                                                 0b00001111, 0b11110000,
                                                 0b00001111, 0b11110000,
                                                 0b00001111, 0b11110000,
                                                 0b00001111, 0b11110000,
                                                 0b00001111, 0b11110000,
                                                 0b00001111, 0b11110000,
                                                 0b00011111, 0b11111000,
                                                 0b00011111, 0b11111000,
                                                 0b00111111, 0b11111100,
                                                 0b01111111, 0b11111110,
                                                 0b00000011, 0b11000000,
                                                 0b00000001, 0b10000000,
                                                 0b00000000, 0b00000000};

void printBell()
{
  display.clearDisplay();
  display.drawBitmap(
      (display.width() - LOGO2_WIDTH) / 2,
      (display.height() - LOGO2_HEIGHT) / 2,
      logoBell, LOGO2_WIDTH, LOGO2_HEIGHT, 1);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

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

void printMicroSensibility()
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(15, 10);
  display.println(microSensibility);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}

void printLEDState()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(23, 0);
  display.println(F("R"));
  int rrr = int(map(RLEDValue, 0, 255, 14, 54));
  display.drawLine(25, 14, 25, rrr, SSD1306_WHITE);
  display.setCursor(63, 0);
  display.println(F("G"));
  int ggg = int(map(RLEDValue, 0, 255, 14, 54));
  display.drawLine(65, 14, 65, ggg, SSD1306_WHITE);
  display.setCursor(102, 0);
  display.println(F("B"));
  int bbb = int(map(RLEDValue, 0, 255, 14, 54));
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

void printAllOFF()
{
  display.clearDisplay();
  display.setTextSize(3);
  display.setCursor(0, 5);
  display.println(F("ALL OFF"));
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

void printPowerSupplyONTime()
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(20, 0);
  display.println(F("Temps restant :"));
  display.setTextSize(3);
  display.setCursor(30, 10);
  int tempsminutes = powerSupplyDelayON / 60000;
  display.println(tempsminutes);
  display.display();
  ScreenCurrentOnTime = ScreenOnTime;
}