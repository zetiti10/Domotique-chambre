#ifndef MAIN_DEFINITIONS
#define MAIN_DEFINITIONS

#include <Adafruit_SSD1306.h>
#include <PN532.h>

#define STORED_CARD_COUNTER_STORAGE_LOCATION 0
#define ALARM_BUZZER_STATE_STORAGE_LOCATION 1
#define VOLUME_STORAGE_LOCATION 5
#define MULTICOLOR_ANIMATION_SPEED_STORAGE_LOCATION 3
#define SOUND_REACT_ANIMATION_SENSIBILITY_STORAGE_LOCATION 4

extern unsigned long cardCounter;

extern boolean debugMode;
extern float temperature;
extern float humidity;

extern Adafruit_SSD1306 display;
extern PN532 nfcReader;

#endif