/**
 * @file utils/television.cpp
 * @author Louis L
 * @brief Fonction de lecture d'une chaîne de caractère stockée dans la mémoire du programme.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "readPROGMEMString.hpp"

/// @brief Fonction permettant de lire facilement une chaîne de caractères depuis la mémoire flash de l'Arduino.
/// @param progmemStr Le pointeur vers la chaîne de caractères. La longueur maximale de la chaîne est de `127` caractères.
/// @return Un `String` de la chaîne de caractères.
String readProgmemString(const char *progmemStr)
{
    int bufferSize = 128;
    char buffer[bufferSize];
    strncpy_P(buffer, progmemStr, bufferSize);
    buffer[bufferSize - 1] = '\0';
    return String(buffer);
}