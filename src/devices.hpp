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

extern boolean discoState;
extern boolean LEDCubeState;
extern boolean streetState;
extern boolean deskLightState;
extern boolean trayState;
extern boolean RGBStripState;
extern boolean soundReactState;
extern boolean multicolorState;
extern boolean TVState;
extern boolean alarmState;

extern double soundReactSensibility;

extern unsigned long multicolorSpeed;

extern int RLEDValue;
extern int GLEDValue;
extern int BLEDValue;
extern int RGBStripPrecision;

extern int volume;

extern boolean alarmBuzzerState;
extern boolean cardToStoreState;

void switchDisco(int action, boolean displayState);
void switchLEDCube(int action, boolean displayState);
void switchStreet(int action, boolean displayState);
void switchDeskLight(int action, boolean displayState);
void switchTray(int action, boolean displayState);
void switchRGBStrip(int action, boolean displayState);
void controlRGBStrip(int r, int g, int b);
void switchMulticolor(int action, boolean displayState);
void multicolorScheduler();
void switchSoundReact(int action, boolean displayState);
void soundReactScheduler();
void volumeSono(int action, boolean displayState);
void switchTV(int action, boolean displayState);
void switchAlarm(int action, boolean displayState);
void alarmSheduler();
void storeCard(uint8_t card[4]);
void removeCards();

#endif