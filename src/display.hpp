#ifndef SCREEN_DEFINITIONS
#define SCREEN_DEFINITIONS

#include <Arduino.h>

extern int ScreenOnTime;
extern int ScreenCurrentOnTime;

void printBell();
void printVolume(int element);
void printAlarm(int element);
void printAllOFF();
void printPowerSupplyONTime();
void printTemperature();
void printLEDState();
void printDeviceState(String element, boolean on);
void printKeypadMode(String mode);
void printMicroSensibility();
void printMulticolorSpeed();
void printVolumePrecision();

#endif