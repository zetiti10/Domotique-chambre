#ifndef ALARM_DEFINITIONS
#define ALARM_DEFINITIONS

extern boolean alarmMode;
extern boolean alarmIsRinging;
extern int refusalCounter;
extern int intrusionCounter;

void stopAlarme();
void allarmeAutoOff();
void infoAlarme();

#endif