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
#include <PN532_HSU.h>
#include <PN532.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include <pinDefinitions.hpp>
#include <display.hpp>
#include <main.hpp>
#include <devices.hpp>
#include <TimerFreeTone.h>
#include <buzzer.hpp>

boolean discoState = false;
boolean LEDCubeState = false;
boolean streetState = false;
boolean deskLightState = false;
boolean trayState = false;
boolean RGBStripState = false;
boolean soundReactState = false;
boolean multicolorState = false;
boolean TVState = false;
boolean alarmState = false;

unsigned long soundReactLastTimeColorChanged = 0;
unsigned long soundReactLastTime = 0;
double soundReactSensibility = 0.75;
int soundReactMax = 0;

unsigned long multicolorSpeed = 10;
unsigned long multicolorLastTime = 0;
int multicolorStep = 0;

int RLEDValue = 0;
int GLEDValue = 0;
int BLEDValue = 0;
int RLEDOFFValue = 0;
int GLEDOFFValue = 0;
int BLEDOFFValue = 0;
int RGBStripPrecision = 20;

boolean volumeMuted = false;
int volume = 0;

unsigned long alarmAutoTriggerOFFCounter = 0;
unsigned long alarmTriggeredLightsCounter = 0;
boolean alarmTriggered = false;
boolean alarmBuzzerState = false;
boolean cardToStoreState = false;

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchDisco(int action, boolean displayState)
{
    if (action == SWITCH_OFF && discoState)
    {
        digitalWrite(PIN_DISCO_RELAY, LOW);
        discoState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !discoState)
    {
        digitalWrite(PIN_DISCO_RELAY, HIGH);
        discoState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (discoState == false)
            switchDisco(SWITCH_ON, displayState);

        else
            switchDisco(SWITCH_OFF, displayState);
    }
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchLEDCube(int action, boolean displayState)
{
    if (action == SWITCH_OFF && LEDCubeState)
    {
        digitalWrite(PIN_LED_CUBE_RELAY, LOW);
        LEDCubeState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !LEDCubeState)
    {
        digitalWrite(PIN_LED_CUBE_RELAY, HIGH);
        LEDCubeState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!LEDCubeState)
            switchLEDCube(SWITCH_ON, displayState);

        else
            switchLEDCube(SWITCH_OFF, displayState);
    }
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchStreet(int action, boolean displayState)
{
    if (action == SWITCH_OFF && streetState)
    {
        digitalWrite(PIN_STREET_RELAY, LOW);
        streetState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !streetState)
    {
        digitalWrite(PIN_STREET_RELAY, HIGH);
        streetState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!streetState)
            switchStreet(SWITCH_ON, displayState);

        else
            switchStreet(SWITCH_OFF, displayState);
    }
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchDeskLight(int action, boolean displayState)
{
    if (action == SWITCH_OFF && deskLightState)
    {
        digitalWrite(PIN_DESK_LIGHT_RELAY, LOW);
        deskLightState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !deskLightState)
    {
        digitalWrite(PIN_DESK_LIGHT_RELAY, HIGH);
        deskLightState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!deskLightState)
            switchDeskLight(SWITCH_ON, displayState);

        else
            switchDeskLight(SWITCH_OFF, displayState);
    }
}

