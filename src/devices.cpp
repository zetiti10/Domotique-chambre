/**
 * @file devices.cpp
 * @author Louis L
 * @brief Fonctions liées aux périphériques du système de domotique.
 * @version 1.0
 * @date 2023-03-01
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Autres fichiers du programme.
#include <pinDefinitions.hpp>
#include <screen.hpp>
#include <main.hpp>
#include <devices.hpp>
#include <alarm.hpp>
#include <pitches.hpp>
#include <LEDStrips.hpp>

// Les fonction de contrôle ont un paramètre : O = eteindre - 1 = allumer - 2 = changer l'etat.

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
            digitalWrite(PIN_FAN_RELAY, HIGH);
            printDeviceState("fan", true);
        }
        else
        {
            digitalWrite(PIN_FAN_RELAY, LOW);
            printDeviceState("fan", false);
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
            digitalWrite(PIN_LED_CUBE_RELAY, HIGH);
            printDeviceState("cube", true);
        }
        else
        {
            digitalWrite(PIN_LED_CUBE_RELAY, LOW);
            printDeviceState("cube", false);
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
        else
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
            multicolorMode = true;
            printDeviceState("multicolor", true);
        }
        else
        {
            stopMulticolor();
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
            soundReactMode = true;
            printDeviceState("SoundReact", true);
        }
        else
        {
            stopSoundReact();
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
    for(int i = 0; i < 50; i++) {
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
            digitalWrite(PIN_IR_LED_SIGNAL, LOW);
            delay(50);
            digitalWrite(PIN_IR_LED_SIGNAL, HIGH);
            switchDisplay();
        }
    }
    else if (action == 1)
    {
        if (digitalRead(PIN_TV_RELAY) == LOW)
        {
            digitalWrite(PIN_TV_RELAY, HIGH);
            printDeviceState("tv", true);
            digitalWrite(PIN_IR_LED_SIGNAL, LOW);
            delay(50);
            digitalWrite(PIN_IR_LED_SIGNAL, HIGH);
            switchDisplay();
        }
    }
    else
    {
        if (digitalRead(PIN_TV_RELAY) == LOW)
        {
            digitalWrite(PIN_TV_RELAY, HIGH);
            printDeviceState("tv", true);
            digitalWrite(PIN_IR_LED_SIGNAL, LOW);
            delay(50);
            digitalWrite(PIN_IR_LED_SIGNAL, HIGH);
            switchDisplay();
        }
        else
        {
            digitalWrite(PIN_TV_RELAY, LOW);
            printDeviceState("tv", false);
            digitalWrite(PIN_IR_LED_SIGNAL, LOW);
            delay(50);
            digitalWrite(PIN_IR_LED_SIGNAL, HIGH);
            switchDisplay();
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
            alarmMode = false;
            infoAlarme();
            yesSound();
        }
    }
    else if (action == 1)
    {
        if (alarmMode == false)
        {
            printDeviceState("alarme", true);
            alarmMode = true;
            infoAlarme();
            yesSound();
        }
    }
    else
    {
        if (alarmMode == false)
        {
            printDeviceState("alarme", true);
            alarmMode = true;
            infoAlarme();
            yesSound();
        }
        else
        {
            if (alarmMode == true)
            {
                if (alarmIsRinging == true)
                {
                    stopAlarme();
                }
                alarmMode = false;
                printDeviceState("alarme", false);
                infoAlarme();
                yesSound();
            }
        }
    }
}

// Paramètre :  0 = diminuer le volume - 1 = augmenter le volume.
void sonoVolume(int action)
{
    if (action == 0)
    {
        digitalWrite(PIN_IR_LED_SIGNAL, LOW);
        delay(100);
        digitalWrite(PIN_IR_LED_SIGNAL, HIGH);
    }
    else
    {
        digitalWrite(PIN_IR_LED_SIGNAL, LOW);
        delay(150);
        digitalWrite(PIN_IR_LED_SIGNAL, HIGH);
    }
}

// Cette fonction sert à contrôler l'alimentation (pour l'allumer / l'éteindre).
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

void stopEverything() {
    powerSupplyDelayON = 0;
    switchFan(0);
    switchLEDCube(0);
    switchTV(0);
    switchTray(0);
    switchAlarm(0);
    switchMulticolor(0);
    switchSoundReact(0);
    printAllOFF();
    powerSupplyControl();
}

void yesSound()
{
    tone(PIN_BUZZER, NOTE_G5, 1000);
}

void noSound()
{
    tone(PIN_BUZZER, NOTE_D4, 1000);
}

// Les deux fonctions suivantes sont utilisées pour produire la musique de la sonnette.
int music[] = {

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

void musique1()
{

    int divider = 0;
    int noteDuration = 0;

    for (unsigned long thisNote = 0; thisNote < (sizeof(music) / sizeof(music[0])); thisNote = thisNote + 2)
    {

        divider = music[thisNote + 1];

        if (divider > 0)
        {

            noteDuration = ((60000 * 4) / 180) / divider;
        }
        else if (divider < 0)
        {

            noteDuration = ((60000 * 4) / 180) / abs(divider);
            noteDuration *= 1.5;
        }

        tone(PIN_BUZZER, music[thisNote], noteDuration * 0.9);
        delay(noteDuration);
        noTone(PIN_BUZZER);
    }
}