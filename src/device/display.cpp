/**
 * @file device/display.cpp
 * @author Louis L
 * @brief Classe représentant l'écran du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "display.hpp"
#include "../logger.hpp"
#include "../bitmaps.hpp"

Display::Display(String friendlyName) : Device(friendlyName), m_display(128, 64, &Wire, -1) {}

void Display::setup()
{
    if (m_display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
    {
        m_display.clearDisplay();
        m_display.cp437(true);
        m_display.setTextColor(WHITE);

        m_displayCounter = 0;

        m_operational = true;

        sendLogMessage(INFO, "L'écran '" + m_friendlyName + "' est initialisé.");
    }

    else
        sendLogMessage(ERROR, "La communication avec l'écran '" + m_friendlyName + "' n'a pas pu être est initialisée.");
}

void Display::displayUnavailableDevices()
{
}

void Display::displayBell()
{
    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, bellBitmap, 128, 64, WHITE);
    m_display.display();
    m_displayCounter = millis();
}

void Display::displayMessage(String message, String title)
{
    m_display.clearDisplay();

    m_display.setCursor(0, 0);
    m_display.setTextSize(2);
    m_display.print(title);

    m_display.setCursor(0, 18);
    m_display.setTextSize(1);
    m_display.print(message);

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayVolume(volumeType action)
{
    m_display.clearDisplay();

    if (action == DECREASE)
        m_display.drawBitmap(0, 0, volumeDecreaseBitmap, 128, 64, WHITE);

    else if (action == INCREASE)
        m_display.drawBitmap(0, 0, volumeIncreaseBitmap, 128, 64, WHITE);

    else if (action == MUTE)
        m_display.drawBitmap(0, 0, muteBitmap, 128, 64, WHITE);

    else if (action == UNMUTE)
        m_display.drawBitmap(0, 0, unmuteBitmap, 128, 64, WHITE);

    if (action == DECREASE || action == INCREASE || action == UNMUTE)
    {
        m_display.drawRect(50, 52, 27, 3, WHITE);

        /*if (volume > 0)
            m_display.drawLine(51, 53, 51 + volume, 53, WHITE);*/

        m_display.setTextSize(1);
        m_display.setCursor(80, 52);
        // m_display.print(volume);
    }

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayAlarmTriggered(boolean colorsInverted)
{
    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, alarmTriggeredBitmap, 128, 64, 1);

    if (!colorsInverted)
        m_display.invertDisplay(false);

    else
        m_display.invertDisplay(true);

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayAirValues()
{
    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, airBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);

    m_display.setCursor(40, 42);
    // m_display.print(temperature);
    m_display.write(0xF8);
    m_display.print("C");

    m_display.setCursor(40, 10);
    // m_display.print(humidity);
    m_display.write(0x25);

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayLuminosityMotionSensorValues()
{
    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, luminosityMotionSensorsBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);

    m_display.setCursor(40, 42);
    // m_display.print(analogRead(PIN_LIGHT_SENSOR));

    m_display.setCursor(40, 10);
    /*if (digitalRead(PIN_MOTION_SENSOR))
        m_display.print("OUI");

    else
        m_display.print("NON");*/

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayLEDState()
{
    m_display.clearDisplay();
    m_display.setTextSize(2);

    m_display.setCursor(22, 0);
    m_display.print("R");
    /*int rGauge = int(map(RLEDValue, 0, 255, 0, 45));
    m_display.fillRect(25, 18, 4, rGauge, WHITE);*/
    m_display.drawRect(24, 17, 6, 47, WHITE);

    m_display.setCursor(62, 0);
    m_display.print("V");
    /*int gGauge = int(map(GLEDValue, 0, 255, 0, 45));
    m_display.fillRect(65, 18, 4, gGauge, WHITE);*/
    m_display.drawRect(64, 17, 6, 47, WHITE);

    m_display.setCursor(101, 0);
    m_display.print("B");
    /*int bGauge = int(map(BLEDValue, 0, 255, 0, 45));
    m_display.fillRect(102, 18, 4, bGauge, WHITE);*/
    m_display.drawRect(101, 17, 6, 47, WHITE);

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayDeviceState(boolean on)
{
    m_display.clearDisplay();

    if (on == true)
    {
        for (int i = 51; i < 75; i += 4)
        {
            m_display.clearDisplay();
            m_display.drawRoundRect(41, 14, 46, 24, 12, WHITE);
            m_display.fillCircle(i, 26, 8, WHITE);
            m_display.display();
        }
    }

    else
    {
        for (int i = 75; i > 51; i -= 4)
        {
            m_display.clearDisplay();
            m_display.drawRoundRect(41, 14, 46, 24, 12, WHITE);
            m_display.drawCircle(i, 26, 8, WHITE);
            m_display.display();
        }
    }

    m_displayCounter = millis();
}

void Display::displayKeypadMenu()
{
    m_display.clearDisplay();
    m_display.setTextSize(1);
    m_display.setCursor(0, 0);

    /*switch (keypadMenu)
    {
    case LIGHTS_MENU:
        m_display.drawBitmap(0, 0, lightsMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(20, 55);
        m_display.print("Menu : lumieres");
        break;

    case SOFA_LIGHT_CONTROL_SUBMENU:
        m_display.print("Lampe canape");

        m_display.setCursor(0, 15);
        m_display.println("1. Temperature");
        m_display.println("2. Luminosite");
        m_display.print("3. Effet");
        break;

    case SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU:
        m_display.print("Temperature : PAS CONFIGURE");
        // Afficher rectangle temperature.
        break;

    case SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU:
        m_display.print("Luminosite : PAS CONFIGURE");
        // Afficher rectangle luminosite.
        break;

    case SOFA_LIGHT_EFFECT_CONTROL_SUBMENU:
        m_display.print("1. Test - 2. Test - 3. Test PAS CONFIGURE");
        break;

    case BEDSIDE_LIGHT_CONTROL_SUBMENU:
        m_display.print("Lampe de chevet");

        m_display.setCursor(0, 15);
        m_display.println("1. Temperature");
        m_display.println("2. Couleur");
        m_display.println("3. Luminosite");
        m_display.print("4. Effet");
        break;

    case BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU:
        m_display.print("Temperature : PAS CONFIGURE");
        // Afficher rectangle temperature.
        break;

    case BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU:
        m_display.print("Couleur : PAS CONFIGURE");
        // Afficher les 3 rectangles de la couleur.
        break;

    case BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU:
        m_display.print("Luminosite : PAS CONFIGURE");
        // Afficher rectangle luminosite.
        break;

    case BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU:
        m_display.print("1. Test - 2. Test - 3. Test PAS CONFIGURE");
        break;

    case RGB_STRIP_CONTROL_SUBMENU:
        m_display.print("Rubans de DEL");

        m_display.setCursor(0, 15);
        m_display.println("1. Couleur");
        m_display.print("2. Effet");
        break;

    case RGB_STRIP_COLOR_CONTROL_SUBMENU:
        displayLEDState();
        break;

    case RGB_STRIP_EFFECT_CONTROL_SUBMENU:
        m_display.print("Effet");

        m_display.setCursor(0, 15);
        m_display.println("1. Multicolore");
        m_display.print("2. Son reaction");
        break;

    case DEVICES_MENU:
        m_display.drawBitmap(0, 0, devicesMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(4, 55);
        m_display.print("Menu : peripheriques");
        break;

    case SENSORS_MENU:
        m_display.drawBitmap(0, 0, sensorsMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(20, 55);
        m_display.print("Menu : capteurs");
        break;

    case TV_MENU:
        m_display.drawBitmap(0, 0, TVMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(15, 55);
        m_display.print("Menu : television");
        break;

    case CONFIGURATION_MENU:
        m_display.drawBitmap(0, 0, configurationMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(5, 55);
        m_display.print("Menu : configuration");
        break;

    case ALARM_CODE_CONFIGURATION_SUBMENU:
        m_display.print("Entrez le mot de passe pour acceder au menu de gestion de l'alarme.");
        break;

    case ALARM_CONFIGURATION_SUBMENU:
        m_display.print("Alarme");

        m_display.setCursor(0, 15);
        m_display.println("1. Son de l'alarme");
        m_display.println("2. Ajouter une carte");
        m_display.print("3. Suprimer les cartes");
        break;

    default:
        break;
    }*/

    m_display.display();
    m_displayCounter = millis();
}

void Display::displayTray(boolean shareInformation, boolean on)
{
    if (on)
    {
        m_display.clearDisplay();
        m_display.fillRoundRect(5, -10, 118, 20, 5, WHITE);
        for (int i = 0; i < 40; i++)
        {
            m_display.drawLine(28, 11 + i, 100, 11 + i, WHITE);
            if (shareInformation)
                m_display.display();
            delay(15);
        }

        if (shareInformation)
            m_displayCounter = millis();
    }

    else
    {
        m_display.fillRoundRect(5, -10, 118, 20, 5, WHITE);
        for (int i = 0; i < 40; i++)
            m_display.drawLine(28, 11 + i, 100, 11 + i, WHITE);

        if (shareInformation)
            m_display.display();

        for (int i = 0; i < 40; i++)
        {
            m_display.drawLine(28, 50 - i, 100, 50 - i, BLACK);
            if (shareInformation)
                m_display.display();
            delay(15);
        }

        if (shareInformation)
            m_displayCounter = millis();
    }
}

void Display::loop()
{
    if (m_operational && (m_displayCounter != 0) && ((millis() - m_displayCounter) >= 8000))
    {
        m_displayCounter = 0;
        m_display.clearDisplay();
        m_display.display();
    }
}