// Paramètre :  CLOSE = fermet le plateau - OPEN = ouvrir le plateau - TOGGLE = basculer le plateau.
void switchTray(int action, boolean displayState)
{

    if (alarmState)
        return;

    if (action == CLOSE && trayState)
    {
        analogWrite(PIN_TRAY_MOTOR_SPEED, 140);
        digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
        digitalWrite(PIN_MOTOR_TRAY_2, LOW);

        display.fillRoundRect(5, -10, 118, 20, 5, WHITE);
        for (int i = 0; i < 40; i++)
        {
            display.drawLine(28, 11 + i, 100, 11 + i, WHITE);
        }
        if (displayState)
            display.display();

        for (int i = 0; i < 40; i++)
        {
            display.drawLine(28, 50 - i, 100, 50 - i, BLACK);
            if (displayState)
                display.display();
            delay(15);
        }

        if (displayState)
            ScreenCurrentOnTime = millis();

        digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
        digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
        digitalWrite(PIN_TRAY_MOTOR_SPEED, LOW);
        trayState = false;
    }

    else if (action == OPEN && !trayState)
    {
        analogWrite(PIN_TRAY_MOTOR_SPEED, 120);
        digitalWrite(PIN_MOTOR_TRAY_1, LOW);
        digitalWrite(PIN_MOTOR_TRAY_2, HIGH);

        display.clearDisplay();
        display.fillRoundRect(5, -10, 118, 20, 5, WHITE);
        for (int i = 0; i < 40; i++)
        {
            display.drawLine(28, 11 + i, 100, 11 + i, WHITE);
            if (displayState)
                display.display();
            delay(15);
        }

        if (displayState)
            ScreenCurrentOnTime = millis();

        digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
        digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
        digitalWrite(PIN_TRAY_MOTOR_SPEED, LOW);
        trayState = true;
    }

    else if (action == TOGGLE)
    {
        if (!trayState)
            switchTray(OPEN, displayState);

        else
            switchTray(CLOSE, displayState);
    }
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état. Cette fonction ne contrôle que les rubans en mode couleur seule.
void switchRGBStrip(int action, boolean displayState)
{
    if (action == SWITCH_OFF && RGBStripState)
    {
        RLEDOFFValue = RLEDValue;
        GLEDOFFValue = GLEDValue;
        BLEDOFFValue = BLEDValue;

        RLEDValue = 0;
        GLEDValue = 0;
        BLEDValue = 0;

        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);

        RGBStripState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !RGBStripState)
    {

        switchMulticolor(SWITCH_OFF, false);
        switchSoundReact(SWITCH_OFF, false);

        RLEDValue = RLEDOFFValue;
        GLEDValue = GLEDOFFValue;
        BLEDValue = BLEDOFFValue;

        RLEDOFFValue = 0;
        GLEDOFFValue = 0;
        BLEDOFFValue = 0;

        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);

        RGBStripState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!RGBStripState)
            switchRGBStrip(SWITCH_ON, displayState);

        else
            switchRGBStrip(SWITCH_OFF, displayState);
    }
}

// Change la couleur des rubans de DEL.
void controlRGBStrip(int r, int g, int b)
{
    switchMulticolor(SWITCH_OFF, false);
    switchSoundReact(SWITCH_OFF, false);

    if (r <= 0 && g <= 0 && b <= 0)
    {
        switchRGBStrip(SWITCH_OFF, false);
        return;
    }

    if (!RGBStripState)
        switchRGBStrip(SWITCH_ON, false);

    if (r < 0)
        r = 0;

    if (r > 255)
        r = 255;

    if (g < 0)
        g = 0;

    if (g > 255)
        g = 255;

    if (b < 0)
        b = 0;

    if (b > 255)
        b = 255;

    RLEDValue = r;
    GLEDValue = g;
    BLEDValue = b;

    analogWrite(PIN_RED_LED, RLEDValue);
    analogWrite(PIN_GREEN_LED, GLEDValue);
    analogWrite(PIN_BLUE_LED, BLEDValue);
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchMulticolor(int action, boolean displayState)
{
    if (action == SWITCH_OFF && multicolorState)
    {
        RLEDValue = 0;
        GLEDValue = 0;
        BLEDValue = 0;
        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);
        multicolorStep = 0;
        multicolorState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !multicolorState)
    {
        switchSoundReact(SWITCH_OFF, false);
        switchRGBStrip(SWITCH_OFF, false);

        RLEDValue = 0;
        GLEDValue = 0;
        BLEDValue = 255;
        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);
        multicolorLastTime = millis();
        multicolorState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!multicolorState)
            switchMulticolor(SWITCH_ON, displayState);

        else
            switchMulticolor(SWITCH_OFF, displayState);
    }
}

