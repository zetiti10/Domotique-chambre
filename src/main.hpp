#ifndef MAIN_DEFINITIONS
#define MAIN_DEFINITIONS

#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

extern boolean muteMode;
extern float temperature;
extern int microSensibility;
extern int microDelay;
extern long powerSupplyDelayON;
extern int button4Timer;

extern SoftwareSerial ESP8266;
extern Adafruit_SSD1306 display;
extern PN532 nfcReader;

#endif