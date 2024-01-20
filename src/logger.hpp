#ifndef LOGGER_DEFINITIONS
#define LOGGER_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

extern boolean loggerEnabled;

enum messageType
{
    INFO,
    ERROR,
    HIGHLIGHT
};

void sendLogMessage(messageType type, String message);

#endif