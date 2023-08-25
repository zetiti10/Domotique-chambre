#ifndef SCREEN_DEFINITIONS
#define SCREEN_DEFINITIONS

#include <Arduino.h>

extern int ScreenOnTime;
extern unsigned long ScreenCurrentOnTime;

void printBell();
void printVolume(int element);
void printAlarm(int element);
void printAllOFF();
void printPowerSupplyONTime();
void printTemperature();
void printLEDState();
void printDeviceState(boolean on);
void printKeypadMenu(int menu);
void printMicroSensibility();
void printMulticolorSpeed();
void printVolumePrecision();
void displaySheduler();

#endif