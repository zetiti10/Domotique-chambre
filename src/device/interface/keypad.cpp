/**
 * @file device/interface/keypad.cpp
 * @author Louis L
 * @brief Classe gérant un petit clavier avec un système de sous-menus.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibliothèques au programme.
#include <Arduino.h>
#include <Adafruit_Keypad.h>

// Autres fichiers du programme.
#include "device/device.hpp"
#include "device/interface/display.hpp"
#include "device/output/tray.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/alarm.hpp"
#include "device/output/television.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/analogInput.hpp"
#include "keypad.hpp"

Keypad::Keypad(const __FlashStringHelper *friendlyName, int ID, Display &display, byte *userKeymap, byte *row, byte *col, int numRows, int numCols) : Device(friendlyName, ID), m_keypad(userKeymap, row, col, numRows, numCols), m_keyPressTime(0), m_lastInteraction(0), m_display(display), m_mainMenu(nullptr), m_currentMenu(nullptr), m_devicesDefined(false) {}

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

        KeypadMenuOutputList *devicesMenu = new KeypadMenuOutputList("Périphériques " + String(i + 1), *this);

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

        KeypadMenuLightList *lightsMenu = new KeypadMenuLightList("Lumières " + String(i + 1), *this);

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

        KeypadMenuAlarmMissileLauncherControl *missileLanucherControl = new KeypadMenuAlarmMissileLauncherControl("Lance-missile", *this);
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

        KeypadMenuInputList *sensorsMenu = new KeypadMenuInputList("Capteurs " + String(i + 1), *this);

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

    m_devicesDefined = true;
}

void Keypad::setup()
{
    if (m_operational || !m_devicesDefined || m_currentMenu == nullptr)
        return;

    m_keypad.begin();
    m_display.setup();

    m_operational = true;
}

void Keypad::loop()
{
    if (!m_operational)
        return;

    m_keypad.tick();

    if (!(m_currentMenu == m_mainMenu) && ((m_lastInteraction + 600000) <= millis()))
        setMenu(m_mainMenu);

    while (m_keypad.available())
    {
        keypadEvent event = m_keypad.read();

        if (event.bit.EVENT == KEY_JUST_PRESSED)
        {
            if (m_currentMenu->advancedClickControl())
            {
                if (event.bit.KEY >= '1' && event.bit.KEY <= '9')
                    m_currentMenu->keyPressed(event.bit.KEY, false);
            }
            
            if (m_keyPressTime != 0)
                continue;

            m_keyPressTime = millis();
        }

        else
        {
            if (m_keyPressTime == 0)
                continue;

            bool longClick = ((millis() - m_keyPressTime) >= 300);

            if (event.bit.KEY >= '1' && event.bit.KEY <= '9')
            {
                if (m_currentMenu->advancedClickControl())
                    m_currentMenu->keyReleased(event.bit.KEY);

                else
                    m_currentMenu->keyPressed(event.bit.KEY, longClick);
            }

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
                    setMenu(m_mainMenu);
                    break;

                case 'B':
                    if (m_currentMenu->getParentMenu() != nullptr)
                        setMenu(m_currentMenu->getParentMenu());

                    break;

                case 'C':
                    if (m_currentMenu->getPreviousMenu() != nullptr)
                        setMenu(m_currentMenu->getPreviousMenu());

                    break;

                case 'D':
                    if (m_currentMenu->getNextMenu() != nullptr)
                        setMenu(m_currentMenu->getNextMenu());

                    break;
                }
            }

            m_keyPressTime = 0;
            m_lastInteraction = millis();
        }
    }
}

Display &Keypad::getDisplay()
{
    return m_display;
}

void Keypad::setMenu(KeypadMenu *menu)
{
    m_currentMenu = menu;
    m_currentMenu->displayMenu();
}

KeypadMenu::KeypadMenu(String friendlyName, Keypad &keypad) : m_friendlyName(friendlyName), m_keypad(keypad), m_parentMenu(nullptr), m_previousMenu(nullptr), m_nextMenu(nullptr) {}

String KeypadMenu::getFriendlyName() const
{
    return m_friendlyName;
}

void KeypadMenu::setParentMenu(KeypadMenu *menu)
{
    m_parentMenu = menu;
}

KeypadMenu *KeypadMenu::getParentMenu() const
{
    return m_parentMenu;
}

void KeypadMenu::setPreviousMenu(KeypadMenu *menu)
{
    m_previousMenu = menu;
}

KeypadMenu *KeypadMenu::getPreviousMenu() const
{
    return m_previousMenu;
}

void KeypadMenu::setNextMenu(KeypadMenu *menu)
{
    m_nextMenu = menu;
}

KeypadMenu *KeypadMenu::getNextMenu() const
{
    return m_nextMenu;
}

void KeypadMenu::keyReleased(char key)
{
}

bool KeypadMenu::advancedClickControl()
{
    return false;
}

KeypadMenuOutputList::KeypadMenuOutputList(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_outputList(nullptr), m_outputsNumber(0) {}

void KeypadMenuOutputList::setDevices(Output *outputList[], int &outputsNumber)
{
    m_outputList = outputList;
    m_outputsNumber = outputsNumber;
}

void KeypadMenuOutputList::keyPressed(char key, bool longClick)
{
    if ((key - '0' - 1) < m_outputsNumber)
        m_outputList[key - '0' - 1]->toggle(true);
}

void KeypadMenuOutputList::displayHelp()
{
    String help[10];

    for (int i = 0; i < m_outputsNumber; i++)
        help[i] = m_outputList[i]->getFriendlyName();

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuOutputList::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(OUTPUTS, m_friendlyName);
}

KeypadMenuLightList::KeypadMenuLightList(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_lightList(nullptr), m_lightMenuList(nullptr), m_lightsNumber(0) {}

void KeypadMenuLightList::setLights(Output *lightList[], KeypadMenu *lightMenuList[], int &lightsNumber)
{
    m_lightList = lightList;
    m_lightMenuList = lightMenuList;
    m_lightsNumber = lightsNumber;
}

void KeypadMenuLightList::keyPressed(char key, bool longClick)
{
    int index = key - '0' - 1;
    if (index < m_lightsNumber)
    {
        if (!longClick)
            m_lightList[index]->toggle(true);

        else if (!(m_lightMenuList[index] == nullptr))
            m_keypad.setMenu(m_lightMenuList[index]);
    }
}

void KeypadMenuLightList::displayHelp()
{
    String help[10];

    for (int i = 0; i < m_lightsNumber; i++)
        help[i] = m_lightList[i]->getFriendlyName();

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuLightList::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(LIGHTS, m_friendlyName);
}

KeypadMenuRGBLEDStripControl::KeypadMenuRGBLEDStripControl(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_strip(nullptr), m_colorMode(nullptr), m_rainbowMode(nullptr), m_soundreactMode(nullptr), m_alarmMode(nullptr) {}

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

void KeypadMenuRGBLEDStripControl::keyPressed(char key, bool longClick)
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
            m_keypad.setMenu(m_colorMode);

        break;

    case '3':
        if (!longClick)
            m_strip->setMode(&m_rainbowMode->getRainbowMode(), true);

        else
            m_keypad.setMenu(m_rainbowMode);

        break;

    case '4':
        if (!longClick)
            m_strip->setMode(&m_soundreactMode->getSoundreactMode(), true);

        else
            m_keypad.setMenu(m_soundreactMode);

        break;

    case '5':
        if (!longClick)
            m_strip->setMode(&m_alarmMode->getAlarmMode(), true);

        else
            m_keypad.setMenu(m_alarmMode);

        break;
    }
}

void KeypadMenuRGBLEDStripControl::displayHelp()
{
    String help[10];

    help[0] = "Basculer";
    help[1] = "Couleur unique";
    help[2] = "Arc-en-ciel";
    help[3] = "Son-réaction";
    help[4] = "Alarme";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripColorModeControl::KeypadMenuRGBLEDStripColorModeControl(String friendlyName, Keypad &keypad, ColorMode &colorMode) : KeypadMenu(friendlyName, keypad), m_colorMode(colorMode) {}

ColorMode &KeypadMenuRGBLEDStripColorModeControl::getColorMode()
{
    return m_colorMode;
}

void KeypadMenuRGBLEDStripColorModeControl::keyPressed(char key, bool longClick)
{
    int precision = 20;

    switch (key)
    {
    case '1':
        m_colorMode.setColor(m_colorMode.getR() + precision, m_colorMode.getG(), m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;

    case '2':
        m_colorMode.setColor(m_colorMode.getR(), m_colorMode.getG() + precision, m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;

    case '3':
        m_colorMode.setColor(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB() + precision);
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;

    case '4':
        m_colorMode.setColor(m_colorMode.getR() - precision, m_colorMode.getG(), m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;

    case '5':
        m_colorMode.setColor(m_colorMode.getR(), m_colorMode.getG() - precision, m_colorMode.getB());
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;

    case '6':
        m_colorMode.setColor(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB() - precision);
        m_keypad.getDisplay().displayLEDState(m_colorMode.getR(), m_colorMode.getG(), m_colorMode.getB());
        break;
    }
}

void KeypadMenuRGBLEDStripColorModeControl::displayHelp()
{
    String help[10];

    help[0] = "+ rouge";
    help[1] = "+ vert";
    help[2] = "+ bleu";
    help[3] = "- rouge";
    help[4] = "- vert";
    help[4] = "- bleu";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripColorModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripRainbowModeControl::KeypadMenuRGBLEDStripRainbowModeControl(String friendlyName, Keypad &keypad, RainbowMode &rainbowMode) : KeypadMenu(friendlyName, keypad), m_rainbowMode(rainbowMode) {}

RainbowMode &KeypadMenuRGBLEDStripRainbowModeControl::getRainbowMode()
{
    return m_rainbowMode;
}

void KeypadMenuRGBLEDStripRainbowModeControl::keyPressed(char key, bool longClick)
{
    int precision = 10;

    switch (key)
    {
    case '1':
        m_rainbowMode.setAnimationSpeed(m_rainbowMode.getAnimationSpeed() + precision);
        m_keypad.getDisplay().displayPercentage("Vitesse", m_rainbowMode.getAnimationSpeed());
        break;

    case '2':
        m_rainbowMode.setAnimationSpeed(m_rainbowMode.getAnimationSpeed() - precision);
        m_keypad.getDisplay().displayPercentage("Vitesse", m_rainbowMode.getAnimationSpeed());
        break;
    }
}

void KeypadMenuRGBLEDStripRainbowModeControl::displayHelp()
{
    String help[10];

    help[0] = "+ vite";
    help[1] = "- vite";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripRainbowModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripSoundreactModeControl::KeypadMenuRGBLEDStripSoundreactModeControl(String friendlyName, Keypad &keypad, SoundreactMode &soundreactMode) : KeypadMenu(friendlyName, keypad), m_soundreactMode(soundreactMode) {}

SoundreactMode &KeypadMenuRGBLEDStripSoundreactModeControl::getSoundreactMode()
{
    return m_soundreactMode;
}

void KeypadMenuRGBLEDStripSoundreactModeControl::keyPressed(char key, bool longClick)
{
    int precision = 5;

    switch (key)
    {
    case '1':
        m_soundreactMode.setSensitivity(m_soundreactMode.getSensitivity() + precision);
        m_keypad.getDisplay().displayPercentage("Sensibilité", m_soundreactMode.getSensitivity());
        break;

    case '2':
        m_soundreactMode.setSensitivity(m_soundreactMode.getSensitivity() - precision);
        m_keypad.getDisplay().displayPercentage("Sensibilité", m_soundreactMode.getSensitivity());
        break;
    }
}

void KeypadMenuRGBLEDStripSoundreactModeControl::displayHelp()
{
    String help[10];

    help[0] = "+ sensible";
    help[1] = "- sensible";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripSoundreactModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuRGBLEDStripAlarmModeControl::KeypadMenuRGBLEDStripAlarmModeControl(String friendlyName, Keypad &keypad, AlarmMode &alarmMode) : KeypadMenu(friendlyName, keypad), m_alarmMode(alarmMode) {}

AlarmMode &KeypadMenuRGBLEDStripAlarmModeControl::getAlarmMode()
{
    return m_alarmMode;
}

void KeypadMenuRGBLEDStripAlarmModeControl::keyPressed(char key, bool longClick)
{
    // Rien...
}

void KeypadMenuRGBLEDStripAlarmModeControl::displayHelp()
{
    String help[10];
    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuRGBLEDStripAlarmModeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedTemperatureVariableLightControl::KeypadMenuConnectedTemperatureVariableLightControl(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_temperatureMenu(nullptr), m_luminosityMenu(nullptr) {}

void KeypadMenuConnectedTemperatureVariableLightControl::setLight(ConnectedTemperatureVariableLight &light)
{
    m_temperatureMenu = new KeypadMenuConnectedLightTemperatureControl("Température " + light.getFriendlyName(), m_keypad, light);
    m_temperatureMenu->setParentMenu(this);
    m_luminosityMenu = new KeypadMenuConnectedLightLuminosityControl("Luminosité " + light.getFriendlyName(), m_keypad, light);
    m_luminosityMenu->setParentMenu(this);

    m_temperatureMenu->setNextMenu(m_luminosityMenu);
    m_temperatureMenu->setPreviousMenu(m_luminosityMenu);
    m_luminosityMenu->setNextMenu(m_temperatureMenu);
    m_luminosityMenu->setPreviousMenu(m_temperatureMenu);
}

void KeypadMenuConnectedTemperatureVariableLightControl::keyPressed(char key, bool longClick)
{
    if (m_temperatureMenu == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_temperatureMenu->getLight().toggle(true);
        break;

    case '2':
        m_keypad.setMenu(m_temperatureMenu);
        break;

    case '3':
        m_keypad.setMenu(m_luminosityMenu);
        break;
    }
}

void KeypadMenuConnectedTemperatureVariableLightControl::displayHelp()
{
    String help[10];

    help[0] = "Basculer";
    help[1] = "Température";
    help[2] = "Luminosité";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedTemperatureVariableLightControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedColorVariableLightControl::KeypadMenuConnectedColorVariableLightControl(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_temperatureMenu(nullptr), m_luminosityMenu(nullptr), m_colorMenu(nullptr) {}

void KeypadMenuConnectedColorVariableLightControl::setLight(ConnectedColorVariableLight &light)
{
    m_temperatureMenu = new KeypadMenuConnectedLightTemperatureControl("Température " + light.getFriendlyName(), m_keypad, light);
    m_temperatureMenu->setParentMenu(this);
    m_luminosityMenu = new KeypadMenuConnectedLightLuminosityControl("Luminosité " + light.getFriendlyName(), m_keypad, light);
    m_luminosityMenu->setParentMenu(this);
    m_colorMenu = new KeypadMenuConnectedLightColorControl("Couleur " + light.getFriendlyName(), m_keypad, light);
    m_colorMenu->setParentMenu(this);

    m_colorMenu->setNextMenu(m_temperatureMenu);
    m_colorMenu->setPreviousMenu(m_luminosityMenu);
    m_temperatureMenu->setNextMenu(m_luminosityMenu);
    m_temperatureMenu->setPreviousMenu(m_colorMenu);
    m_luminosityMenu->setNextMenu(m_colorMenu);
    m_luminosityMenu->setPreviousMenu(m_temperatureMenu);
}

void KeypadMenuConnectedColorVariableLightControl::keyPressed(char key, bool longClick)
{
    if (m_temperatureMenu == nullptr)
        return;

    switch (key)
    {
    case '1':
        m_temperatureMenu->getLight().toggle(true);
        break;

    case '2':
        m_keypad.setMenu(m_temperatureMenu);
        break;

    case '3':
        m_keypad.setMenu(m_luminosityMenu);
        break;

    case '4':
        m_keypad.setMenu(m_colorMenu);
        break;
    }
}

void KeypadMenuConnectedColorVariableLightControl::displayHelp()
{
    String help[10];

    help[0] = "Basculer";
    help[1] = "Température";
    help[2] = "Luminosité";
    help[3] = "Couleur";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedColorVariableLightControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedLightLuminosityControl::KeypadMenuConnectedLightLuminosityControl(String friendlyName, Keypad &keypad, ConnectedTemperatureVariableLight &light) : KeypadMenu(friendlyName, keypad), m_light(light) {}

ConnectedTemperatureVariableLight &KeypadMenuConnectedLightLuminosityControl::getLight()
{
    return m_light;
}

void KeypadMenuConnectedLightLuminosityControl::keyPressed(char key, bool longClick)
{
    int precision = 20;

    switch (key)
    {
    case '1':
        m_light.setLuminosity(m_light.getLuminosity() + precision, true);
        break;

    case '2':
        m_light.setLuminosity(m_light.getLuminosity() - precision, true);
        break;
    }
}

void KeypadMenuConnectedLightLuminosityControl::displayHelp()
{
    String help[10];

    help[0] = "+ luminosité";
    help[1] = "- luminosité";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedLightLuminosityControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedLightTemperatureControl::KeypadMenuConnectedLightTemperatureControl(String friendlyName, Keypad &keypad, ConnectedTemperatureVariableLight &light) : KeypadMenu(friendlyName, keypad), m_light(light) {}

ConnectedTemperatureVariableLight &KeypadMenuConnectedLightTemperatureControl::getLight()
{
    return m_light;
}

void KeypadMenuConnectedLightTemperatureControl::keyPressed(char key, bool longClick)
{
    int precision = 200;

    switch (key)
    {
    case '1':
        m_light.setColorTemperature(m_light.getColorTemperature() + precision, true);
        break;

    case '2':
        m_light.setColorTemperature(m_light.getColorTemperature() - precision, true);
        break;
    }
}

void KeypadMenuConnectedLightTemperatureControl::displayHelp()
{
    String help[10];

    help[0] = "+ température";
    help[1] = "- température";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedLightTemperatureControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuConnectedLightColorControl::KeypadMenuConnectedLightColorControl(String friendlyName, Keypad &keypad, ConnectedColorVariableLight &light) : KeypadMenu(friendlyName, keypad), m_light(light) {}

ConnectedColorVariableLight &KeypadMenuConnectedLightColorControl::getLight()
{
    return m_light;
}

void KeypadMenuConnectedLightColorControl::keyPressed(char key, bool longClick)
{
    int precision = 20;

    switch (key)
    {
    case '1':
        m_light.setColor(m_light.getRLuminosity() + precision, m_light.getGLuminosity(), m_light.getBLuminosity(), true);
        break;

    case '2':
        m_light.setColor(m_light.getRLuminosity(), m_light.getGLuminosity() + precision, m_light.getBLuminosity(), true);
        break;

    case '3':
        m_light.setColor(m_light.getRLuminosity(), m_light.getGLuminosity(), m_light.getBLuminosity() + precision, true);
        break;

    case '4':
        m_light.setColor(m_light.getRLuminosity() - precision, m_light.getGLuminosity(), m_light.getBLuminosity(), true);
        break;

    case '5':
        m_light.setColor(m_light.getRLuminosity(), m_light.getGLuminosity() - precision, m_light.getBLuminosity(), true);
        break;

    case '6':
        m_light.setColor(m_light.getRLuminosity(), m_light.getGLuminosity(), m_light.getBLuminosity() - precision, true);
        break;
    }
}

void KeypadMenuConnectedLightColorControl::displayHelp()
{
    String help[10];

    help[0] = "+ rouge";
    help[1] = "+ vert";
    help[2] = "+ bleu";
    help[3] = "- rouge";
    help[4] = "- vert";
    help[4] = "- bleu";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuConnectedLightColorControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}

KeypadMenuTelevision::KeypadMenuTelevision(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_television(nullptr) {}

void KeypadMenuTelevision::setTelevision(Television *television)
{
    m_television = television;
}

void KeypadMenuTelevision::keyPressed(char key, bool longClick)
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
    }
}

void KeypadMenuTelevision::displayHelp()
{
    String help[10];

    help[0] = "Basculer";
    help[1] = "+ volume";
    help[2] = "- volume";
    help[3] = "Basculer sourdinne";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuTelevision::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(TELEVISIONS, m_friendlyName);
}

KeypadMenuAlarm::KeypadMenuAlarm(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_alarm(nullptr) {}

void KeypadMenuAlarm::setAlarm(Alarm *alarm)
{
    m_alarm = alarm;
}

void KeypadMenuAlarm::setMissileLauncherControlMenu(KeypadMenuAlarmMissileLauncherControl *menu)
{
    m_missileLauncherControlMenu = menu;
}

void KeypadMenuAlarm::keyPressed(char key, bool longClick)
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
        
        m_keypad.setMenu(m_missileLauncherControlMenu);
        break;
    }
}

void KeypadMenuAlarm::displayHelp()
{
    String help[10];

    help[0] = "Basculer";
    help[1] = "Basculer son";
    help[2] = "Ajouter carte";
    help[3] = "Retirer cartes";
    help[4] = "Arrêter la sonnerie";
    help[5] = "Déclencher";
    help[6] = "Lance-missile";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuAlarm::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(ALARMS, m_friendlyName);
}

KeypadMenuAlarmMissileLauncherControl::KeypadMenuAlarmMissileLauncherControl(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_alarm(nullptr), m_missileLauncher(nullptr) {}

void KeypadMenuAlarmMissileLauncherControl::setAlarm(Alarm *alarm)
{
    m_alarm = alarm;
    m_missileLauncher = &m_alarm->getMissileLauncher();
}

void KeypadMenuAlarmMissileLauncherControl::keyPressed(char key, bool longClick)
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

void KeypadMenuAlarmMissileLauncherControl::keyReleased(char key)
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
    String help[10];

    help[2] = "Haut";
    help[4] = "Gauche";
    help[5] = "Tirer un missile";
    help[6] = "Droite";
    help[8] = "Bas";
    help[9] = "Calibrer";

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

KeypadMenuInputList::KeypadMenuInputList(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_inputList(nullptr), m_menuList(nullptr), m_sensorTypeList(nullptr), m_sensorsNumber(0) {}

void KeypadMenuInputList::setInputs(Input **inputList, KeypadMenu **menuList, KeypadMenuSensorType *sensorTypeList, int sensorsNumber)
{
    m_inputList = inputList;
    m_menuList = menuList;
    m_sensorTypeList = sensorTypeList;
    m_sensorsNumber = sensorsNumber;
}

void KeypadMenuInputList::keyPressed(char key, bool longClick)
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
                    m_keypad.setMenu(m_menuList[index]);
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
            m_keypad.setMenu(m_menuList[index]);
    }
}

void KeypadMenuInputList::displayHelp()
{
    String help[10];

    for (int i = 0; i < m_sensorsNumber; i++)
        help[i] = m_inputList[i]->getFriendlyName();

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuInputList::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(INPUTS, m_friendlyName);
}

KeypadMenuWardrobeControl::KeypadMenuWardrobeControl(String friendlyName, Keypad &keypad, WardrobeDoorSensor &sensor) : KeypadMenu(friendlyName, keypad), m_sensor(sensor) {}

void KeypadMenuWardrobeControl::keyPressed(char key, bool longClick)
{
    if (key == '1')
    {
        m_sensor.toggleActivation();
        m_keypad.getDisplay().displayDeviceState(m_sensor.getActivation());
    }
}

void KeypadMenuWardrobeControl::displayHelp()
{
    String help[10];

    help[0] = "Automatisation";

    m_keypad.getDisplay().displayKeypadMenuHelp(help, m_friendlyName);
}

void KeypadMenuWardrobeControl::displayMenu()
{
    m_keypad.getDisplay().displayKeypadMenu(CONTROLS, m_friendlyName);
}