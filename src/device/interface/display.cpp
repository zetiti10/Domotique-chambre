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
#include "display.hpp"
#include "device/device.hpp"
#include "bitmaps.hpp"
#include "device/output/television.hpp"
#include "utils/readPROGMEMString.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
Display::Display(const __FlashStringHelper *friendlyName, unsigned int ID) : Device(friendlyName, ID), m_display(128, 64, &Wire, -1), m_lastTime(0), m_lastStateAnimation(0), m_menuHelpList(nullptr), m_menuHelpMenu(1) {}

/// @brief Initialise l'objet.
void Display::setup()
{
    if (m_operational || !m_display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
        return;

    m_display.clearDisplay();
    m_display.cp437(true);
    m_display.setTextColor(WHITE);
    this->displayMessage("Initialisation en cours...");
    m_operational = true;
}

/// @brief Affiche la liste des périphériques indisponibles.
/// @param deviceList Un pointeur vers la liste à vérifier.
/// @param devicesNumber Le nombre d'éléments de la liste.
void Display::displayUnavailableDevices(Device *deviceList[], int &devicesNumber)
{
    if (!m_operational)
        return;

    this->resetDisplay();

    // Vérification pour chaque périphérique de la liste s'il est indisponible.
    int counter = 0;
    for (int i = 0; i < devicesNumber; i++)
    {
        if (!deviceList[i]->getAvailability())
            counter++;
    }

    // Si aucune erreur n'a été détectée, affichage d'un message.
    if (counter == 0)
        this->displayMessage("Démarré !");

    // Affichage de la liste des périphériques indisponibles.
    else
    {
        m_display.setCursor(0, 0);
        this->displayMessage("", "ERREURS");

        counter = 0;
        for (int i = 0; i < devicesNumber; i++)
        {
            if (!deviceList[i]->getAvailability())
            {
                m_display.setCursor(0, counter * 10 + 10);
                this->printAccents(deviceList[i]->getFriendlyName());
                counter++;
            }
        }

        this->display();
    }
}

/// @brief Affiche un pictogramme de cloche à l'écran.
void Display::displayBell()
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, bellBitmap, 128, 64, WHITE);
    this->display();
}

/// @brief Affiche un message à l'écran avec un titre centré.
/// @param message Le message à afficher.
/// @param title Le titre du message (par défaut `INFO`).
void Display::displayMessage(const String &message, const String &title)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    this->printCenteredAccents(title, 1, 0);
    m_display.setCursor(0, 10);
    this->printAccents(message);
    this->display();
}

/// @brief Affiche le volume actuel.
/// @param action Le type de message à afficher (modifie le picrogramme ainsi que les informations affichées ; par défaut, le volume actuel).
/// @param volume Le volume actuel (par défaut `0`, pour les actions qui n'affichent pas le volume actuel).
void Display::displayVolume(VolumeType action, int volume)
{
    if (!m_operational)
        return;

    this->resetDisplay();

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

        this->printCenteredAccents(String(volume), 1, 57);
    }

    this->display();
}

/// @brief Affiche un pictogramme d'alerte (point d'exclamation).
/// @param colorsInverted Inversion ou non des couleurs de l'écran.
void Display::displayAlarmTriggered(bool colorsInverted)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, alarmTriggeredBitmap, 128, 64, 1);

    if (colorsInverted)
        m_display.invertDisplay(true);

    this->display();
}

/// @brief Affiche les valeurs des capteurs de température et d'humidité, avec deux pictogrammes.
/// @param temperature La valeur de la température à afficher.
/// @param humidity La valeur de l'humidité à afficher.
void Display::displayAirValues(float temperature, float humidity)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, airBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);
    // Affichage de la température.
    m_display.setCursor(40, 42);
    m_display.print(temperature);
    // Code pour insérer un "°".
    m_display.write(0xF8);
    m_display.print(F("C"));
    // Affichage de l'humidité.
    m_display.setCursor(40, 10);
    m_display.print(humidity);
    // Code pour insérer un "%".
    m_display.write(0x25);
    this->display();
}

/// @brief Affiche la valeur d'un capteur analogique.
/// @param value La valeur du capteur analogique.
void Display::displayAnalogSensorValue(int value)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, analogSensorBitmap, 128, 64, WHITE);
    this->printCenteredAccents(String(value), 2, 45);
    this->display();
}

/// @brief Affiche la valeur d'un capteur binaire.
/// @param value La valeur du capteur binaire.
void Display::displayBinarySensorValue(bool value)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, binarySensorBitmap, 128, 64, WHITE);
    this->printCenteredAccents(String(value), 2, 45);
    this->display();
}

