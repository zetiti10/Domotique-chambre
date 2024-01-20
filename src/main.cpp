/**
 * @file main.cpp
 * @author Louis L
 * @brief Fichier principal du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "devices/binaryDevice.hpp"

// Instanciation des périphériques du système.
BinaryDevice LEDCube(25);

void setup()
{
    Serial.begin(115200);

    LEDCube.setup();
}

void loop()
{
    LEDCube.turnOn();

    delay(5000);

    LEDCube.toggle();

    delay(5000);
}