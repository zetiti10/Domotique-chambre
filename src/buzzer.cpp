/**
 * @file buzzer.cpp
 * @author Louis L
 * @brief Fonctions liées à l'émissions de tonalités par le buzzer.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <TimerFreeTone.h>

// Autres fichiers du programme.
#include "pinDefinitions.hpp"

// Court son signifiant que le clique a bien été pris en compte.
void clickSound()
{
    TimerFreeTone(PIN_BUZZER, 1000, 25, 8);
}

// Son de validation / de succès.
void yesSound()
{
    TimerFreeTone(PIN_BUZZER, 800, 250);
}

// Son de refus / d'erreur.
void noSound()
{
    TimerFreeTone(PIN_BUZZER, 200, 250);
}

// Musique de la sonnette.
void doorbellMusic()
{
    TimerFreeTone(PIN_BUZZER, 200, 50);
    TimerFreeTone(PIN_BUZZER, 400, 100);
    TimerFreeTone(PIN_BUZZER, 600, 150);
    TimerFreeTone(PIN_BUZZER, 800, 200);
}