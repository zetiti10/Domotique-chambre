#ifndef READ_PROGMEM_STRING_DEFINITIONS
#define READ_PROGMEM_STRING_DEFINITIONS

// Ajout des bibliothèques au programme.
#include <Arduino.h>

/// @brief Fonction permettant de lire facilement une chaîne de caractères depuis la mémoire flash de l'Arduino.
/// @param progmemStr Le pointeur vers la chaîne de caractères. La longueur maximale de la chaîne est de `127` caractères.
/// @return Un `String` de la chaîne de caractères.
String readProgmemString(const char *progmemStr);

#endif