/**
 * @file device/interface/display.cpp
 * @author Louis L
 * @brief Classe gérant l'écran du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Autres fichiers du programme.
#include "device/interface/display.hpp"
#include "device/device.hpp"
#include "bitmaps.hpp"
#include "display.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
Display::Display(const __FlashStringHelper *friendlyName, int ID) : Device(friendlyName, ID), m_display(128, 64, &Wire, -1), m_lastTime(0), m_lastStateAnimation(0), m_menuHelpList(nullptr), m_menuHelpMenu(1) {}

/// @brief Initialise l'objet.
void Display::setup()
{
    if (m_operational || !m_display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
        return;

    m_display.clearDisplay();
    m_display.cp437(true);
    m_display.setTextColor(WHITE);

    m_operational = true;
}

/// @brief Affiche la liste des périphériques indisponibles.
/// @param deviceList Un pointeur vers la liste à vérifier.
/// @param devicesNumber Le nombre d'éléments de la liste.
void Display::displayUnavailableDevices(Device *deviceList[], int &devicesNumber)
{
    if (!m_operational)
        return;

    resetDisplay();

    // Vérification pour chaque périphérique de la liste s'il est indisponible.
    int counter = 0;
    for (int i = 0; i < devicesNumber; i++)
    {
        if (!deviceList[i]->getAvailability())
        {
            m_display.setCursor(0, counter * 10 + 10);
            printAccents(deviceList[i]->getFriendlyName());
            counter++;
        }
    }

    // Si aucune erreur n'a été détectée, affichage d'un message.
    if (counter == 0)
    {
        displayMessage("Démarré !");
    }

    // Affichage de la liste des périphériques indisponibles.
    else
    {
        m_display.setCursor(0, 0);
        displayMessage("", "ERREURS");
        display();
    }
}

/// @brief Affiche un pictogramme de cloche à l'écran.
void Display::displayBell()
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, bellBitmap, 128, 64, WHITE);
    display();
}

/// @brief Affiche un message à l'écran avec un gros titre.
/// @param message Le message à afficher.
/// @param title Le titre du message (par défaut `INFO`).
void Display::displayMessage(const String &message, const String &title)
{
    if (!m_operational)
        return;

    resetDisplay();
    printCenteredAccents(title, 1, 0);
    m_display.setCursor(0, 10);
    printAccents(message);
    display();
}

/// @brief Affiche le volume actuel.
/// @param action Le type de message à afficher (modifie le picrogramme ainsi que les informations affichées ; par défaut, le volume actuel).
/// @param volume Le volume actuel (par défaut `0`, pour les actions qui n'affichent pas le volume actuel).
void Display::displayVolume(VolumeType action, int volume)
{
    if (!m_operational)
        return;

    resetDisplay();

    // Affichage d'un pictogramme en fonction de l'action sélectionnée.
    if (action == DECREASE)
        m_display.drawBitmap(0, 0, volumeDecreaseBitmap, 128, 64, WHITE);

    else if (action == INCREASE)
        m_display.drawBitmap(0, 0, volumeIncreaseBitmap, 128, 64, WHITE);

    else if (action == MUTE)
        m_display.drawBitmap(0, 0, muteBitmap, 128, 64, WHITE);

    else if (action == UNMUTE)
        m_display.drawBitmap(0, 0, unmuteBitmap, 128, 64, WHITE);

    // Affichage du volume selon le mode sélectionné.
    if (action == DECREASE || action == INCREASE || action == UNMUTE)
    {
        m_display.drawRect(14, 45, 100, 6, WHITE);

        if (volume > 0)
            m_display.fillRect(15, 46, map(volume, 0, 25, 0, 98), 4, WHITE);

        printCenteredAccents(String(volume), 1, 57);
    }

    display();
}

/// @brief Affiche un pictogramme d'alerte (point d'exclamation).
/// @param colorsInverted Inversion ou non des couleurs de l'écran.
void Display::displayAlarmTriggered(bool colorsInverted)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, alarmTriggeredBitmap, 128, 64, 1);

    if (!colorsInverted)
        m_display.invertDisplay(false);

    else
        m_display.invertDisplay(true);

    display();
}

/// @brief Affiche les valeurs des capteurs de température et d'humidité, avec deux pictogrammes.
/// @param temperature La valeur de la température à afficher.
/// @param humidity La valeur de l'humidité à afficher.
void Display::displayAirValues(float temperature, float humidity)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, airBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);

    m_display.setCursor(40, 42);
    m_display.print(temperature);
    // Code pour insérer un "°".
    m_display.write(0xF8);
    m_display.print(F("C"));

    m_display.setCursor(40, 10);
    m_display.print(humidity);
    // Code pour insérer un "%".
    m_display.write(0x25);

    display();
}

/// @brief Affiche la valeur d'un capteur analogique.
/// @param value La valeur du capteur analogique.
void Display::displayAnalogSensorValue(int value)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, analogSensorBitmap, 128, 64, WHITE);
    printCenteredAccents(String(value), 2, 45);
    display();
}

/// @brief Affiche la valeur d'un capteur binaire.
/// @param value La valeur du capteur binaire.
void Display::displayBinarySensorValue(bool value)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, binarySensorBitmap, 128, 64, WHITE);
    printCenteredAccents(String(value), 2, 45);
    display();
}

/// @brief Affiche trois jauges correspondant à l'intensité des trois couleurs.
/// @param r La valeur de l'intensité du rouge.
/// @param g La valeur de l'intensité du vert.
/// @param b La valeur de l'intensité du bleu.
void Display::displayLEDState(int r, int g, int b)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.setTextSize(2);

    m_display.setCursor(22, 0);
    m_display.print(F("R"));
    m_display.fillRect(25, 18, 4, int(map(r, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(24, 17, 6, 47, WHITE);

    m_display.setCursor(62, 0);
    m_display.print(F("V"));
    m_display.fillRect(65, 18, 4, int(map(g, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(64, 17, 6, 47, WHITE);

    m_display.setCursor(101, 0);
    m_display.print(F("B"));
    m_display.fillRect(102, 18, 4, int(map(b, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(101, 17, 6, 47, WHITE);

    display();
}

/// @brief Affiche une animation illustrant la mise en marche ou l'arrêt d'un périphérique.
/// @param on Mise en marche ou errêt.
void Display::displayDeviceState(bool on)
{
    if (!m_operational || (millis() - m_lastStateAnimation) <= 500)
        return;

    resetDisplay();

    // Animation de mise en marche.
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

    // Animation d'arrêt.
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

    m_lastStateAnimation = millis();

    display();
}

void Display::displayKeypadMenu(MenuIcons menuIcon, String &menuName)
{
    if (!m_operational)
        return;

    resetDisplay();

    switch (menuIcon)
    {
    case OUTPUTS:
        m_display.drawBitmap(0, 0, devicesMenuBitmap, 128, 64, WHITE);
        break;

    case LIGHTS:
        m_display.drawBitmap(0, 0, lightsMenuBitmap, 128, 64, WHITE);
        break;

    case INPUTS:
        m_display.drawBitmap(0, 0, sensorsMenuBitmap, 128, 64, WHITE);
        break;

    case TELEVISIONS:
        m_display.drawBitmap(0, 0, TVMenuBitmap, 128, 64, WHITE);
        break;

    case ALARMS:
        m_display.drawBitmap(0, 0, AlarmMenuBitmap, 128, 64, WHITE);
        break;

    case CONTROLS:
        m_display.drawBitmap(0, 0, deviceControlMenuBitmap, 128, 64, WHITE);
        break;
    }

    printCenteredAccents(menuName, 1, 53);
    display();
}

/// @brief Affiche l'aide d'un menu, avec deux affichages qui s'alternent.
/// @param menuHelpList La liste des commandes du menu pour chaque touche, d'une taille de 10 éléments (de 1 à 9, avec le 0 en dernier).
/// @param menuTitle Le nom du menu.
void Display::displayKeypadMenuHelp(String *menuHelpList, String &menuName)
{
    if (!m_operational)
        return;

    resetDisplay(false);

    int begin = 0;

    if ((menuHelpList != m_menuHelpList) || (m_menuHelpMenu == 2) || (menuHelpList[5] == "" && menuHelpList[6] == "" && menuHelpList[7] == "" && menuHelpList[8] == "" && menuHelpList[9] == ""))
        m_menuHelpMenu = 1;

    else
    {
        m_menuHelpMenu = 2;
        begin = 5;
    }

    m_display.setCursor(0, 9);
    m_display.setTextWrap(false);

    for (int i = begin; i < (begin + 5); i++)
    {
        if (menuHelpList[i] == "")
            continue;

        if (i < 9)
            m_display.print(i + 1);

        else
            m_display.print(0);

        m_display.print(F(" "));
        printAccents(menuHelpList[i]);
        m_display.println();
    }

    m_menuHelpList = menuHelpList;

    m_display.setCursor(0, 0);
    m_display.print(F("? "));
    printAccents(menuName);

    display();
}

/// @brief Affiche l'animation d'ouverture ou de fermeture du plateau.
/// @param on Ouverture ou fermeture du plateau.
/// @param shareInformation Affichage ou non de l'animation (permet d'obtenir un délai sans rien afficher).
void Display::displayTray(bool on, bool shareInformation)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.fillRoundRect(5, -10, 118, 20, 5, WHITE);

    if (on)
    {
        for (int i = 0; i < 40; i++)
        {
            m_display.drawLine(28, 11 + i, 100, 11 + i, WHITE);
            if (shareInformation)
                m_display.display();
            delay(15);
        }
    }

    else
    {
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
    }

    if (shareInformation)
        display();
}

/// @brief Affiche la température de couleur actuelle d'une ampoule.
/// @param minimum La température minimum en kelvin.
/// @param maximum La température maximum en kelvin.
/// @param temperature La température actuelle en kelvin.
void Display::displayLightColorTemperature(int minimum, int maximum, int temperature)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, lightColorTemperatureBitmap, 128, 64, WHITE);
    m_display.drawRect(14, 45, 100, 6, WHITE);

    if (temperature > minimum)
        m_display.fillRect(15, 46, map(temperature, minimum, maximum, 0, 98), 4, WHITE);

    String text = String(temperature) + "K";
    printCenteredAccents(text, 1, 57);
    display();
}

/// @brief Affiche la luminosité actuelle d'une ampoule.
/// @param luminosity La luminosité de `0` à `255`.
void Display::displayLuminosity(int luminosity)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, lightLuminosityBitmap, 128, 64, WHITE);
    m_display.drawRect(14, 45, 100, 6, WHITE);

    if (luminosity > 0)
        m_display.fillRect(15, 46, map(luminosity, 0, 255, 0, 98), 4, WHITE);

    printCenteredAccents(String(map(luminosity, 0, 255, 0, 100)), 1, 57);
    display();
}

void Display::displayPercentage(String name, int value)
{
    if (!m_operational)
        return;

    resetDisplay();
    printCenteredAccents(name, 1, 20);
    m_display.drawRect(14, 45, 100, 6, WHITE);

    if (value > 0)
        m_display.fillRect(15, 46, value, 4, WHITE);

    printCenteredAccents(String(value), 1, 57);
    display();
}

/// @brief Méthode d'exécution des tâches liées à l'écran : mise en veille de l'écran au bout d'un certain temps.
void Display::loop()
{
    if (m_operational && (m_lastTime != 0) && ((millis() - m_lastTime) >= 15000))
    {
        m_lastTime = 0;
        resetDisplay();
        m_display.display();
        m_menuHelpList = nullptr;
        m_menuHelpMenu = 1;
    }
}

void Display::printAccents(const String &string)
{
    int stringBeginIndex = 0;

    for (unsigned int i = 0; i < string.length(); i++)
    {
        String c = string.substring(i, i + 2);
        if (c != "é" && c != "à" && c != "è" && c != "ù" && c != "â" && c != "ê" && c != "î" && c != "ô" && c != "û" && c != "ç")
            continue;

        m_display.print(string.substring(stringBeginIndex, i));
        stringBeginIndex = i + 2;

        if (c == "é")
            m_display.write(0x82);

        else if (c == "à")
            m_display.write(0x85);

        else if (c == "è")
            m_display.write(0x8A);

        else if (c == "ù")
            m_display.write(0x97);

        else if (c == "â")
            m_display.write(0x83);

        else if (c == "ê")
            m_display.write(0x88);

        else if (c == "î")
            m_display.write(0x8C);

        else if (c == "ô")
            m_display.write(0x93);

        else if (c == "û")
            m_display.write(0x96);

        else if (c == "ç")
            m_display.write(0x87);
    }

    m_display.print(string.substring(stringBeginIndex, string.length()));
}

void Display::printCenteredAccents(const String &string, int textSize, int y)
{
    int stringLength = string.length();
    for (unsigned int i = 0; i < string.length(); i++)
    {
        String c = string.substring(i, i + 2);
        if (c != "é" && c != "à" && c != "è" && c != "ù" && c != "â" && c != "ê" && c != "î" && c != "ô" && c != "û" && c != "ç")
            continue;

        stringLength--;
    }

    m_display.setCursor(ceil((128.0 - double((6 * textSize) * stringLength)) / 2), y);
    m_display.setTextWrap(false);
    m_display.setTextSize(textSize);
    printAccents(string);
}

void Display::resetDisplay(bool resetHelpMenu)
{
    m_display.clearDisplay();
    m_display.setCursor(0, 0);
    m_display.setTextColor(WHITE);
    m_display.setTextWrap(true);
    m_display.setTextSize(1);

    if (resetHelpMenu)
    {
        m_menuHelpList = nullptr;
        m_menuHelpMenu = 1;
    }
}

void Display::display()
{
    m_display.display();
    m_lastTime = millis();
}