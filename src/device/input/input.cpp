/**
 * @file device/input/input.cpp
 * @author Louis L
 * @brief Classe mère commune à tous les capteurs du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "input.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
Input::Input(String friendlyName, int ID) : Device(friendlyName, ID) {}