#ifndef MAIN_DEFINITIONS
#define MAIN_DEFINITIONS

#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

extern boolean bedroomDoorState;
extern boolean doorbellToSend;
extern boolean muteMode;
extern boolean trayIsOpen;
extern float temperature;
extern int microValue;
extern int microSensibility;
extern int microDelay;
extern long powerSupplyDelayON;
extern int button4Timer;

extern SoftwareSerial ESP8266;
extern Adafruit_SSD1306 display;

#endif