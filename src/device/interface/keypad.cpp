/**
 * @file device/interface/keypad.cpp
 * @author Louis L
 * @brief Classe gérant un petit clavier avec un système de sous-menus.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#include <Adafruit_Keypad.h>

// Autres fichiers du programme.
#include "keypad.hpp"
#include "utils/globalVariables.hpp"
#include "device/device.hpp"
#include "device/interface/display.hpp"
#include "device/output/tray.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/alarm.hpp"
#include "device/output/television.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/analogInput.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param display L'écran utilisé par le système.
/// @param userKeymap Les touches du clavier.
/// @param row Les broches des lignes du clavier.
/// @param col Les broches des colonnes du clavier.
/// @param numRows Le nombre de lignes du clavier.
/// @param numCols Le nombre de colonnes du clavier.
Keypad::Keypad(const __FlashStringHelper *friendlyName, unsigned int ID, Display &display, byte *userKeymap, byte *row, byte *col, int numRows, int numCols) : Device(friendlyName, ID), m_keypad(userKeymap, row, col, numRows, numCols), m_keyPressTime(0), m_lastInteraction(0), m_display(display), m_mainMenu(nullptr), m_currentMenu(nullptr), m_devicesDefined(false) {}

/// @brief Méthode permettant de définir les périphériques contrôlés depuis le clavier. Tous les menus seront crées ici.
/// @param deviceList La liste des périphériques.
/// @param devicesNumber Le nombre de périphériques.
/// @param lightList La liste des lumières.
/// @param lightsNumber Le nombre de lumières.
/// @param RGBLEDStripList La liste des rubans de DEL RVB.
/// @param colorModeList La liste des modes de couleur unique de rubans.
/// @param rainbowModeList La liste des modes arc-en-ciel de rubans.
/// @param soundreactModeList La liste des modes son-réaction de rubans.
/// @param alarmModeList La liste des modes d'alarme de rubans.
/// @param RGBLEDStripsNumber Le nombre de rubans de DEL RVB.
/// @param connectedTemperatureVariableLightList La liste des lumières distantes à température de couleur variable.
/// @param connectedTemperatureVariableLightsNumber Le nombre de lumières distantes à température de couleur variable.
/// @param connectedColorVariableLightList La liste des lumières distantes à couleur variable.
/// @param connectedColorVariableLightsNumber Le nombre de lumières distantes à couleur variable.
/// @param televisionList La liste des télévisions.
/// @param televisionsNumber Le nombre de télévisions.
/// @param alarmList La liste des alarmes.
/// @param alarmsNumber Le nombre de alarmes.
/// @param binaryInputList La liste des capteurs binaires.
/// @param binaryInputsNumber Le nombre de capteurs binaires.
/// @param analogInputList La liste des capteurs analogiques.
/// @param analogInputsNumber Le nombre de capteurs analogiques.
/// @param airSensorList La liste des capteurs d'air.
/// @param airSensorsNumber Le nombre de capteurs d'air.
/// @param wardrobeDoorSensorList La liste des capteurs de portes d'armoire.
/// @param wardrobeDoorSensorsNumber Le nombre de capteurs de portes d'armoire.
void Keypad::setDevices(Output *deviceList[], int &devicesNumber, Output *lightList[], int &lightsNumber, RGBLEDStrip *RGBLEDStripList[], ColorMode *colorModeList[], RainbowMode *rainbowModeList[], SoundreactMode *soundreactModeList[], AlarmMode *alarmModeList[], int &RGBLEDStripsNumber, ConnectedTemperatureVariableLight *connectedTemperatureVariableLightList[], int &connectedTemperatureVariableLightsNumber, ConnectedColorVariableLight *connectedColorVariableLightList[], int &connectedColorVariableLightsNumber, Television *televisionList[], int &televisionsNumber, Alarm *alarmList[], int &alarmsNumber, BinaryInput *binaryInputList[], int &binaryInputsNumber, AnalogInput *analogInputList[], int &analogInputsNumber, AirSensor *airSensorList[], int &airSensorsNumber, WardrobeDoorSensor *wardrobeDoorSensorList[], int &wardrobeDoorSensorsNumber)
{
    KeypadMenu *previousMenu = nullptr;

    // Création des menus pour le contrôle des périphériques 'basiques'.
    int devicesMenusNumber = ceil(double(devicesNumber) / 9.0);
    for (int i = 0; i < devicesMenusNumber; i++)
    {
        int itemsInCurrentMenu;
        if ((i + 1) == devicesMenusNumber)
            itemsInCurrentMenu = devicesNumber - (i * 9);

        else
            itemsInCurrentMenu = 9;

        Output **devicesInCurrentMenu = new Output *[itemsInCurrentMenu];

        KeypadMenuOutputList *devicesMenu = new KeypadMenuOutputList(F("Périphériques"), *this);

        for (int j = 0; j < itemsInCurrentMenu; j++)
        {
            int deviceIndex = (i * 9) + j;
            devicesInCurrentMenu[j] = deviceList[deviceIndex];
        }

        devicesMenu->setDevices(devicesInCurrentMenu, itemsInCurrentMenu);

        if (m_mainMenu == nullptr)
        {
            m_mainMenu = devicesMenu;
            m_currentMenu = devicesMenu;
        }

        else
        {
            previousMenu->setNextMenu(devicesMenu);
            devicesMenu->setPreviousMenu(previousMenu);
        }

        previousMenu = devicesMenu;
    }

    // Création des menus pour le contrôle des lumières.
    int lightsTotalNumber = lightsNumber + RGBLEDStripsNumber + connectedTemperatureVariableLightsNumber + connectedColorVariableLightsNumber;
    int lightsMenusNumber = ceil(double(lightsTotalNumber) / 9.0);
    for (int i = 0; i < lightsMenusNumber; i++)
    {
        int itemsInCurrentMenu;
        if ((i + 1) == lightsMenusNumber)
            itemsInCurrentMenu = lightsTotalNumber - (i * 9);

        else
            itemsInCurrentMenu = 9;

        Output **lightsInCurrentMenu = new Output *[itemsInCurrentMenu];
        KeypadMenu **subMenusInCurrentMenu = new KeypadMenu *[itemsInCurrentMenu];

        KeypadMenuLightList *lightsMenu = new KeypadMenuLightList(F("Lumières"), *this);

        for (int j = 0; j < itemsInCurrentMenu; j++)
        {
            int deviceIndex = (i * 9) + j;

            if (deviceIndex < lightsNumber)
            {
                lightsInCurrentMenu[j] = lightList[deviceIndex];
                subMenusInCurrentMenu[j] = nullptr;
            }

            else if (deviceIndex < (lightsNumber + RGBLEDStripsNumber))
            {
                int index = deviceIndex - lightsNumber;
                RGBLEDStrip *strip = RGBLEDStripList[index];

                KeypadMenuRGBLEDStripControl *menu = new KeypadMenuRGBLEDStripControl(strip->getFriendlyName(), *this);
                menu->setStrip(strip, colorModeList[index], rainbowModeList[index], soundreactModeList[index], alarmModeList[index]);
                menu->setParentMenu(lightsMenu);

                lightsInCurrentMenu[j] = strip;
                subMenusInCurrentMenu[j] = menu;
            }

            else if (deviceIndex < (lightsNumber + RGBLEDStripsNumber + connectedTemperatureVariableLightsNumber))
            {
                int index = deviceIndex - (lightsNumber + RGBLEDStripsNumber);
                ConnectedTemperatureVariableLight *light = connectedTemperatureVariableLightList[index];

                KeypadMenuConnectedTemperatureVariableLightControl *menu = new KeypadMenuConnectedTemperatureVariableLightControl(light->getFriendlyName(), *this);
                menu->setLight(*light);
                menu->setParentMenu(lightsMenu);

                lightsInCurrentMenu[j] = light;
                subMenusInCurrentMenu[j] = menu;
            }

            else if (deviceIndex < (lightsNumber + RGBLEDStripsNumber + connectedTemperatureVariableLightsNumber + connectedColorVariableLightsNumber))
            {
                int index = deviceIndex - (lightsNumber + RGBLEDStripsNumber + connectedTemperatureVariableLightsNumber);
                ConnectedColorVariableLight *light = connectedColorVariableLightList[index];

                KeypadMenuConnectedColorVariableLightControl *menu = new KeypadMenuConnectedColorVariableLightControl(light->getFriendlyName(), *this);
                menu->setLight(*light);
                menu->setParentMenu(lightsMenu);

                lightsInCurrentMenu[j] = light;
                subMenusInCurrentMenu[j] = menu;
            }
        }

        lightsMenu->setLights(lightsInCurrentMenu, subMenusInCurrentMenu, itemsInCurrentMenu);

        if (m_mainMenu == nullptr)
        {
            m_mainMenu = lightsMenu;
            m_currentMenu = lightsMenu;
        }

        else
        {
            previousMenu->setNextMenu(lightsMenu);
            lightsMenu->setPreviousMenu(previousMenu);
        }

        previousMenu = lightsMenu;
    }

    // Création des menus pour les télévisions.
    for (int i = 0; i < televisionsNumber; i++)
    {
        Television *television = televisionList[i];

        KeypadMenuTelevision *televisionMenu = new KeypadMenuTelevision(television->getFriendlyName(), *this);
        televisionMenu->setTelevision(television);

        if (m_mainMenu == nullptr)
        {
            m_mainMenu = televisionMenu;
            m_currentMenu = televisionMenu;
        }

        else
        {
            previousMenu->setNextMenu(televisionMenu);
            televisionMenu->setPreviousMenu(previousMenu);
        }

        KeypadMenuTelevisionMusicSelector *musicSelector = new KeypadMenuTelevisionMusicSelector(F("Musiques"), *this);
        musicSelector->setTelevision(television);
        musicSelector->setParentMenu(televisionMenu);

        televisionMenu->setMusicSelectionMenu(musicSelector);

        previousMenu = televisionMenu;
    }

    // Création des menus pour les Alarmes.
    for (int i = 0; i < alarmsNumber; i++)
    {
        Alarm *alarm = alarmList[i];

        KeypadMenuAlarm *alarmMenu = new KeypadMenuAlarm(alarm->getFriendlyName(), *this);
        alarmMenu->setAlarm(alarm);

        if (m_mainMenu == nullptr)
        {
            m_mainMenu = alarmMenu;
            m_currentMenu = alarmMenu;
        }

        else
        {
            previousMenu->setNextMenu(alarmMenu);
            alarmMenu->setPreviousMenu(previousMenu);
        }

        KeypadMenuAlarmMissileLauncherControl *missileLanucherControl = new KeypadMenuAlarmMissileLauncherControl(F("Lance-missile"), *this);
        missileLanucherControl->setAlarm(alarm);
        missileLanucherControl->setParentMenu(alarmMenu);

        alarmMenu->setMissileLauncherControlMenu(missileLanucherControl);

        previousMenu = alarmMenu;
    }

    // Création des menus pour les capteurs.
    int inputsNumber = binaryInputsNumber + analogInputsNumber + airSensorsNumber + wardrobeDoorSensorsNumber;
    int inputsMenusNumber = ceil(double(inputsNumber) / 9.0);
    for (int i = 0; i < inputsMenusNumber; i++)
    {
        int itemsInCurrentMenu;
        if ((i + 1) == inputsMenusNumber)
            itemsInCurrentMenu = inputsNumber - (i * 9);

        else
            itemsInCurrentMenu = 9;

        Input **sensorsInCurrentMenu = new Input *[itemsInCurrentMenu];
        KeypadMenu **subMenusInCurrentMenu = new KeypadMenu *[itemsInCurrentMenu];
        KeypadMenuSensorType *sensorsTypesInCurrentMenu = new KeypadMenuSensorType[itemsInCurrentMenu];

        KeypadMenuInputList *sensorsMenu = new KeypadMenuInputList(F("Capteurs"), *this);

        for (int j = 0; j < itemsInCurrentMenu; j++)
        {
            int sensorIndex = (i * 9) + j;

            if (sensorIndex < binaryInputsNumber)
            {
                sensorsInCurrentMenu[j] = binaryInputList[sensorIndex];
                subMenusInCurrentMenu[j] = nullptr;
                sensorsTypesInCurrentMenu[j] = BINARY;
            }

            else if (sensorIndex < (binaryInputsNumber + analogInputsNumber))
            {
                int index = sensorIndex - binaryInputsNumber;

                sensorsInCurrentMenu[j] = analogInputList[index];
                subMenusInCurrentMenu[j] = nullptr;
                sensorsTypesInCurrentMenu[j] = ANALOG;
            }

            else if (sensorIndex < (binaryInputsNumber + analogInputsNumber + airSensorsNumber))
            {
                int index = sensorIndex - (binaryInputsNumber + analogInputsNumber);

                sensorsInCurrentMenu[j] = airSensorList[index];
                subMenusInCurrentMenu[j] = nullptr;
                sensorsTypesInCurrentMenu[j] = AIR;
            }

            else if (sensorIndex < (binaryInputsNumber + analogInputsNumber + airSensorsNumber + wardrobeDoorSensorsNumber))
            {
                int index = sensorIndex - (binaryInputsNumber + analogInputsNumber + airSensorsNumber);
                WardrobeDoorSensor *sensor = wardrobeDoorSensorList[index];

                KeypadMenuWardrobeControl *menu = new KeypadMenuWardrobeControl(sensor->getFriendlyName(), *this, *sensor);
                menu->setParentMenu(sensorsMenu);

                sensorsInCurrentMenu[j] = sensor;
                subMenusInCurrentMenu[j] = menu;
                sensorsTypesInCurrentMenu[j] = WARDROBE_DOOR_SENSOR;
            }
        }

        sensorsMenu->setInputs(sensorsInCurrentMenu, subMenusInCurrentMenu, sensorsTypesInCurrentMenu, itemsInCurrentMenu);

        if (m_mainMenu == nullptr)
        {
            m_mainMenu = sensorsMenu;
            m_currentMenu = sensorsMenu;
        }

        else
        {
            previousMenu->setNextMenu(sensorsMenu);
            sensorsMenu->setPreviousMenu(previousMenu);
        }

        previousMenu = sensorsMenu;
    }

    // Création du menu des paramètres.
    KeypadMenuSettings *menu = new KeypadMenuSettings(F("Paramètres"), *this);
    menu->setPreviousMenu(previousMenu);
    previousMenu->setNextMenu(menu);
    previousMenu = menu;

    m_devicesDefined = true;
}

/// @brief Initialise le clavier. Méthode à appeler après la définition des périphériques.
void Keypad::setup()
{
    if (m_operational || !m_devicesDefined || m_currentMenu == nullptr)
        return;

    m_keypad.begin();
    m_display.setup();

    m_operational = true;
}

/// @brief Méthode d'exécution des tâches liées au clavier.
void Keypad::loop()
{
    if (!m_operational)
        return;

    m_keypad.tick();

    // Retour au menu principal après un certain délais sans interraction.
    if (!(m_currentMenu == m_mainMenu) && ((m_lastInteraction + 600000) <= millis()))
        setMenu(m_mainMenu);

    // Si une action est détectée.
    while (m_keypad.available())
    {
        keypadEvent event = m_keypad.read();

        // Touche pressée.
        if (event.bit.EVENT == KEY_JUST_PRESSED)
        {
            if (m_currentMenu->advancedClickControl())
            {
                if (event.bit.KEY >= '1' && event.bit.KEY <= '9')
                    m_currentMenu->keyReleased(event.bit.KEY, false);
            }

            if (m_keyPressTime != 0)
                continue;

            m_keyPressTime = millis();
        }

        // Touche relâchée.
        else
        {
            if (m_keyPressTime == 0)
                continue;

            // Calcul du temps de clique.
            bool longClick = ((millis() - m_keyPressTime) >= 300);

            if (event.bit.KEY >= '1' && event.bit.KEY <= '9')
            {
                if (m_currentMenu->advancedClickControl())
                    m_currentMenu->keyPressed(event.bit.KEY);

                else
                    m_currentMenu->keyReleased(event.bit.KEY, longClick);
            }

            // Exécution des actions.
            else
            {
                switch (event.bit.KEY)
                {
                case '*':
                    m_currentMenu->displayMenu();
                    break;

                case '#':
                    m_currentMenu->displayHelp();
                    break;

                case 'A':
                    setMenu(m_mainMenu, true);
                    break;

                case 'B':
                    if (m_currentMenu->getParentMenu() != nullptr)
                        setMenu(m_currentMenu->getParentMenu(), true);

                    break;

                case 'C':
                    if (m_currentMenu->getPreviousMenu() != nullptr)
                        setMenu(m_currentMenu->getPreviousMenu(), true);

                    break;

                case 'D':
                    if (m_currentMenu->getNextMenu() != nullptr)
                        setMenu(m_currentMenu->getNextMenu(), true);

                    break;
                }
            }

            m_keyPressTime = 0;
            m_lastInteraction = millis();
        }
    }
}

/// @brief Méthode permettant de récupèrer l'écran du système.
/// @return L'écran du système.
Display &Keypad::getDisplay()
{
    return m_display;
}

/// @brief Méthode permettant de définir le menu actuel.
/// @param menu Un pointeur vers le menu à choisir.
/// @param shareInformation Affiche ou non le changement de menu à l'écran.
void Keypad::setMenu(KeypadMenu *menu, bool shareInformation)
{
    m_currentMenu = menu;

    if (shareInformation)
        m_currentMenu->displayMenu();
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du clavier.
/// @param keypad L'objet du clavier.
KeypadMenu::KeypadMenu(const __FlashStringHelper *friendlyName, Keypad &keypad) : m_friendlyName(friendlyName), m_keypad(keypad), m_parentMenu(nullptr), m_previousMenu(nullptr), m_nextMenu(nullptr) {}

/// @brief Méthode permettant d'obtenir le nom du clavier.
/// @return Le nom du clavier.
const __FlashStringHelper *KeypadMenu::getFriendlyName() const
{
    return m_friendlyName;
}

/// @brief Méthode permettant de définir le menu parent de ce menu (accessible avec la touche B).
/// @param menu Le menu parent.
void KeypadMenu::setParentMenu(KeypadMenu *menu)
{
    m_parentMenu = menu;
}

/// @brief Méthode permettant de récupérer le menu parent.
/// @return Un pointeur vers le menu parent.
KeypadMenu *KeypadMenu::getParentMenu() const
{
    return m_parentMenu;
}

/// @brief Méthode permettant de définir le menu précédent (accessible avec la touche C).
/// @param menu Le menu précédent.
void KeypadMenu::setPreviousMenu(KeypadMenu *menu)
{
    m_previousMenu = menu;
}

/// @brief Méthode permettant de récupérer le menu précédent.
/// @return Un pointeur vers le menu précédent.
KeypadMenu *KeypadMenu::getPreviousMenu() const
{
    return m_previousMenu;
}

/// @brief Méthode permettant de définir le menu suivant (accessible avec la touche D).
/// @param menu Le menu suivant.
void KeypadMenu::setNextMenu(KeypadMenu *menu)
{
    m_nextMenu = menu;
}

/// @brief Méthode permettant de récupérer le menu suivant.
/// @return Un pointeur vers le menu suivant.
KeypadMenu *KeypadMenu::getNextMenu() const
{
    return m_nextMenu;
}

/// @brief Méthode appelée lorsque ce menu est actuellement utilisé, et qu'une touche a été pressée.
/// @param key La touche.
void KeypadMenu::keyPressed(char key)
{
}

/// @brief Méthode permettant de définir le type de menu : le contrôle avancé permet ud'avoir un appel à la méthode `void KeypadMenu::keyPressed(char key)` lorsqu'une touche est pressée.
/// @return Si le menu est avancé ou non.
bool KeypadMenu::advancedClickControl()
{
    return false;
}

// À partir d'ici, les classes correspondent à des types de menus différents.

KeypadMenuOutputList::KeypadMenuOutputList(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_outputList(nullptr), m_outputsNumber(0) {}

void KeypadMenuOutputList::setDevices(Output *outputList[], int &outputsNumber)
{
    m_outputList = outputList;
    m_outputsNumber = outputsNumber;
}

void KeypadMenuOutputList::keyReleased(char key, bool longClick)
{
    if ((key - '0' - 1) < m_outputsNumber)
        m_outputList[key - '0' - 1]->toggle(true);
}

void KeypadMenuOutputList::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    for (int i = 0; i < m_outputsNumber; i++)
        help[i] = m_outputList[i]->getFriendlyName();

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuOutputList::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(OUTPUTS, m_friendlyName);
}

KeypadMenuLightList::KeypadMenuLightList(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_lightList(nullptr), m_lightMenuList(nullptr), m_lightsNumber(0) {}

void KeypadMenuLightList::setLights(Output *lightList[], KeypadMenu *lightMenuList[], int &lightsNumber)
{
    m_lightList = lightList;
    m_lightMenuList = lightMenuList;
    m_lightsNumber = lightsNumber;
}

void KeypadMenuLightList::keyReleased(char key, bool longClick)
{
    int index = key - '0' - 1;
    if (index < m_lightsNumber)
    {
        if (!longClick)
            m_lightList[index]->toggle(true);

        else if (!(m_lightMenuList[index] == nullptr))
            m_keypad.setMenu(m_lightMenuList[index], true);
    }
}

void KeypadMenuLightList::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    for (int i = 0; i < m_lightsNumber; i++)
        help[i] = m_lightList[i]->getFriendlyName();

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuLightList::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(LIGHTS, m_friendlyName);
}

KeypadMenuRGBLEDStripControl::KeypadMenuRGBLEDStripControl(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_strip(nullptr), m_colorMode(nullptr), m_rainbowMode(nullptr), m_soundreactMode(nullptr), m_alarmMode(nullptr) {}

void KeypadMenuRGBLEDStripControl::setStrip(RGBLEDStrip *strip, ColorMode *colorMode, RainbowMode *rainbowMode, SoundreactMode *soundreactMode, AlarmMode *alarmMode)
{
    m_strip = strip;

    m_colorMode = new KeypadMenuRGBLEDStripColorModeControl(colorMode->getFriendlyName(), m_keypad, *colorMode);
    m_colorMode->setParentMenu(this);
    m_rainbowMode = new KeypadMenuRGBLEDStripRainbowModeControl(rainbowMode->getFriendlyName(), m_keypad, *rainbowMode);
    m_rainbowMode->setParentMenu(this);
    m_soundreactMode = new KeypadMenuRGBLEDStripSoundreactModeControl(soundreactMode->getFriendlyName(), m_keypad, *soundreactMode);
    m_soundreactMode->setParentMenu(this);
    m_alarmMode = new KeypadMenuRGBLEDStripAlarmModeControl(alarmMode->getFriendlyName(), m_keypad, *alarmMode);
    m_alarmMode->setParentMenu(this);

    m_colorMode->setNextMenu(m_rainbowMode);
    m_colorMode->setPreviousMenu(m_alarmMode);
    m_rainbowMode->setNextMenu(m_soundreactMode);
    m_rainbowMode->setPreviousMenu(m_colorMode);
    m_soundreactMode->setNextMenu(m_alarmMode);
    m_soundreactMode->setPreviousMenu(m_rainbowMode);
    m_alarmMode->setNextMenu(m_colorMode);
    m_alarmMode->setPreviousMenu(m_soundreactMode);
}

void KeypadMenuRGBLEDStripControl::keyReleased(char key, bool longClick)
{
    if (m_strip == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_strip->toggle(true);
        break;

    case '2':
        if (!longClick)
            m_strip->setMode(&m_colorMode->getColorMode(), true);

        else
            m_keypad.setMenu(m_colorMode, true);

        break;

    case '3':
        if (!longClick)
            m_strip->setMode(&m_rainbowMode->getRainbowMode(), true);

        else
            m_keypad.setMenu(m_rainbowMode, true);

        break;

    case '4':
        if (!longClick)
            m_strip->setMode(&m_soundreactMode->getSoundreactMode(), true);

        else
            m_keypad.setMenu(m_soundreactMode, true);

        break;

    case '5':
        if (!longClick)
            m_strip->setMode(&m_alarmMode->getAlarmMode(), true);

        else
            m_keypad.setMenu(m_alarmMode, true);

        break;
    }
}

void KeypadMenuRGBLEDStripControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Basculer");
    help[1] = F("Couleur unique");
    help[2] = F("Arc-en-ciel");
    help[3] = F("Son-réaction");
    help[4] = F("Alarme");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripColorModeControl::KeypadMenuRGBLEDStripColorModeControl(const __FlashStringHelper *friendlyName, Keypad &keypad, ColorMode &colorMode) : KeypadMenu(friendlyName, keypad), m_colorMode(colorMode) {}

ColorMode &KeypadMenuRGBLEDStripColorModeControl::getColorMode()
{
    return m_colorMode;
}

void KeypadMenuRGBLEDStripColorModeControl::keyReleased(char key, bool longClick)
{
    int precision = 20;

    switch (key)
    {
    case '1':
    {
        unsigned int newValue = m_colorMode.getR();
        if (int(m_colorMode.getR()) <= (255 - precision))
            newValue += precision;
        
        else
            newValue = 255;

        m_colorMode.setColor(newValue, m_colorMode.getG(), m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }

    case '2':
    {
        unsigned int newValue = m_colorMode.getG();
        if (int(m_colorMode.getG()) <= (255 - precision))
            newValue += precision;
        
        else
            newValue = 255;

        m_colorMode.setColor(m_colorMode.getR(), newValue, m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }

    case '3':
    {
        unsigned int newValue = m_colorMode.getB();
        if (int(m_colorMode.getB()) <= (255 - precision))
            newValue += precision;
        
        else
            newValue = 255;

        m_colorMode.setColor(m_colorMode.getR(), m_colorMode.getG(), newValue);
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }

    case '4':
    {
        unsigned int newValue = m_colorMode.getR();
        if (int(m_colorMode.getR()) >= precision)
            newValue -= precision;
        
        else
            newValue = 0;

        m_colorMode.setColor(newValue, m_colorMode.getG(), m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }

    case '5':
    {
        unsigned int newValue = m_colorMode.getG();
        if (int(m_colorMode.getG()) >= precision)
            newValue -= precision;
        
        else
            newValue = 0;

        m_colorMode.setColor(m_colorMode.getR(), newValue, m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }

    case '6':
    {
        unsigned int newValue = m_colorMode.getB();
        if (int(m_colorMode.getB()) >= precision)
            newValue -= precision;
        
        else
            newValue = 0;

        m_colorMode.setColor(m_colorMode.getR(), m_colorMode.getG(), newValue);
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }
    }
}

void KeypadMenuRGBLEDStripColorModeControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("+ rouge");
    help[1] = F("+ vert");
    help[2] = F("+ bleu");
    help[3] = F("- rouge");
    help[4] = F("- vert");
    help[4] = F("- bleu");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripColorModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripRainbowModeControl::KeypadMenuRGBLEDStripRainbowModeControl(const __FlashStringHelper *friendlyName, Keypad &keypad, RainbowMode &rainbowMode) : KeypadMenu(friendlyName, keypad), m_rainbowMode(rainbowMode) {}

RainbowMode &KeypadMenuRGBLEDStripRainbowModeControl::getRainbowMode()
{
    return m_rainbowMode;
}

void KeypadMenuRGBLEDStripRainbowModeControl::keyReleased(char key, bool longClick)
{
    int precision = 10;

    switch (key)
    {
    case '1':
    {
        unsigned int newValue = m_rainbowMode.getAnimationSpeed();
        if (int(m_rainbowMode.getAnimationSpeed()) <= (100 - precision))
            newValue += precision;

        else
            newValue = 100;

        m_rainbowMode.setAnimationSpeed(newValue);
        m_keypad.getDisplay().displayPercentage("Vitesse", m_rainbowMode.getAnimationSpeed());
        break;
    }

    case '2':
    {
        unsigned int newValue = m_rainbowMode.getAnimationSpeed();
        if (int(m_rainbowMode.getAnimationSpeed()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_rainbowMode.setAnimationSpeed(newValue);
        m_keypad.getDisplay().displayPercentage("Vitesse", m_rainbowMode.getAnimationSpeed());
        break;
    }
    }
}

void KeypadMenuRGBLEDStripRainbowModeControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("+ vite");
    help[1] = F("- vite");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripRainbowModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripSoundreactModeControl::KeypadMenuRGBLEDStripSoundreactModeControl(const __FlashStringHelper *friendlyName, Keypad &keypad, SoundreactMode &soundreactMode) : KeypadMenu(friendlyName, keypad), m_soundreactMode(soundreactMode) {}

SoundreactMode &KeypadMenuRGBLEDStripSoundreactModeControl::getSoundreactMode()
{
    return m_soundreactMode;
}

void KeypadMenuRGBLEDStripSoundreactModeControl::keyReleased(char key, bool longClick)
{
    int precision = 5;

    switch (key)
    {
    case '1':
    {
        unsigned int newValue = m_soundreactMode.getSensitivity();
        if (int(m_soundreactMode.getSensitivity()) <= (100 - precision))
            newValue += precision;

        else
            newValue = 100;

        m_soundreactMode.setSensitivity(newValue);
        m_keypad.getDisplay().displayPercentage("Sensibilité", m_soundreactMode.getSensitivity());
        break;
    }

    case '2':
    {
        unsigned int newValue = m_soundreactMode.getSensitivity();
        if (int(m_soundreactMode.getSensitivity()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_soundreactMode.setSensitivity(newValue);
        m_keypad.getDisplay().displayPercentage("Sensibilité", m_soundreactMode.getSensitivity());
        break;
    }
    }
}

void KeypadMenuRGBLEDStripSoundreactModeControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("+ sensible");
    help[1] = F("- sensible");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripSoundreactModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripAlarmModeControl::KeypadMenuRGBLEDStripAlarmModeControl(const __FlashStringHelper *friendlyName, Keypad &keypad, AlarmMode &alarmMode) : KeypadMenu(friendlyName, keypad), m_alarmMode(alarmMode) {}

AlarmMode &KeypadMenuRGBLEDStripAlarmModeControl::getAlarmMode()
{
    return m_alarmMode;
}

void KeypadMenuRGBLEDStripAlarmModeControl::keyReleased(char key, bool longClick)
{
    // Rien...
}

void KeypadMenuRGBLEDStripAlarmModeControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};
    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripAlarmModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedTemperatureVariableLightControl::KeypadMenuConnectedTemperatureVariableLightControl(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_temperatureMenu(nullptr), m_luminosityMenu(nullptr) {}

void KeypadMenuConnectedTemperatureVariableLightControl::setLight(ConnectedTemperatureVariableLight &light)
{
    m_temperatureMenu = new KeypadMenuConnectedLightTemperatureControl(F("Température"), m_keypad, light);
    m_temperatureMenu->setParentMenu(this);
    m_luminosityMenu = new KeypadMenuConnectedLightLuminosityControl(F("Luminosité"), m_keypad, light);
    m_luminosityMenu->setParentMenu(this);

    m_temperatureMenu->setNextMenu(m_luminosityMenu);
    m_temperatureMenu->setPreviousMenu(m_luminosityMenu);
    m_luminosityMenu->setNextMenu(m_temperatureMenu);
    m_luminosityMenu->setPreviousMenu(m_temperatureMenu);
}

void KeypadMenuConnectedTemperatureVariableLightControl::keyReleased(char key, bool longClick)
{
    if (m_temperatureMenu == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_temperatureMenu->getLight().toggle(true);
        break;

    case '2':
        m_keypad.setMenu(m_temperatureMenu, true);
        break;

    case '3':
        m_keypad.setMenu(m_luminosityMenu, true);
        break;
    }
}

void KeypadMenuConnectedTemperatureVariableLightControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Basculer");
    help[1] = F("Température");
    help[2] = F("Luminosité");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedTemperatureVariableLightControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedColorVariableLightControl::KeypadMenuConnectedColorVariableLightControl(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_temperatureMenu(nullptr), m_luminosityMenu(nullptr), m_colorMenu(nullptr) {}

void KeypadMenuConnectedColorVariableLightControl::setLight(ConnectedColorVariableLight &light)
{
    m_temperatureMenu = new KeypadMenuConnectedLightTemperatureControl(F("Température"), m_keypad, light);
    m_temperatureMenu->setParentMenu(this);
    m_luminosityMenu = new KeypadMenuConnectedLightLuminosityControl(F("Luminosité"), m_keypad, light);
    m_luminosityMenu->setParentMenu(this);
    m_colorMenu = new KeypadMenuConnectedLightColorControl(F("Couleur"), m_keypad, light);
    m_colorMenu->setParentMenu(this);

    m_colorMenu->setNextMenu(m_temperatureMenu);
    m_colorMenu->setPreviousMenu(m_luminosityMenu);
    m_temperatureMenu->setNextMenu(m_luminosityMenu);
    m_temperatureMenu->setPreviousMenu(m_colorMenu);
    m_luminosityMenu->setNextMenu(m_colorMenu);
    m_luminosityMenu->setPreviousMenu(m_temperatureMenu);
}

void KeypadMenuConnectedColorVariableLightControl::keyReleased(char key, bool longClick)
{
    if (m_temperatureMenu == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_temperatureMenu->getLight().toggle(true);
        break;

    case '2':
        m_keypad.setMenu(m_temperatureMenu, true);
        break;

    case '3':
        m_keypad.setMenu(m_luminosityMenu, true);
        break;

    case '4':
        m_keypad.setMenu(m_colorMenu, true);
        break;
    }
}

void KeypadMenuConnectedColorVariableLightControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Basculer");
    help[1] = F("Température");
    help[2] = F("Luminosité");
    help[3] = F("Couleur");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedColorVariableLightControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedLightLuminosityControl::KeypadMenuConnectedLightLuminosityControl(const __FlashStringHelper *friendlyName, Keypad &keypad, ConnectedTemperatureVariableLight &light) : KeypadMenu(friendlyName, keypad), m_light(light) {}

ConnectedTemperatureVariableLight &KeypadMenuConnectedLightLuminosityControl::getLight()
{
    return m_light;
}

void KeypadMenuConnectedLightLuminosityControl::keyReleased(char key, bool longClick)
{
    int precision = 20;

    switch (key)
    {
    case '1':
    {
        unsigned int newValue = m_light.getLuminosity();
        if (int(m_light.getLuminosity()) <= (255 - precision))
            newValue += precision;

        else
            newValue = 255;

        m_light.setLuminosity(newValue, true);
        break;
    }

    case '2':
    {
        unsigned int newValue = m_light.getLuminosity();
        if (int(m_light.getLuminosity()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_light.setLuminosity(newValue, true);
        break;
    }
    }
}

void KeypadMenuConnectedLightLuminosityControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("+ luminosité");
    help[1] = F("- luminosité");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedLightLuminosityControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedLightTemperatureControl::KeypadMenuConnectedLightTemperatureControl(const __FlashStringHelper *friendlyName, Keypad &keypad, ConnectedTemperatureVariableLight &light) : KeypadMenu(friendlyName, keypad), m_light(light) {}

ConnectedTemperatureVariableLight &KeypadMenuConnectedLightTemperatureControl::getLight()
{
    return m_light;
}

void KeypadMenuConnectedLightTemperatureControl::keyReleased(char key, bool longClick)
{
    int precision = 200;

    switch (key)
    {
    case '1':
    {
        unsigned int newValue = m_light.getColorTemperature();
        if (int(m_light.getColorTemperature()) <= (10000 - precision))
            newValue += precision;

        else
            newValue = 10000;

        m_light.setColorTemperature(newValue, true);
        break;
    }

    case '2':
    {
        unsigned int newValue = m_light.getColorTemperature();
        if (int(m_light.getColorTemperature()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_light.setColorTemperature(newValue, true);
        break;
    }
    }
}

void KeypadMenuConnectedLightTemperatureControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("+ température");
    help[1] = F("- température");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedLightTemperatureControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedLightColorControl::KeypadMenuConnectedLightColorControl(const __FlashStringHelper *friendlyName, Keypad &keypad, ConnectedColorVariableLight &light) : KeypadMenu(friendlyName, keypad), m_light(light) {}

ConnectedColorVariableLight &KeypadMenuConnectedLightColorControl::getLight()
{
    return m_light;
}

void KeypadMenuConnectedLightColorControl::keyReleased(char key, bool longClick)
{
    int precision = 20;

    switch (key)
    {
    case '1':
    {
        unsigned int newValue = m_light.getRLuminosity();
        if (int(m_light.getRLuminosity()) <= (255 - precision))
            newValue += precision;

        else
            newValue = 255;

        m_light.setColor(newValue, m_light.getGLuminosity(), m_light.getBLuminosity(), true);
        break;
    }

    case '2':
    {
        unsigned int newValue = m_light.getGLuminosity();
        if (int(m_light.getGLuminosity()) <= (255 - precision))
            newValue += precision;

        else
            newValue = 255;

        m_light.setColor(m_light.getRLuminosity(), newValue, m_light.getBLuminosity(), true);
        break;
    }

    case '3':
    {
        unsigned int newValue = m_light.getBLuminosity();
        if (int(m_light.getBLuminosity()) <= (255 - precision))
            newValue += precision;

        else
            newValue = 255;

        m_light.setColor(m_light.getRLuminosity(), m_light.getGLuminosity(), newValue, true);
        break;
    }

    case '4':
    {
        unsigned int newValue = m_light.getRLuminosity();
        if (int(m_light.getRLuminosity()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_light.setColor(newValue, m_light.getGLuminosity(), m_light.getBLuminosity(), true);
        break;
    }

    case '5':
    {
        unsigned int newValue = m_light.getGLuminosity();
        if (int(m_light.getGLuminosity()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_light.setColor(m_light.getRLuminosity(), newValue, m_light.getBLuminosity(), true);
        break;
    }

    case '6':
    {
        unsigned int newValue = m_light.getBLuminosity();
        if (int(m_light.getBLuminosity()) >= precision)
            newValue -= precision;

        else
            newValue = 0;

        m_light.setColor(m_light.getRLuminosity(), m_light.getGLuminosity(), newValue, true);
        break;
    }
    }
}

void KeypadMenuConnectedLightColorControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("+ rouge");
    help[1] = F("+ vert");
    help[2] = F("+ bleu");
    help[3] = F("- rouge");
    help[4] = F("- vert");
    help[4] = F("- bleu");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedLightColorControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuTelevision::KeypadMenuTelevision(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_television(nullptr), m_musicSelectionMenu(nullptr) {}

void KeypadMenuTelevision::setTelevision(Television *television)
{
    m_television = television;
}

void KeypadMenuTelevision::setMusicSelectionMenu(KeypadMenuTelevisionMusicSelector *menu)
{
    m_musicSelectionMenu = menu;
}

void KeypadMenuTelevision::keyReleased(char key, bool longClick)
{
    if (m_television == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_television->toggle(true);
        break;

    case '2':
        m_television->increaseVolume(true);
        break;

    case '3':
        m_television->decreaseVolume(true);
        break;

    case '4':
        m_television->toggleMute(true);
        break;

    case '5':
        m_keypad.setMenu(m_musicSelectionMenu, true);
        break;

    case '6':
        m_television->syncVolume(true);
        break;
    }
}

void KeypadMenuTelevision::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Basculer");
    help[1] = F("+ volume");
    help[2] = F("- volume");
    help[3] = F("Basculer sourdinne");
    help[4] = F("Musiques");
    help[5] = F("Synchroniser");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuTelevision::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(TELEVISIONS, m_friendlyName);
}

KeypadMenuTelevisionMusicSelector::KeypadMenuTelevisionMusicSelector(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_television(nullptr), m_index(0) {}

void KeypadMenuTelevisionMusicSelector::setTelevision(Television *television)
{
    m_television = television;
}

void KeypadMenuTelevisionMusicSelector::keyReleased(char key, bool longClick)
{
    if (m_television == nullptr)
        return;

    if (m_index >= m_television->getMusicNumber())
        m_index = 0;

    switch (key)
    {
    case '1':
        m_television->stopMusic();
        m_keypad.getDisplay().displayMessage("Musique arrêtée");
        break;

    case '2':
        if (m_index == 0)
            break;

        m_index--;
        m_keypad.getDisplay().displaySelectedMusic(*m_television, m_index);
        break;

    case '5':
        m_television->playMusic(m_index);
        break;

    case '8':
        if (m_index >= (m_television->getMusicNumber() - 1))
            break;

        m_index++;
        m_keypad.getDisplay().displaySelectedMusic(*m_television, m_index);
        break;
    }
}

void KeypadMenuTelevisionMusicSelector::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Arrêt de la lecture");
    help[1] = F("Précédent");
    help[4] = F("Sélectionner");
    help[7] = F("Suivant");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuTelevisionMusicSelector::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(TELEVISIONS, m_friendlyName);
}

KeypadMenuAlarm::KeypadMenuAlarm(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_alarm(nullptr), m_missileLauncherControlMenu(nullptr) {}

void KeypadMenuAlarm::setAlarm(Alarm *alarm)
{
    m_alarm = alarm;
}

void KeypadMenuAlarm::setMissileLauncherControlMenu(KeypadMenuAlarmMissileLauncherControl *menu)
{
    m_missileLauncherControlMenu = menu;
}

void KeypadMenuAlarm::keyReleased(char key, bool longClick)
{
    if (m_alarm == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_alarm->toggle(true);
        break;

    case '2':
        m_alarm->toggleBuzzer();
        m_keypad.getDisplay().displayDeviceState(m_alarm->getBuzzerState());
        break;

    case '3':
        m_alarm->storeCard();
        break;

    case '4':
        m_alarm->removeCards();
        break;

    case '5':
        m_alarm->stopRinging();
        break;

    case '6':
        m_alarm->trigger();
        break;

    case '7':
        if (m_missileLauncherControlMenu == nullptr)
            break;

        m_keypad.setMenu(m_missileLauncherControlMenu, true);
        break;
    }
}

void KeypadMenuAlarm::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Basculer");
    help[1] = F("Basculer son");
    help[2] = F("Ajouter carte");
    help[3] = F("Retirer cartes");
    help[4] = F("Arrêter la sonnerie");
    help[5] = F("Déclencher");
    help[6] = F("Lance-missile");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuAlarm::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(ALARMS, m_friendlyName);
}

KeypadMenuAlarmMissileLauncherControl::KeypadMenuAlarmMissileLauncherControl(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_alarm(nullptr), m_missileLauncher(nullptr) {}

void KeypadMenuAlarmMissileLauncherControl::setAlarm(Alarm *alarm)
{
    m_alarm = alarm;
    m_missileLauncher = &m_alarm->getMissileLauncher();
}

void KeypadMenuAlarmMissileLauncherControl::keyReleased(char key, bool longClick)
{
    if (m_missileLauncher == nullptr)
        return;

    if (!m_missileLauncher->isReady())
    {
        m_keypad.getDisplay().displayMessage("Le lance-missile n'est pas opérationnel.", "Erreur");
        return;
    }

    switch (key)
    {
    case '2':
        m_missileLauncher->beginMove(UP);
        break;

    case '4':
        m_missileLauncher->beginMove(LEFT);
        break;

    case '5':
        m_missileLauncher->launchMissile(1);
        break;

    case '6':
        m_missileLauncher->beginMove(RIGHT);
        break;

    case '8':
        m_missileLauncher->beginMove(DOWN);
        break;

    case '9':
        m_missileLauncher->calibrate();
        break;
    }
}

void KeypadMenuAlarmMissileLauncherControl::keyPressed(char key)
{
    if (m_missileLauncher == nullptr)
        return;

    if (!m_missileLauncher->isReady())
    {
        m_keypad.getDisplay().displayMessage("Le lance-missile n'est pas opérationnel.", "Erreur");
        return;
    }

    switch (key)
    {
    case '2':
        m_missileLauncher->stopMove(ANGLE);
        break;

    case '4':
        m_missileLauncher->stopMove(BASE);
        break;

    case '6':
        m_missileLauncher->stopMove(BASE);
        break;

    case '8':
        m_missileLauncher->stopMove(ANGLE);
        break;
    }
}

void KeypadMenuAlarmMissileLauncherControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[1] = F("Haut");
    help[3] = F("Gauche");
    help[4] = F("Tirer un missile");
    help[5] = F("Droite");
    help[7] = F("Bas");
    help[8] = F("Calibrer");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuAlarmMissileLauncherControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

bool KeypadMenuAlarmMissileLauncherControl::advancedClickControl()
{
    return true;
}

KeypadMenuInputList::KeypadMenuInputList(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_inputList(nullptr), m_menuList(nullptr), m_sensorTypeList(nullptr), m_sensorsNumber(0) {}

void KeypadMenuInputList::setInputs(Input **inputList, KeypadMenu **menuList, KeypadMenuSensorType *sensorTypeList, int sensorsNumber)
{
    m_inputList = inputList;
    m_menuList = menuList;
    m_sensorTypeList = sensorTypeList;
    m_sensorsNumber = sensorsNumber;
}

void KeypadMenuInputList::keyReleased(char key, bool longClick)
{
    int index = key - '0' - 1;
    if (index < m_sensorsNumber)
    {
        if (!longClick)
        {
            switch (m_sensorTypeList[index])
            {
            case ANALOG:
            {
                AnalogInput *sensor = static_cast<AnalogInput *>(m_inputList[index]);
                m_keypad.getDisplay().displayAnalogSensorValue(sensor->getValue());
                break;
            }

            case BINARY:
            case WARDROBE_DOOR_SENSOR:
            {
                if (longClick && m_sensorTypeList[index] == WARDROBE_DOOR_SENSOR)
                {
                    m_keypad.setMenu(m_menuList[index], true);
                    break;
                }

                BinaryInput *sensor = static_cast<BinaryInput *>(m_inputList[index]);
                m_keypad.getDisplay().displayBinarySensorValue(sensor->getState());
                break;
            }

            case AIR:
            {
                AirSensor *sensor = static_cast<AirSensor *>(m_inputList[index]);
                m_keypad.getDisplay().displayAirValues(sensor->getTemperature(), sensor->getHumidity());
                break;
            }
            }
        }

        else if (!(m_menuList[index] == nullptr))
            m_keypad.setMenu(m_menuList[index], true);
    }
}

void KeypadMenuInputList::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    for (int i = 0; i < m_sensorsNumber; i++)
        help[i] = m_inputList[i]->getFriendlyName();

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuInputList::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(INPUTS, m_friendlyName);
}

KeypadMenuWardrobeControl::KeypadMenuWardrobeControl(const __FlashStringHelper *friendlyName, Keypad &keypad, WardrobeDoorSensor &sensor) : KeypadMenu(friendlyName, keypad), m_sensor(sensor) {}

void KeypadMenuWardrobeControl::keyReleased(char key, bool longClick)
{
    switch (key)
    {
    case '1':
        m_sensor.toggleActivation();
        m_keypad.getDisplay().displayDeviceState(m_sensor.getActivation());
        break;
    }
}

void KeypadMenuWardrobeControl::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Automatisation");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuWardrobeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuSettings::KeypadMenuSettings(const __FlashStringHelper *friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad) {}

void KeypadMenuSettings::keyReleased(char key, bool longClick)
{
    switch (key)
    {
    case '1':
        systemToShutdown = true;
        systemToRestart = false;
        powerSupplyToShutdown = false;
        break;

    case '2':
        systemToShutdown = true;
        systemToRestart = false;
        powerSupplyToShutdown = true;
        break;

    case '3':
        systemToShutdown = true;
        systemToRestart = true;
        powerSupplyToShutdown = true;
        break;

    case '4':
    {
        extern int __heap_start, *__brkval;
        int v;
        int freeSpace = (int)&v - (__brkval == 0 ? (int)&__heap_start : (int)__brkval);
        m_keypad.getDisplay().displayMessage("Disponible : " + String(freeSpace), "RAM");
        break;
    }

    case '5':
        m_keypad.getDisplay().displayMessage("Actuel : " + String(TPS), "TPS");
        break;
    }
}

void KeypadMenuSettings::displayHelp()
{
    const __FlashStringHelper *help[10] = {nullptr};

    help[0] = F("Arrêter le système");
    help[1] = F("Eteindre le système");
    help[2] = F("Redémarrer le système");
    help[3] = F("RAM disponible");
    help[4] = F("TPS actuel");

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuSettings::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}
