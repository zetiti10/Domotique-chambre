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
#include <pitches.hpp>

void yesSound()
{
    TimerFreeTone(PIN_BUZZER, NOTE_G5, 500);
}

void noSound()
{
    TimerFreeTone(PIN_BUZZER, NOTE_D4, 500);
}

// Les deux fonctions suivantes sont utilisées pour produire la musique de la sonnette.
int doorbellMusicNotes[] = {
    NOTE_E5,
    8,
    NOTE_D5,
    8,
    NOTE_FS4,
    4,
    NOTE_GS4,
    4,
    NOTE_CS5,
    8,
    NOTE_B4,
    8,
    NOTE_D4,
    4,
    NOTE_E4,
    4,
    NOTE_B4,
    8,
    NOTE_A4,
    8,
    NOTE_CS4,
    4,
    NOTE_E4,
    4,
    NOTE_A4,
    2,
};

void doorbellMusic()
{

    int divider = 0;
    int noteDuration = 0;

    for (unsigned long thisNote = 0; thisNote < (sizeof(doorbellMusicNotes) / sizeof(doorbellMusicNotes[0])); thisNote = thisNote + 2)
    {

        divider = doorbellMusicNotes[thisNote + 1];

        if (divider > 0)
        {

            noteDuration = ((60000 * 4) / 180) / divider;
        }

        else if (divider < 0)
        {

            noteDuration = ((60000 * 4) / 180) / abs(divider);
            noteDuration *= 1.5;
        }

        TimerFreeTone(PIN_BUZZER, doorbellMusicNotes[thisNote], noteDuration * 0.9);
    }
}