/// @brief Affiche trois jauges correspondant à l'intensité des trois couleurs.
/// @param r La valeur de l'intensité du rouge.
/// @param g La valeur de l'intensité du vert.
/// @param b La valeur de l'intensité du bleu.
void Display::displayLEDState(int r, int g, int b)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.setTextSize(2);
    // Rouge.
    m_display.setCursor(22, 0);
    m_display.print(F("R"));
    m_display.fillRect(25, 18, 4, int(map(r, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(24, 17, 6, 47, WHITE);
    // Vert.
    m_display.setCursor(62, 0);
    m_display.print(F("V"));
    m_display.fillRect(65, 18, 4, int(map(g, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(64, 17, 6, 47, WHITE);
    // Bleu.
    m_display.setCursor(101, 0);
    m_display.print(F("B"));
    m_display.fillRect(102, 18, 4, int(map(b, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(101, 17, 6, 47, WHITE);
    this->display();
}

/// @brief Affiche une animation illustrant la mise en marche ou l'arrêt d'un périphérique.
/// @param on Mise en marche ou errêt.
void Display::displayDeviceState(bool on)
{
    if (!m_operational || (millis() - m_lastStateAnimation) <= 500)
        return;

    this->resetDisplay();

    // Animation de mise en marche.
    if (on)
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
    this->display();
}

/// @brief Affiche le menu actuel du clavier.
/// @param menuIcon L'icon associé au type du menu.
/// @param menuName Le nom du menu.
void Display::displayKeypadMenu(MenuIcons menuIcon, const __FlashStringHelper *menuName)
{
    if (!m_operational)
        return;

    this->resetDisplay();

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

    this->printCenteredAccents(menuName, 1, 53);
    this->display();
}

/// @brief Affiche l'aide d'un menu, avec deux affichages qui s'alternent pour les 10 touches.
/// @param menuHelpList La liste des commandes du menu pour chaque touche, d'une taille de 10 éléments (de 1 à 9, avec le 0 en dernier ; l'élément 0 de la liste correspond à la touche 1).
/// @param menuTitle Le nom du menu.
void Display::displayKeypadMenuHelp(const __FlashStringHelper **menuHelpList, const __FlashStringHelper *menuName)
{
    if (!m_operational)
        return;

    this->resetDisplay(false);

    // Choix de l'affichage de la page 1 ou de la page 2 de l'aide.
    int begin = 0;
    if ((menuHelpList != m_menuHelpList) || (m_menuHelpMenu == 2) || (menuHelpList[5] == nullptr && menuHelpList[6] == nullptr && menuHelpList[7] == nullptr && menuHelpList[8] == nullptr && menuHelpList[9] == nullptr))
        m_menuHelpMenu = 1;

    else
    {
        m_menuHelpMenu = 2;
        begin = 5;
    }

    // Affichage de la page.
    m_display.setCursor(0, 9);
    m_display.setTextWrap(false);

    for (int i = begin; i < (begin + 5); i++)
    {
        if (menuHelpList[i] == nullptr)
            continue;

        if (i < 9)
            m_display.print(i + 1);

        else
            m_display.print(0);

        m_display.print(F(" "));
        this->printAccents(menuHelpList[i]);
        m_display.println();
    }

    m_menuHelpList = menuHelpList;
    m_display.setCursor(0, 0);
    m_display.print(F("? "));
    this->printAccents(menuName);
    m_display.setTextWrap(true);
    this->display();
}

/// @brief Affiche l'animation d'ouverture ou de fermeture du plateau.
/// @param on Ouverture ou fermeture du plateau.
/// @param shareInformation Affichage ou non de l'animation (permet d'obtenir un délai sans rien afficher).
void Display::displayTray(bool on, bool shareInformation)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.fillRoundRect(5, -10, 118, 20, 5, WHITE);

    if (on)
    {
        for (int i = 0; i < 40; i++)
        {
            m_display.drawLine(28, 11 + i, 100, 11 + i, WHITE);
            if (shareInformation)
                m_display.display();

            else
                delay(15);

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

            else
                delay(15);

            delay(15);
        }
    }

    if (shareInformation)
        this->display();
}

/// @brief Affiche la température de couleur actuelle d'une ampoule.
/// @param minimum La température minimum en kelvin.
/// @param maximum La température maximum en kelvin.
/// @param temperature La température actuelle en kelvin.
void Display::displayLightColorTemperature(int minimum, int maximum, int temperature)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, lightColorTemperatureBitmap, 128, 64, WHITE);
    m_display.drawRect(14, 45, 100, 6, WHITE);

    if (temperature > minimum)
        m_display.fillRect(15, 46, map(temperature, minimum, maximum, 0, 98), 4, WHITE);

    String text = String(temperature) + "K";
    this->printCenteredAccents(text, 1, 57);
    this->display();
}

/// @brief Affiche la luminosité actuelle d'une ampoule.
/// @param luminosity La luminosité de `0` à `255`.
void Display::displayLuminosity(int luminosity)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    m_display.drawBitmap(0, 0, lightLuminosityBitmap, 128, 64, WHITE);
    m_display.drawRect(14, 45, 100, 6, WHITE);

    if (luminosity > 0)
        m_display.fillRect(15, 46, map(luminosity, 0, 255, 0, 98), 4, WHITE);

    this->printCenteredAccents(String(map(luminosity, 0, 255, 0, 100)), 1, 57);
    this->display();
}

/// @brief Affiche un pourcentage à l'écran.
/// @param name L'intitulé de la valeur.
/// @param value Le pourcentage de la jauge.
void Display::displayPercentage(String name, int value)
{
    if (!m_operational)
        return;

    this->resetDisplay();
    this->printCenteredAccents(name, 1, 20);
    m_display.drawRect(14, 45, 100, 6, WHITE);

    if (value > 0)
        m_display.fillRect(15, 46, value, 4, WHITE);

    this->printCenteredAccents(String(value), 1, 57);
    this->display();
}

/// @brief Affiche à l'écran la liste des musiques disponibles, avec en son centre la musique sélectionnée.
/// @param television La télévision fournissant les musiques.
/// @param musicIndex La position de la musique dans la liste des musiques de la télévision.
void Display::displaySelectedMusic(Television &television, unsigned int musicIndex)
{
    if (!m_operational || musicIndex >= television.getMusicNumber())
        return;

    this->resetDisplay();
    this->printCenteredAccents("Musiques", 1, 0);

    // Affiche la musique sélectionnée.
    m_display.setCursor(0, 25);
    m_display.setTextWrap(false);
    Music music = television.getMusicFromIndex(musicIndex);
    this->printAccents("-> " + readProgmemString(music.friendlyName));

    // Affiche la musique précédente.
    if (musicIndex > 0)
    {
        m_display.setCursor(0, 17);
        Music previousMusic = television.getMusicFromIndex(musicIndex - 1);
        this->printAccents(String(musicIndex) + ". " + readProgmemString(previousMusic.friendlyName));
    }

    // Affiche la musique suivante.
    if (musicIndex < (television.getMusicNumber() - 1))
    {
        m_display.setCursor(0, 33);
        Music nextMusic = television.getMusicFromIndex(musicIndex + 1);
        this->printAccents(String(musicIndex + 2) + ". " + readProgmemString(nextMusic.friendlyName));
    }

    m_display.setTextWrap(true);
    this->display();
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

/// @brief Méthode d'arrêt des services de l'écran.
void Display::shutdown()
{
    this->resetDisplay();
    m_display.display();
}

/// @brief Méthode permettant d'afficher un texte intégrant des accents et autres caractères spéciaux. Le texte est imprimé en suivant les instructions précédentes (taille du texte, position...).
/// @param string Le texte à afficher et pouvant inclure des accents.
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

/// @brief Affiche un texte contenant des accents centré verticalement.
/// @param string Le texte à afficher et pouvant inclure des accents.
/// @param textSize La taile du texte.
/// @param y La position horizontale du texte.
void Display::printCenteredAccents(const String &string, int textSize, int y)
{
    // Un caractère spécial est compté comme double : il faut donc reprendre la bonne longueur.
    int stringLength = string.length();
    for (unsigned int i = 0; i < string.length(); i++)
    {
        String c = string.substring(i, i + 2);
        if (c != "é" && c != "à" && c != "è" && c != "ù" && c != "â" && c != "ê" && c != "î" && c != "ô" && c != "û" && c != "ç")
            continue;

        stringLength--;
    }

    // Affichage du texte.
    m_display.setCursor(ceil((128.0 - double((6 * textSize) * stringLength)) / 2), y);
    m_display.setTextWrap(false);
    m_display.setTextSize(textSize);
    this->printAccents(string);
    m_display.setTextWrap(true);
}

/// @brief Réinitialise l'écran actuel de la mémoire tampon, avec les paramètres par défaut.
/// @param resetHelpMenu
void Display::resetDisplay(bool resetHelpMenu)
{
    m_display.clearDisplay();
    m_display.setCursor(0, 0);
    m_display.setTextColor(WHITE);
    m_display.setTextWrap(true);
    m_display.invertDisplay(false);
    m_display.setTextSize(1);

    if (resetHelpMenu)
    {
        m_menuHelpList = nullptr;
        m_menuHelpMenu = 1;
    }
}

/// @brief Affiche le contenu de la mémoire tampon proprement.
void Display::display()
{
    m_display.display();
    m_lastTime = millis();
}