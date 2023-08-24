#ifndef DEVICES_DEFINITIONS
#define DEVICES_DEFINITIONS

#define SWITCH_OFF 0
#define CLOSE 0
#define DECREASE 0
#define SWITCH_ON 1
#define OPEN 1
#define INCREASE 1
#define TOGGLE 2
#define MUTE 2
#define UNMUTE 3
#define STOP_RINGING 3
#define TOGGLE_MUTE 4

extern int volumePrecision;
extern int RGBStripPrecision;
extern double soundReactSensibility;
extern int multicolorSpeed;

extern int RLEDValue;
extern int GLEDValue;
extern int BLEDValue;

extern boolean multicolorState;

void switchDisco(int action);
void switchLEDCube(int action);
void switchStreet(int action);
void switchDeskLight(int action);
void switchTray(int action);
void switchRGBStrip(int action);
void controlRGBStrip(int r, int g, int b);
void switchMulticolor(int action);
void multicolorScheduler();
void switchSoundReact(int action);
void soundReactScheduler();
void volumeSono(int action);
void switchTV(int action);
void switchAlarm(int action);
void alarmSheduler();


#endif