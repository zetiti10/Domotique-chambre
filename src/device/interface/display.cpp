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
Display::Display(String friendlyName, int ID) : Device(friendlyName, ID), m_display(128, 64, &Wire, -1), m_lastTime(0), m_lastStateAnimation(0) {}

/// @brief Initialise l'objet.
void Display::setup()
{
    if (m_operational)
        return;

    if (m_display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
    {
        m_display.clearDisplay();
        m_display.cp437(true);
        m_display.setTextColor(WHITE);

        m_operational = true;
    }
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
        displayMessage("Initialisation terminée !");
    }

    // Affichage de la liste des périphériques indisponibles.
    else
    {
        printAccents("ERREURS");
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
void Display::displayMessage(String message, String title)
{
    if (!m_operational)
        return;

    resetDisplay();
    printAccents(title);
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

        if (volume < 10)
            m_display.setCursor(58, 57);

        else
            m_display.setCursor(55, 57);

        m_display.print(volume);
        // Code pour insérer un "♫".
        m_display.write(0x0E);
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
    m_display.print("C");

    m_display.setCursor(40, 10);
    m_display.print(humidity);
    // Code pour insérer un "%".
    m_display.write(0x25);

    display();
}

/// @brief Affiche la valeur du capteur de luminosité, avec un pictogramme.
/// @param luminosity La valeur de la luminosité à afficher.
void Display::displayLuminosityValue(int luminosity)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, lightLuminosityBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);

    if (luminosity > 10)
        m_display.setCursor(45, 58);

    else if (luminosity < 100)
        m_display.setCursor(45, 52);

    else
        m_display.setCursor(45, 45);

    m_display.print(luminosity);

    display();
}

/// @brief Affiche la valeur du capteur de mouvement, avec un pictogramme.
/// @param motionDetected La valeur du capteur de mouvement à afficher.
void Display::displayMotionSensorValue(bool motionDetected)
{
    if (!m_operational)
        return;

    resetDisplay();
    m_display.drawBitmap(0, 0, motionBitmap, 128, 64, WHITE);
    m_display.setCursor(45, 50);
    m_display.setTextSize(2);

    if (motionDetected)
        m_display.print("OUI");

    else
        m_display.print("NON");

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
    m_display.print("R");
    m_display.fillRect(25, 18, 4, int(map(r, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(24, 17, 6, 47, WHITE);

    m_display.setCursor(62, 0);
    m_display.print("V");
    m_display.fillRect(65, 18, 4, int(map(g, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(64, 17, 6, 47, WHITE);

    m_display.setCursor(101, 0);
    m_display.print("B");
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

    case INPUTS:
        m_display.drawBitmap(0, 0, sensorsMenuBitmap, 128, 64, WHITE);
        break;

    case LIGHTS:
        m_display.drawBitmap(0, 0, lightsMenuBitmap, 128, 64, WHITE);
        break;

    case SETTINGS:
        m_display.drawBitmap(0, 0, configurationMenuBitmap, 128, 64, WHITE);
        break;

    case TELEVISIONS:
        m_display.drawBitmap(0, 0, TVMenuBitmap, 128, 64, WHITE);
        break;

    case CONTROL:
        m_display.drawBitmap(0, 0, deviceControlMenuBitmap, 128, 64, WHITE);
        break;
    }

    String text;

    if (menuName.length() > (18 - 5))
        text = menuName;

    else
        text = "Menu " + menuName;

    m_display.setCursor(((128 - (7 * text.length())) / 2), 55);
    m_display.setTextWrap(false);
    printAccents(text);
    display();
}

/*/// @brief Affiche les informations sur le menu sélectionné.
void Display::displayKeypadMenu()
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.setTextSize(1);
    m_display.setCursor(0, 0);

    switch (keypadMenu)
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
    }

    m_display.display();
    m_lastTime = millis();
}*/

/// @brief Affiche l'aide d'un menu, avec deux affichages qui s'alternent.
/// @param menuHelpList La liste des commandes du menu pour chaque touche, d'une taille de 10 éléments (de 1 à 9, avec le 0 en dernier).
/// @param menuTitle Le nom du menu.
void Display::displayKeypadMenuHelp(String *menuHelpList, String &menuName)
{
    if (!m_operational)
        return;

    resetDisplay();

    int begin = 0;

    if ((menuHelpList != m_menuHelpList) || (m_menuHelpMenu == 2))
        m_menuHelpMenu = 1;

    else
    {
        m_menuHelpMenu = 2;
        begin = 5;
    }

    for (int i = begin; i < (begin + 5); i++)
    {
        m_display.setCursor(0, (9 * i + 9));
        m_display.setTextWrap(false);
        m_display.write(i + 1);
        m_display.write(". ");
        printAccents(menuHelpList[i]);
    }

    m_menuHelpList = menuHelpList;

    m_display.setCursor(0, 0);
    m_display.print("Aide ");
    m_display.print(m_menuHelpMenu);
    m_display.print(" - ");
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

    m_display.setTextSize(1);
    m_display.setCursor(49, 57);
    m_display.print(temperature);
    m_display.print('K');

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

    m_display.setTextSize(1);

    if (luminosity < 10)
        m_display.setCursor(58, 57);

    else if (luminosity < 100)
        m_display.setCursor(55, 57);

    else
        m_display.setCursor(52, 57);

    m_display.print(map(luminosity, 0, 255, 0, 100));
    // Code pour insérer un "%".
    m_display.write(0x25);

    display();
}

/// @brief Méthode d'exécution des tâches liées à l'écran : mise en veille de l'écran au bout d'un certain temps.
void Display::loop()
{
    if (m_operational && (m_lastTime != 0) && ((millis() - m_lastTime) >= 8000))
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
        String c = String(string.charAt(i));
        if (c != "é" && c != "à" && c != "è" && c != "ù" && c != "â" && c != "ê" && c != "î" && c != "ô" && c != "û")
            continue;

        m_display.print(string.substring(stringBeginIndex, i - 1));
        stringBeginIndex = i + 1;

        if (c == "à")
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
    }
}

void Display::resetDisplay()
{
    m_display.clearDisplay();
    m_display.setCursor(0, 0);
    m_display.setTextColor(WHITE);
    m_display.setTextWrap(true);
    m_display.setTextSize(1);
}

void Display::display()
{
    m_display.display();
    m_lastTime = millis();
}