// Fonction de gestion de l'animation multicolore.
void multicolorScheduler()
{

    if (!multicolorState)
        return;

    unsigned long actualTime = millis();

    if ((actualTime - multicolorLastTime) < (multicolorSpeed * 10))
        return;

    multicolorLastTime = actualTime;

    if (multicolorStep == 0)
    {
        RLEDValue += 5;
        BLEDValue -= 5;
        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);

        if (RLEDValue == 255)
            multicolorStep = 1;
    }

    else if (multicolorStep == 1)
    {
        GLEDValue += 5;
        RLEDValue -= 5;
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_RED_LED, RLEDValue);

        if (GLEDValue == 255)
            multicolorStep = 2;
    }

    else if (multicolorStep == 2)
    {
        BLEDValue += 5;
        GLEDValue -= 5;
        analogWrite(PIN_BLUE_LED, BLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);

        if (BLEDValue == 255)
            multicolorStep = 0;
    }
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchSoundReact(int action, boolean displayState)
{
    if (action == SWITCH_OFF && soundReactState)
    {
        soundReactMax = 0;
        RLEDValue = 0;
        GLEDValue = 0;
        BLEDValue = 0;
        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);
        soundReactState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !soundReactState)
    {
        switchMulticolor(SWITCH_OFF, false);
        switchRGBStrip(SWITCH_OFF, false);

        randomSeed(PIN_RANDOM_SEED_GENERATOR);
        soundReactLastTime = millis();
        RLEDValue = 0;
        GLEDValue = 0;
        BLEDValue = 0;
        analogWrite(PIN_RED_LED, RLEDValue);
        analogWrite(PIN_GREEN_LED, GLEDValue);
        analogWrite(PIN_BLUE_LED, BLEDValue);
        soundReactState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!soundReactState)
            switchSoundReact(SWITCH_ON, displayState);

        else
            switchSoundReact(SWITCH_OFF, displayState);
    }
}

// Fonction de gestion de l'animation multicolore.
void soundReactScheduler()
{
    if (!soundReactState)
        return;

    // On manipule la valeur envoyée par le microphone pour obtenir une valeur de 0 à +x.
    int sound = analogRead(PIN_MICROPHONE);
    sound = sound - 287;
    sound = abs(sound);

    boolean soundReactMaxChanged = false;

    if (sound > soundReactMax)
    {
        soundReactMax = sound;
        soundReactMaxChanged = true;
    }

    unsigned long actualTime = millis();

    boolean colorChanged = false;

    if ((actualTime - soundReactLastTimeColorChanged) >= 200)
    {
        if (sound >= (soundReactSensibility * soundReactMax))
        {
            colorChanged = true;

            soundReactLastTimeColorChanged = actualTime;

            int eliminatedColor = random(2);
            int firstColor = random(255);
            int secondColor = random(255);

            if (eliminatedColor == 0)
            {
                RLEDValue = 0;
                GLEDValue = firstColor;
                BLEDValue = secondColor;
            }

            else if (eliminatedColor == 1)
            {
                RLEDValue = firstColor;
                GLEDValue = 0;
                BLEDValue = secondColor;
            }

            else if (eliminatedColor == 2)
            {
                RLEDValue = firstColor;
                GLEDValue = secondColor;
                BLEDValue = 0;
            }
        }
    }

    if ((actualTime - soundReactLastTime) >= 300)
    {
        soundReactLastTime = actualTime;

        if (soundReactMaxChanged == false)
            soundReactMax--;

        if (colorChanged == false)
        {
            if ((RLEDValue - 5) >= 0)
                RLEDValue -= 5;

            else
                RLEDValue = 0;

            if ((GLEDValue - 5) >= 0)
                GLEDValue -= 5;

            else
                GLEDValue = 0;

            if ((BLEDValue - 5) >= 0)
                BLEDValue -= 5;

            else
                BLEDValue = 0;
        }
    }

    analogWrite(PIN_RED_LED, RLEDValue);
    analogWrite(PIN_GREEN_LED, GLEDValue);
    analogWrite(PIN_BLUE_LED, BLEDValue);
}

