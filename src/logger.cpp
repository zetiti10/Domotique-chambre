/**
 * @file logger.cpp
 * @author Louis L
 * @brief Fichier de gestion de la journalisation.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "logger.hpp"

// Variables globales.
boolean loggerEnabled = false;

void sendLogMessage(messageType type, String message)
{
    if (loggerEnabled)
    {
        String messageToSend;

        switch (type)
        {
        case INFO:
            messageToSend = "[I] ";
            messageToSend += message;
            break;

        case ERROR:
            messageToSend = "\033[1;31m[E] ";
            messageToSend += message;
            messageToSend += "\033[0m";
            break;

        case HIGHLIGHT:
            messageToSend = "\033[1;36m[E] ";
            messageToSend += message;
            messageToSend += "\033[0m";
            break;
        
        default:
            break;
        }

        Serial.println(messageToSend);
    }
}