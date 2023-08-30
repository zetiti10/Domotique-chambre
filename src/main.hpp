#ifndef MAIN_DEFINITIONS
#define MAIN_DEFINITIONS

#include <Adafruit_SSD1306.h>
#include <PN532.h>

extern boolean debugMode;

extern unsigned long cardCounter;

extern float temperature;
extern float humidity;

extern Adafruit_SSD1306 display;
extern PN532 nfcReader;

#endif