// Paramètre :  DECREASE = diminuer le volume - INCREASE = augmenter le volume - MUTE = couper le son - UNMUTE = réetablir le son - TOGGLE_MUTE = basculer le son.
void volumeSono(int action, boolean displayState)
{
    if (!TVState)
        return;

    if (action == DECREASE && (volume > 0))
    {
        IrSender.sendNEC(0x44C1, 0xC7, 2);
        volume--;
        EEPROM.write(VOLUME_STORAGE_LOCATION, volume);

        if (displayState)
            displayVolume(DECREASE);
    }

    else if (action == INCREASE && (volume < 25))
    {
        IrSender.sendNEC(0x44C1, 0x47, 2);
        volume++;
        EEPROM.write(VOLUME_STORAGE_LOCATION, volume);

        if (displayState)
            displayVolume(INCREASE);
    }

    else if (action == MUTE && volumeMuted == false)
    {
        IrSender.sendNEC(0x44C1, 0x77, 3);
        volumeMuted = true;

        if (displayState)
            displayVolume(MUTE);
    }

    else if (action == UNMUTE && volumeMuted == true)
    {
        IrSender.sendNEC(0x44C1, 0x77, 3);
        volumeMuted = false;

        if (displayState)
            displayVolume(UNMUTE);
    }

    else if (action == TOGGLE_MUTE)
    {
        if (!volumeMuted)
            volumeSono(MUTE, displayState);

        else
            volumeSono(UNMUTE, displayState);
    }
}

