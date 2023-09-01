#ifndef SCREEN_DEFINITIONS
#define SCREEN_DEFINITIONS

#include <Arduino.h>

extern unsigned long ScreenCurrentOnTime;

void displayBell();
void displayMessage(String title, String message);
void displayVolume(int element);
void displayAlarmTriggered(int element);
void displayAirValues();
void displayLEDState();
void displayDeviceState(boolean on);
void displayKeypadMenu();
void displayMulticolorSpeed();
void displaySoundReactSensibility();
void displaySheduler();

#endif