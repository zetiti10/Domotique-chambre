/**
 * @file buzzer.cpp
 * @author Louis L
 * @brief Fonctions liées à l'émissions de tonalités par le buzzer.
 * @version 1.1
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <TimerFreeTone.h>

// Autres fichiers du programme.
#include <pinDefinitions.hpp>

void clickSound()
{
    TimerFreeTone(PIN_BUZZER, 750, 25, 8);
}

void yesSound()
{
    TimerFreeTone(PIN_BUZZER, 800, 250);
}

void noSound()
{
    TimerFreeTone(PIN_BUZZER, 200, 250);
}

void doorbellMusic()
{
    TimerFreeTone(PIN_BUZZER, 200, 250);
    TimerFreeTone(PIN_BUZZER, 400, 250);
    TimerFreeTone(PIN_BUZZER, 600, 250);
    TimerFreeTone(PIN_BUZZER, 800, 250);
}