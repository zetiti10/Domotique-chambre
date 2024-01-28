#ifndef LOGGER_DEFINITIONS
#define LOGGER_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

extern boolean loggerEnabled;

// Liste des types de message disponibles.
enum messageType
{
    INFO,
    WARN,
    ERROR,
    HIGHLIGHT
};

void sendLogMessage(messageType type, String message);

#endif