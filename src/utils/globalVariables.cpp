/**
 * @file utils/globalVariables.cpp
 * @author Louis L
 * @brief Fichier contenant les variables globales du système.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "globalVariables.hpp"

// Variables liées à l'arrêt du système.
bool systemToRestart = false;
bool systemToShutdown = false;
bool powerSupplyToShutdown = false;

// Variables liées à la surveillance du système.
unsigned long TPSMeasureStartTime = 0;
unsigned int TPSCounter = 0;
unsigned int TPS = 0;