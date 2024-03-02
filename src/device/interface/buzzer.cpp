/**
 * @file device/interface/buzzer.cpp
 * @author Louis L
 * @brief Classe représentant le buzzer du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <TimerFreeTone.h>

// Autres fichiers du programme.
#include "buzzer.hpp"
#include "../device.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche reliée au buzzer du système de domotique.
Buzzer::Buzzer(String friendlyName, int ID, int pin) : Device(friendlyName, ID), m_pin(pin) {}

/// @brief Initialise l'objet.
void Buzzer::setup()
{
    if (m_operational)
        return;

    pinMode(m_pin, OUTPUT);

    m_operational = true;
}

/// @brief Produit un court son matérialisant un clique du clavier.
void Buzzer::clickSound()
{
    TimerFreeTone(m_pin, 1000, 25, 8);
}

/// @brief Son de validation / de succès.
void Buzzer::yesSound()
{
    TimerFreeTone(m_pin, 800, 250);
}

/// @brief Son de refus / d'erreur.
void Buzzer::noSound()
{
    TimerFreeTone(m_pin, 200, 250);
}

/// @brief Musique de la sonnette.
void Buzzer::doorbellMusic()
{
    TimerFreeTone(m_pin, 200, 50);
    TimerFreeTone(m_pin, 400, 100);
    TimerFreeTone(m_pin, 600, 150);
    TimerFreeTone(m_pin, 800, 200);
}