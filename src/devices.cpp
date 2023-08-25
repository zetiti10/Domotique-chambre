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

/////////////////////////////
// Gestion du module disco //
/////////////////////////////

boolean discoState = false;

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchDisco(int action)
{
    if (action == SWITCH_OFF)
    {
        if (discoState == true)
        {
            digitalWrite(PIN_DISCO_RELAY, LOW);
            printDeviceState(false);
            discoState = false;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (discoState == false)
        {
            digitalWrite(PIN_DISCO_RELAY, HIGH);
            printDeviceState(true);
            discoState = true;
        }
    }

    else
    {
        if (discoState == false)
        {
            switchDisco(SWITCH_ON);
        }

        else
        {
            switchDisco(SWITCH_OFF);
        }
    }
}

////////////////////////////
// Gestion du cube de DEL //
////////////////////////////

boolean LEDCubeState = false;

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchLEDCube(int action)
{
    if (action == SWITCH_OFF)
    {
        if (LEDCubeState == true)
        {
            digitalWrite(PIN_LED_CUBE_RELAY, LOW);
            printDeviceState(false);
            LEDCubeState = false;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (LEDCubeState == false)
        {
            digitalWrite(PIN_LED_CUBE_RELAY, HIGH);
            printDeviceState(true);
            LEDCubeState = true;
        }
    }

    else
    {
        if (LEDCubeState == false)
        {
            switchLEDCube(SWITCH_ON);
        }

        else
        {
            switchLEDCube(SWITCH_OFF);
        }
    }
}

///////////////////////////////////
// Gestion de la maquette de rue //
///////////////////////////////////

boolean streetState = false;

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchStreet(int action)
{
    if (action == SWITCH_OFF)
    {
        if (streetState == true)
        {
            digitalWrite(PIN_STREET_RELAY, LOW);
            printDeviceState(false);
            streetState = false;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (streetState == false)
        {
            digitalWrite(PIN_STREET_RELAY, HIGH);
            printDeviceState(true);
            streetState = true;
        }
    }

    else
    {
        if (streetState == false)
        {
            switchStreet(SWITCH_ON);
        }

        else
        {
            switchStreet(SWITCH_OFF);
        }
    }
}

///////////////////////////////////
// Gestion de la lampe de bureau //
///////////////////////////////////

boolean deskLightState = false;

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchDeskLight(int action)
{
    if (action == SWITCH_OFF)
    {
        if (deskLightState == true)
        {
            digitalWrite(PIN_DESK_LIGHT_RELAY, LOW);
            printDeviceState(false);
            deskLightState = false;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (deskLightState == false)
        {
            digitalWrite(PIN_DESK_LIGHT_RELAY, HIGH);
            printDeviceState(true);
            deskLightState = true;
        }
    }

    else
    {
        if (deskLightState == false)
        {
            switchDeskLight(SWITCH_ON);
        }

        else
        {
            switchDeskLight(SWITCH_OFF);
        }
    }
}

////////////////////////
// Gestion du plateau //
////////////////////////

boolean trayState = false;

// Paramètre :  CLOSE = fermet le plateau - OPEN = ouvrir le plateau - TOGGLE = basculer le plateau.
void switchTray(int action)
{
    if (action == CLOSE)
    {
        if (trayState == true)
        {
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
            ScreenCurrentOnTime = millis();
            digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
            digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
            trayState = false;
        }
    }

    else if (action == OPEN)
    {
        if (trayState == false)
        {
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
            ScreenCurrentOnTime = millis();
            digitalWrite(PIN_MOTOR_TRAY_1, HIGH);
            digitalWrite(PIN_MOTOR_TRAY_2, HIGH);
            trayState = true;
        }
    }

    else
    {
        if (trayState == false)
        {
            switchTray(OPEN);
        }

        else
        {
            switchTray(CLOSE);
        }
    }
}

/////////////////////////// RVB ///////////////////////////////////

boolean soundReactState = false;
int soundReactMax = 0;
double soundReactSensibility = 0.75;
unsigned long soundReactLastTime = 0;

boolean multicolorState = false;
unsigned long multicolorSpeed = 10UL;
int multicolorStep = 0;
unsigned long multicolorLastTime = 0;

int RLEDValue = 0;
int GLEDValue = 0;
int BLEDValue = 0;

int RLEDOFFValue = 0;
int GLEDOFFValue = 0;
int BLEDOFFValue = 0;

int RGBStripPrecision = 20;

boolean RGBStripState = false;

void switchRGBStrip(int action)
{
    if (multicolorState == true)
    {
        switchMulticolor(SWITCH_OFF);
    }

    if (soundReactState == true)
    {
        switchSoundReact(SWITCH_OFF);
    }

    if (action == SWITCH_OFF)
    {
        if (RGBStripState == true)
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
        }
    }

    else if (action == SWITCH_ON)
    {
        if (RGBStripState == false)
        {
            RLEDValue = RLEDOFFValue;
            RLEDValue = RLEDOFFValue;
            RLEDValue = RLEDOFFValue;

            RLEDOFFValue = 0;
            GLEDOFFValue = 0;
            BLEDOFFValue = 0;

            analogWrite(PIN_RED_LED, RLEDValue);
            analogWrite(PIN_GREEN_LED, GLEDValue);
            analogWrite(PIN_BLUE_LED, BLEDValue);

            RGBStripState = true;
        }
    }

    else
    {
        if (RGBStripState == false)
        {
            switchRGBStrip(SWITCH_ON);
        }

        else
        {
            switchRGBStrip(SWITCH_OFF);
        }
    }
}

void controlRGBStrip(int r, int g, int b)
{
    if (multicolorState == true)
    {
        switchMulticolor(SWITCH_OFF);
    }

    if (soundReactState == true)
    {
        switchSoundReact(SWITCH_OFF);
    }

    if (r <= 0 && g <= 0 && b <= 0)
    {
        switchRGBStrip(SWITCH_OFF);
        return;
    }

    if (RGBStripState == false)
    {
        RLEDOFFValue = 0;
        GLEDOFFValue = 0;
        BLEDOFFValue = 0;

        RGBStripState = true;
    }

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
    RLEDValue = g;
    RLEDValue = b;

    analogWrite(PIN_RED_LED, RLEDValue);
    analogWrite(PIN_GREEN_LED, GLEDValue);
    analogWrite(PIN_BLUE_LED, BLEDValue);
}

/////////////////////////////////////////////////
// Gestion du mode multicolore du ruban de DEL //
/////////////////////////////////////////////////

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchMulticolor(int action)
{
    if (action == SWITCH_OFF)
    {
        if (multicolorState == true)
        {
            multicolorState = false;
            printDeviceState(false);
            RLEDValue = 0;
            GLEDValue = 0;
            BLEDValue = 0;
            analogWrite(PIN_RED_LED, RLEDValue);
            analogWrite(PIN_GREEN_LED, GLEDValue);
            analogWrite(PIN_BLUE_LED, BLEDValue);
            multicolorStep = 0;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (multicolorState == false)
        {
            switchSoundReact(SWITCH_OFF);
            multicolorState = true;
            printDeviceState(true);
            RLEDValue = 0;
            GLEDValue = 0;
            BLEDValue = 255;
            analogWrite(PIN_RED_LED, RLEDValue);
            analogWrite(PIN_GREEN_LED, GLEDValue);
            analogWrite(PIN_BLUE_LED, BLEDValue);
            multicolorLastTime = millis();
        }
    }

    else
    {
        if (multicolorState == false)
        {
            switchMulticolor(SWITCH_ON);
        }

        else
        {
            switchMulticolor(SWITCH_OFF);
        }
    }
}

void multicolorScheduler()
{

    if (multicolorState == false)
    {
        return;
    }

    unsigned long actualTime = millis();

    if ((actualTime - multicolorLastTime) >= multicolorSpeed)
    {
        multicolorLastTime = actualTime;

        if (multicolorStep == 0)
        {
            RLEDValue++;
            BLEDValue--;
            analogWrite(PIN_RED_LED, RLEDValue);
            analogWrite(PIN_BLUE_LED, BLEDValue);

            if (RLEDValue == 255)
            {
                multicolorStep = 1;
            }
        }

        else if (multicolorStep == 1)
        {
            GLEDValue++;
            RLEDValue--;
            analogWrite(PIN_GREEN_LED, GLEDValue);
            analogWrite(PIN_RED_LED, RLEDValue);

            if (GLEDValue == 255)
            {
                multicolorStep = 2;
            }
        }

        else if (multicolorStep == 2)
        {
            BLEDValue++;
            GLEDValue--;
            analogWrite(PIN_BLUE_LED, BLEDValue);
            analogWrite(PIN_GREEN_LED, GLEDValue);

            if (BLEDValue == 255)
            {
                multicolorStep = 0;
            }
        }
    }
}

//////////////////////////////////////////////////
// Gestion du mode son-réaction du ruban de DEL //
//////////////////////////////////////////////////

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état.
void switchSoundReact(int action)
{
    if (action == SWITCH_OFF)
    {
        if (soundReactState == true)
        {
            soundReactState = false;
            printDeviceState(false);
            soundReactMax = 0;
            RLEDValue = 0;
            GLEDValue = 0;
            BLEDValue = 0;
            analogWrite(PIN_RED_LED, RLEDValue);
            analogWrite(PIN_GREEN_LED, GLEDValue);
            analogWrite(PIN_BLUE_LED, BLEDValue);
        }
    }

    else if (action == SWITCH_ON)
    {
        if (soundReactState == false)
        {
            switchMulticolor(SWITCH_OFF);
            soundReactState = true;
            printDeviceState(true);
            randomSeed(PIN_RANDOM_SEED_GENERATOR);
            soundReactLastTime = millis();
            RLEDValue = 0;
            GLEDValue = 0;
            BLEDValue = 0;
            analogWrite(PIN_RED_LED, RLEDValue);
            analogWrite(PIN_GREEN_LED, GLEDValue);
            analogWrite(PIN_BLUE_LED, BLEDValue);
        }
    }

    else
    {
        if (soundReactState == false)
        {
            switchSoundReact(SWITCH_ON);
        }

        else
        {
            switchSoundReact(SWITCH_OFF);
        }
    }
}

void soundReactScheduler()
{

    if (soundReactState == false)
    {
        return;
    }

    int sound = analogRead(PIN_MICROPHONE);
    sound = abs(sound);
    sound = sound - 287;

    boolean soundReactMaxChanged = false;

    if (sound > soundReactMax)
    {
        soundReactMax = sound;
        soundReactMaxChanged = true;
    }

    unsigned long actualTime = millis();

    if ((actualTime - soundReactLastTime) >= 100)
    {
        if (soundReactMaxChanged == false)
        {
            soundReactMax--;
        }

        if (sound >= (soundReactSensibility * soundReactMax))
        {
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

    else
    {
        if (RLEDValue > 0)
        {
            RLEDValue--;
        }

        if (GLEDValue > 0)
        {
            GLEDValue--;
        }

        if (BLEDValue > 0)
        {
            BLEDValue--;
        }
    }

    analogWrite(PIN_RED_LED, RLEDValue);
    analogWrite(PIN_GREEN_LED, GLEDValue);
    analogWrite(PIN_BLUE_LED, BLEDValue);
}

//////////////////////////////////
// Gestion du volume de la sono //
//////////////////////////////////

boolean TVState = false;

int volumePrecision = 5;
int volume = 0; // Récupérer la valeur de EEPROM !
boolean volumeMuted = false;

// Paramètre :  DECREASE = diminuer le volume - INCREASE = augmenter le volume - MUTE = couper le son - UNMUTE = réetablir le son - TOGGLE_MUTE = basculer le son.
void volumeSono(int action)
{
    if (TVState == true)
    {
        if (action == DECREASE)
        {
            if ((volume - volumePrecision) >= 0)
            {
                printVolume(DECREASE);
                IrSender.sendNEC(0x44C1, 0xC7, volumePrecision);
                volume = volume - volumePrecision;
            }
        }

        else if (action == INCREASE)
        {
            if ((volume + volumePrecision) <= 100)
            {
                printVolume(INCREASE);
                IrSender.sendNEC(0x44C1, 0x47, volumePrecision);
                volume = volume + volumePrecision;
            }
        }

        else if (action == MUTE)
        {
            if (volumeMuted == false)
            {
                printVolume(MUTE);
                IrSender.sendNEC(0x44C1, 0x77, 3);
                volumeMuted = true;
            }
        }

        else if (action == UNMUTE)
        {
            if (volumeMuted == true)
            {
                printVolume(MUTE);
                IrSender.sendNEC(0x44C1, 0x77, 3);
                volumeMuted = false;
            }
        }

        else if (action == TOGGLE_MUTE)
        {
            if (volumeMuted == false)
            {
                volumeSono(MUTE);
            }

            else
            {
                volumeSono(UNMUTE);
            }
        }
    }
}

//////////////////////////////
// Gestion de la télévision //
//////////////////////////////

// Basculer l'état de la sono.
void switchSono()
{
    IrSender.sendNEC(0x44C1, 0x87, 3);
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
void switchTV(int action)
{
    if (action == SWITCH_OFF)
    {
        if (TVState == true)
        {
            printDeviceState(false);
            switchDisplay();
            switchSono();
            TVState = false;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (TVState == false)
        {
            printDeviceState(true);
            switchDisplay();
            switchSono();
            TVState = false;
        }
    }

    else
    {
        if (TVState == false)
        {
            switchTV(SWITCH_ON);
        }

        else
        {
            switchTV(SWITCH_OFF);
        }
    }
}

/////////////////////////
// Gestion de l'alarme //
/////////////////////////

boolean alarmState = false;
boolean alarmTriggered = false;
unsigned long alarmAutoTriggerOFFCounter = 0;
unsigned long alarmTriggeredLightsCounter = 0;

// Fonction qui arrête de faire sonner l'alarme proprement.
void stopAlarmRinging()
{
    if (alarmTriggered == false)
    {
        return;
    }

    alarmTriggered = false;

    if (alarmBuzzerState == true)
    {
        digitalWrite(PIN_ALARM_RELAY, LOW);
    }

    digitalWrite(PIN_RED_LED, LOW);
    controlRGBStrip(0, 0, 0);
    digitalWrite(PIN_DOOR_LED, LOW);
    printAlarm(3);

    display.invertDisplay(false);
    display.clearDisplay();
    display.display();
}

// Paramètre : SWITCH_OFF = éteindre - SWITCH_ON = allumer - TOGGLE = changer l'état - STOP_RINGING = arrêter la sonnerie.
void switchAlarm(int action)
{
    if (action == SWITCH_OFF)
    {
        if (alarmState == true)
        {
            if (alarmTriggered == true)
            {
                stopAlarmRinging();
            }

            digitalWrite(PIN_DOOR_LED, LOW);
            printAlarm(0);
            alarmState = false;
        }
    }

    else if (action == SWITCH_ON)
    {
        if (alarmState == false)
        {
            if (cardToStoreState == true)
            {
                return;
            }

            digitalWrite(PIN_DOOR_LED, HIGH);
            printAlarm(1);
            alarmState = true;
        }
    }

    else if (action == TOGGLE)
    {
        if (alarmState == false)
        {
            switchAlarm(SWITCH_ON);
        }

        else
        {
            switchAlarm(SWITCH_OFF);
        }
    }

    else
    {
        if (alarmTriggered == true)
        {
            stopAlarmRinging();
        }
    }
}

boolean alarmBuzzerState = true;

void triggerAlarm()
{
    if (alarmState == false)
    {
        alarmState = true;
    }

    if (alarmBuzzerState == true)
    {
        digitalWrite(PIN_ALARM_RELAY, HIGH);
    }

    alarmTriggered = true;
    alarmTriggeredLightsCounter = millis();
    alarmAutoTriggerOFFCounter = millis();
}

boolean checkCard(uint8_t card[5])
{
    int storedCardsNumber = EEPROM.read(0);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        if (EEPROM.read(i * 5 + 11) == card[0] && EEPROM.read(i * 5 + 12) == card[1] && EEPROM.read(i * 5 + 13) == card[2] && EEPROM.read(i * 5 + 14) == card[3] && EEPROM.read(i * 5 + 15) == card[4])
        {
            return true;
        }
    }

    return false;
}

// Boucle qui s'exécute lorsque l'alarme sonne. Elle permet de l'éteindre après un certain temps si la porte est refermée. De plus, cette boucle gère le clignottement des rubans de DELs en rouge.
void alarmSheduler()
{
    uint8_t uid[] = {0, 0, 0, 0, 0};
    uint8_t uidLength;

    if (nfcReader.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 1))
    {
        if (checkCard(uid) == true)
        {
            if (alarmTriggered == true)
            {
                stopAlarmRinging();
            }

            else
            {
                switchAlarm(TOGGLE);
            }

            yesSound();
        }

        else
        {
            triggerAlarm();
        }
    }

    if (alarmState == false)
    {
        return;
    }

    if (digitalRead(PIN_BEDROOM_DOOR_SENSOR) == HIGH && alarmTriggered == false)
    {
        triggerAlarm();
    }

    if (alarmTriggered == true)
    {
        // Si la porte est fermée, on lance un compteur de 10 secondes avant que l'alarme arrête de sonner. Si la porte est (r)ouverte, le compteur est remis à 0.
        boolean bedroomDoorState = digitalRead(PIN_BEDROOM_DOOR_SENSOR);

        if (bedroomDoorState == LOW)
        {
            if ((millis() - alarmAutoTriggerOFFCounter) >= 10000)
            {
                stopAlarmRinging();
            }
        }

        else
        {
            alarmAutoTriggerOFFCounter = millis();
        }

        // On fait clignoter la chambre.
        if ((millis() - alarmTriggeredLightsCounter) >= 200 && (millis() - alarmTriggeredLightsCounter) >= 300)
        {
            alarmTriggeredLightsCounter = alarmTriggeredLightsCounter - 100;
            controlRGBStrip(255, 0, 0);
            digitalWrite(PIN_DOOR_LED, HIGH);
            printAlarm(2);
        }

        if ((millis() - alarmTriggeredLightsCounter) >= 200 && (millis() - alarmTriggeredLightsCounter) >= 300)
        {
            alarmTriggeredLightsCounter = millis();
            controlRGBStrip(0, 0, 0);
            digitalWrite(PIN_DOOR_LED, LOW);
            printAlarm(3);
        }
    }
}

boolean cardToStoreState = false;

void storeCard(uint8_t card[5])
{

    if (checkCard(card) == true)
    {
        return;
    }

    int storeLocation = EEPROM.read(0) * 5 + 11;

    for (int i = 0; i < 5; i++)
    {
        EEPROM.write(storeLocation + i, card[i]);
    }

    EEPROM.write(0, EEPROM.read(0) + 1);
}

void removeCards()
{
    int storedCardsNumber = EEPROM.read(0);

    for (int i = 0; i < storedCardsNumber; i++)
    {
        int storeLocation = i * 5 + 11;

        for (int j = 0; j < 5; j++)
        {
            EEPROM.write(storeLocation + j, 0);
        }
    }
}