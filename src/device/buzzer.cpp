/**
 * @file device/buzzer.cpp
 * @author Louis L
 * @brief Classe représentant le buzzer du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>
#include <TimerFreeTone.h>

// Autres fichiers du programme.
#include "buzzer.hpp"

Buzzer::Buzzer(String friendlyName, int pin): Device(friendlyName), m_pin(pin) {}

void Buzzer::setup()
{
    pinMode(m_pin, OUTPUT);
}

void Buzzer::clickSound()
{
    TimerFreeTone(m_pin, 1000, 25, 8);
}

// Son de validation / de succès.
void Buzzer::yesSound()
{
    TimerFreeTone(m_pin, 800, 250);
}

// Son de refus / d'erreur.
void Buzzer::noSound()
{
    TimerFreeTone(m_pin, 200, 250);
}

// Musique de la sonnette.
void Buzzer::doorbellMusic()
{
    TimerFreeTone(m_pin, 200, 50);
    TimerFreeTone(m_pin, 400, 100);
    TimerFreeTone(m_pin, 600, 150);
    TimerFreeTone(m_pin, 800, 200);
}