// Fonction qui permet de faire bouger le servomoteur sans bibliothèque.
void moveDisplayServo(int pos)
{
    int pulseWidth = map(pos, 0, 180, 600, 2400);
    digitalWrite(PIN_SCREEN_SERVO, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(PIN_SCREEN_SERVO, LOW);
    delay(2);
}

// Fonction qui effectue un clique avec le servomoteur sur le bouton ON/OFF de l'écran.
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

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchTV(int action, boolean displayState)
{
    if (action == SWITCH_OFF && TVState)
    {
        switchDisplay();
        IrSender.sendNEC(0x44C1, 0x87, 3);
        TVState = false;

        if (displayState)
            displayDeviceState(false);
    }

    else if (action == SWITCH_ON && !TVState)
    {
        switchDisplay();
        IrSender.sendNEC(0x44C1, 0x87, 3);
        TVState = true;

        if (displayState)
            displayDeviceState(true);
    }

    else if (action == TOGGLE)
    {
        if (!TVState)
            switchTV(SWITCH_ON, displayState);

        else
            switchTV(SWITCH_OFF, displayState);
    }
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état - STOP_RINGING = arrêter la sonnerie.
void switchAlarm(int action, boolean displayState)
{
    if (action == SWITCH_OFF && alarmState)
    {
        if (alarmTriggered == true)
            switchAlarm(STOP_RINGING, false);

        digitalWrite(PIN_DOOR_LED, LOW);
        displayDeviceState(false);
        alarmState = false;
    }

    else if (action == SWITCH_ON && !alarmState)
    {
        if (cardToStoreState == true)
        {
            displayMessage("ERREUR", "Vous etes en mode ajout d'une carte.");
        }

        digitalWrite(PIN_DOOR_LED, HIGH);
        switchRGBStrip(SWITCH_OFF, false);
        displayDeviceState(true);
        alarmState = true;
    }

    else if (action == TOGGLE)
    {
        if (!alarmState)
            switchAlarm(SWITCH_ON, displayState);

        else
            switchAlarm(SWITCH_OFF, displayState);
    }

    else if (action == STOP_RINGING)
    {
Serial.println("0");
        
        if (alarmTriggered == false)
            return;

        Serial.println("1");

        if (alarmBuzzerState == true)
            digitalWrite(PIN_ALARM_RELAY, LOW);

        digitalWrite(PIN_DOOR_LED, HIGH);
        controlRGBStrip(0, 0, 0);

        display.invertDisplay(false);
        display.clearDisplay();
        display.display();

        alarmTriggered = false;
    }
}

// Déclenche la sonnerie de l'alarme.
void triggerAlarm()
{
    if (alarmState == false)
        alarmState = true;

    if (alarmBuzzerState == true)
        digitalWrite(PIN_ALARM_RELAY, HIGH);

    alarmTriggered = true;
    alarmTriggeredLightsCounter = millis();
    alarmAutoTriggerOFFCounter = millis();
}

// Vérifie si la carte est enregistrée dans le système.
boolean checkCard(uint8_t card[4])
{
    int storedCardsNumber = EEPROM.read(0);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        if (EEPROM.read(i * 5 + 11) == card[0] && EEPROM.read(i * 5 + 12) == card[1] && EEPROM.read(i * 5 + 13) == card[2] && EEPROM.read(i * 5 + 14) == card[3])
            return true;
    }

    return false;
}

// Boucle qui s'exécute lorsque l'alarme sonne. Elle permet de l'éteindre après un certain temps si la porte est refermée. De plus, cette boucle gère le clignottement des rubans de DELs en rouge.
void alarmSheduler()
{
    if ((millis() - cardCounter) >= 1000)
    {
        uint8_t uid[] = {0, 0, 0, 0, 0};
        uint8_t uidLength;

        if (nfcReader.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100))
        {
            if (checkCard(uid))
            {
                yesSound();

                if (alarmTriggered)
                    switchAlarm(STOP_RINGING, true);

                else
                    switchAlarm(TOGGLE, true);
            }

            cardCounter = millis();
        }
    }

    if (!alarmState)
        return;

    if (digitalRead(PIN_BEDROOM_DOOR_SENSOR) == HIGH && !alarmTriggered)
        triggerAlarm();

    if (!alarmTriggered)
        return;

    // Si la porte est fermée, on lance un compteur de 10 secondes avant que l'alarme arrête de sonner. Si la porte est (r)ouverte, le compteur est remis à 0.
    if (digitalRead(PIN_BEDROOM_DOOR_SENSOR) == LOW)
    {
        if ((millis() - alarmAutoTriggerOFFCounter) >= 10000)
            switchAlarm(STOP_RINGING, true);
    }

    else
        alarmAutoTriggerOFFCounter = millis();

    // On fait clignoter la chambre.
    if ((millis() - alarmTriggeredLightsCounter) >= 200 && (millis() - alarmTriggeredLightsCounter) <= 300)
    {
        alarmTriggeredLightsCounter += 100;
        RLEDValue = 255;
        digitalWrite(PIN_DOOR_LED, HIGH);
        RGBStripState = true;
        displayAlarmTriggered(0);
    }

    else if ((millis() - alarmTriggeredLightsCounter) >= 400)
    {
        alarmTriggeredLightsCounter = millis();
        RLEDValue = 0;
        digitalWrite(PIN_DOOR_LED, LOW);
        RGBStripState = false;
        displayAlarmTriggered(1);
    }

    GLEDValue = 0;
    BLEDValue = 0;
    analogWrite(PIN_RED_LED, RLEDValue);
    analogWrite(PIN_GREEN_LED, GLEDValue);
    analogWrite(PIN_BLUE_LED, BLEDValue);
}

void storeCard(uint8_t card[4])
{
    if (checkCard(card))
    {
        noSound();
        displayMessage("ERREUR", "Cette carte a deja ete enregistree.");
        return;
    }

    int storeLocation = EEPROM.read(0) * 5 + 11;

    for (int i = 0; i < 4; i++)
        EEPROM.write(storeLocation + i, card[i]);

    EEPROM.write(0, EEPROM.read(0) + 1);

    yesSound();
    displayMessage("INFO", "La carte a ete enregistree avec succes.");
}

void removeCards()
{
    int storedCardsNumber = EEPROM.read(0);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        int storeLocation = i * 5 + 11;

        for (int j = 0; j < 5; j++)
            EEPROM.write(storeLocation + j, 0);
    }

    yesSound();
    displayMessage("INFO", "Les cartes ont ete supprimees avec succes.");
}