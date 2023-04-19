#ifndef LED_STRIPS_DEFINITIONS
#define LED_STRIPS_DEFINITIONS

extern int LEDPowerSteps;
extern int RLEDValue;
extern int GLEDValue;
extern int BLEDValue;
extern int RLEDValueSave;
extern int GLEDValueSave;
extern int BLEDValueSave;
extern boolean multicolorMode;
extern boolean soundReactMode;
extern int multicolorCounterR;
extern int multicolorCounterG;
extern int multicolorCounterB;
extern int multicolorSpeed;

void multicolor();
void stopMulticolor();
void soundReact();
void stopSoundReact();

#endif