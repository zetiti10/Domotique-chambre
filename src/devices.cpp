/**
 * @file devices.cpp
 * @author Louis L
 * @brief Fonctions liées aux périphériques du système de domotique.
 * @version 1.1
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#define USE_IRREMOTE_HPP_AS_PLAIN_INCLUDE
#include <IRremote.hpp>

// Autres fichiers du programme.
#include <pinDefinitions.hpp>
#include <display.hpp>
#include <main.hpp>
#include <devices.hpp>
#include <alarm.hpp>
#include <pitches.hpp>
#include <LEDStrips.hpp>
#include <TimerFreeTone.h>

// Variables globales.
int volumePrecision = 5;

// Les fonction de contrôle ont un paramètre : O = éteindre - 1 = allumer - 2 = changer l'état.

void switchFan(int action)
{
    if (action == 0)
    {
        if (digitalRead(PIN_FAN_RELAY) == HIGH)
        {
            digitalWrite(PIN_FAN_RELAY, LOW);
            printDeviceState("fan", false);
        }
    }

    else if (action == 1)
    {
        if (digitalRead(PIN_FAN_RELAY) == LOW)
        {
            digitalWrite(PIN_FAN_RELAY, HIGH);
            printDeviceState("fan", true);
        }
    }

    else
    {
        if (digitalRead(PIN_FAN_RELAY) == LOW)
        {
            switchFan(1);
        }

        else
        {
            switchFan(0);
        }
    }
}

void switchLEDCube(int action)
{
    if (action == 0)
    {
        if (digitalRead(PIN_LED_CUBE_RELAY) == HIGH)
        {
            digitalWrite(PIN_LED_CUBE_RELAY, LOW);
            printDeviceState("cube", false);
        }
    }

    else if (action == 1)
    {
        if (digitalRead(PIN_LED_CUBE_RELAY) == LOW)
        {
            digitalWrite(PIN_LED_CUBE_RELAY, HIGH);
            printDeviceState("cube", true);
        }
    }

    else
    {
        if (digitalRead(PIN_LED_CUBE_RELAY) == LOW)
        {
            switchLEDCube(1);
        }

        else
        {
            switchLEDCube(0);
        }
    }
}

void switchTray(int action)
{
    if (action == 0)
    {
        if (trayIsOpen == true)
    {
        digitalWrite(PIN_POWER_SUPPLY, LOW);
        digitalWrite(PIN_MOTOR_TRAY_1, LOW);
        digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
        for (int i = 0; i < 11; i++)
        {
            display.drawLine(30, 10, 98, 10, SSD1306_WHITE);
            display.drawLine(45, 10 + i, 83, 10 + i, SSD1306_WHITE);
        }
        display.display();
        for (int i = 0; i < 12; i++)
        {
            display.drawLine(30, 10, 98, 10, SSD1306_WHITE);
            display.drawLine(45, 22 - i, 83, 22 - i, SSD1306_BLACK);
            display.display();
            delay(80);
        }
        ScreenCurrentOnTime = ScreenOnTime;
        digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
        digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
        trayIsOpen = false;
        powerSupplyControl();
    }
    }

    else if (action == 1)
    {
        if (trayIsOpen == false)
    {
        digitalWrite(PIN_POWER_SUPPLY, LOW);
        digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
        digitalWrite(PIN_MOTOR_TRAY_2, LOW);
        display.clearDisplay();
        for (int i = 0; i < 11; i++)
        {
            display.drawLine(30, 10, 98, 10, SSD1306_WHITE);
            display.drawLine(45, 10 + i, 83, 10 + i, SSD1306_WHITE);
            display.display();
            delay(80);
        }
        ScreenCurrentOnTime = ScreenOnTime;
        digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
        digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
        trayIsOpen = true;
        powerSupplyControl();
    }
    }

    else
    {
        if (trayIsOpen == false)
        {
            switchTray(1);
        }

        else
        {
            switchTray(0);
        }
    }
}

void switchMulticolor(int action)
{
    if (action == 0)
    {
        if (multicolorMode == true)
        {
            stopMulticolor();
        }
    }

    else if (action == 1)
    {
        if (multicolorMode == false)
        {
            multicolorMode = true;
            printDeviceState("multicolor", true);
        }
    }

    else
    {
        if (multicolorMode == false)
        {
           switchMulticolor(1);
        }

        else
        {
            switchMulticolor(0);
        }
    }
}

void switchSoundReact(int action)
{
    if (action == 0)
    {
        if (soundReactMode == true)
        {
            stopSoundReact();
        }
    }

    else if (action == 1)
    {
        if (soundReactMode == false)
        {
            soundReactMode = true;
            printDeviceState("SoundReact", true);
        }
    }

    else
    {
        if (soundReactMode == false)
        {
            switchSoundReact(1);
        }

        else
        {
            switchSoundReact(0);
        }
    }
}

// Permet de faire bouger le servomoteur sans bibliothèque.
void moveDisplayServo(int pos)
{
    int pulseWidth = map(pos, 0, 180, 600, 2400);
    digitalWrite(PIN_SCREEN_SERVO, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(PIN_SCREEN_SERVO, LOW);
    delay(2);
}

// Effectue un clique avec le servomoteur sur le bouton ON/OFF de l'écran.
void switchDisplay()
{
    for (int pos = 80; pos <= 130; pos++)
    {
        moveDisplayServo(pos);
        delay(1);
    }

    for (int i = 0; i < 100; i++)
    {
        moveDisplayServo(130);
    }

    for (int pos = 130; pos >= 80; pos--)
    {
        moveDisplayServo(pos);
        delay(1);
    }
}

void switchTV(int action)
{
    if (action == 0)
    {
        if (digitalRead(PIN_TV_RELAY) == HIGH)
        {
            digitalWrite(PIN_TV_RELAY, LOW);
            printDeviceState("tv", false);
            switchDisplay();
            switchSono();
        }
    }

    else if (action == 1)
    {
        if (digitalRead(PIN_TV_RELAY) == LOW)
        {
            digitalWrite(PIN_TV_RELAY, HIGH);
            printDeviceState("tv", true);
            switchDisplay();
            switchSono();
        }
    }

    else
    {
        if (digitalRead(PIN_TV_RELAY) == LOW)
        {
            switchTV(1);
        }

        else
        {
            switchTV(0);
        }
    }
}

void switchAlarm(int action)
{
    if (action == 0)
    {
        if (alarmMode == true)
        {
            if (alarmIsRinging == true)
            {
                stopAlarme();
            }

            printAlarm(0);
            alarmMode = false;
            infoAlarme();
            yesSound();
        }
    }

    else if (action == 1)
    {
        if (alarmMode == false)
        {
            printAlarm(1);
            alarmMode = true;
            infoAlarme();
            yesSound();
        }
    }

    else
    {
        if (alarmMode == false)
        {
            switchAlarm(1);
        }

        else
        {
            switchAlarm(0);
        }
    }
}

// Paramètre :  0 = diminuer le volume - 1 = augmenter le volume - 2 = couper le son.
void volumeSono(int action)
{
    if (action == 0)
    {
        printVolume(0);
        IrSender.sendNEC(0x44C1, 0xC7, volumePrecision);
    }

    else if (action == 1)
    {
        printVolume(1);
        IrSender.sendNEC(0x44C1, 0x47, volumePrecision);
    }

    else
    {
        printVolume(2);
        IrSender.sendNEC(0x44C1, 0x77, 3);
    }
}

// Basculer l'état de la sono.
void switchSono()
{
    IrSender.sendNEC(0x44C1, 0x87, 3);
}

// Cette fonction sert à contrôler l'alimentation (pour l'allumer / l'éteindre) en fonction de si il y a des périphériques d'allumés ou non.
void powerSupplyControl()
{

    if (digitalRead(PIN_ALARM_RELAY) == HIGH || digitalRead(PIN_FAN_RELAY) == HIGH || digitalRead(PIN_LED_CUBE_RELAY) == HIGH || digitalRead(PIN_TV_RELAY) == HIGH || digitalRead(PIN_WARDROBE_LIGHTS_RELAY) == HIGH || digitalRead(PIN_RELAY_6) == HIGH || digitalRead(PIN_RELAY_7) == HIGH || digitalRead(PIN_RELAY_8) == HIGH || soundReactMode == true)
    {
        digitalWrite(PIN_POWER_SUPPLY, LOW);
    }

    else if (RLEDValue != 0 || GLEDValue != 0 || BLEDValue != 0 || powerSupplyDelayON > 0)
    {
        digitalWrite(PIN_POWER_SUPPLY, LOW);
    }

    else
    {
        digitalWrite(PIN_POWER_SUPPLY, HIGH);
    }
}

void stopEverything()
{
    powerSupplyDelayON = 0;
    switchFan(0);
    switchLEDCube(0);
    switchTV(0);
    switchTray(0);
    switchAlarm(0);
    switchMulticolor(0);
    switchSoundReact(0);
    display.clearDisplay();
    display.display();
    powerSupplyControl();
}

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