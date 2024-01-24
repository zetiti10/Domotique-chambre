/**
 * @file device/input/input.cpp
 * @author Louis L
 * @brief Classe mère commune à tous les capteurs du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "input.hpp"
#include "../device.hpp"

Input::Input(String friendlyName) : Device(friendlyName) {}