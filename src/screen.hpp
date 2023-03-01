#ifndef SCREEN_DEFINITIONS
#define SCREEN_DEFINITIONS

extern int ScreenOnTime;
extern int ScreenCurrentOnTime;

extern void printBell();
extern void printTemperature();
extern void printMicroSensibility();
extern void printLEDState();
extern void printDeviceState(String element, boolean on);
extern void printAllOFF();
extern void printKeypadMode(String mode);
extern void printPowerSupplyONTime();

#endif