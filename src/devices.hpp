#ifndef DEVICES_DEFINITIONS
#define DEVICES_DEFINITIONS

extern int volumePrecision;

void switchFan(int action);
void switchLEDCube(int action);
void switchTray(int action);
void switchMulticolor(int action);
void switchSoundReact(int action);
void switchTV(int action);
void switchAlarm(int action);
void volumeSono(int action);
void switchSono();
void powerSupplyControl();
void stopEverything();
void yesSound();
void noSound();
void doorbellMusic();

#endif