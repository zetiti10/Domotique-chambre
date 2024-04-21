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

Keypad::Keypad(String friendlyName, int ID, Display &display, byte *userKeymap, byte *row, byte *col, int numRows, int numCols) : Device(friendlyName, ID), m_keypad(userKeymap, row, col, numRows, numCols), m_keyPressTime(0), m_display(display), m_mainMenu(nullptr), m_currentMenu(nullptr), m_devicesDefined(false) {}

void Keypad::setDevices(Output *outputList[], int &outputsNumber, Alarm *alarmList[], int &alarmsNumber)
{
    int devicesNumber = outputsNumber + alarmsNumber;
    int menusNumber = ceil(double(devicesNumber) / 9.0);

    KeypadMenu *previousMenu = nullptr;

    for (int i = 0; i < menusNumber; i++)
    {
        int itemsInCurrentMenu;

        if ((i + 1) == menusNumber)
            itemsInCurrentMenu = devicesNumber - (i * 9);

        else
            itemsInCurrentMenu = 9;

        KeypadMenu **subMenusInCurrentMenu = new KeypadMenu *[itemsInCurrentMenu];
        Output **devicesInCurrentMenu = new Output *[itemsInCurrentMenu];

        KeypadMenuOutputList *devicesMenu = new KeypadMenuOutputList("Périphériques " + String(i + 1), *this);

        for (int j = 0; j < itemsInCurrentMenu; j++)
        {
            int deviceIndex = (i * 9) + j;

            if (deviceIndex < outputsNumber)
            {
                subMenusInCurrentMenu[j] = nullptr;

                devicesInCurrentMenu[j] = outputList[deviceIndex];
            }

            else if (deviceIndex < (outputsNumber + alarmsNumber))
            {
                Alarm *alarm = alarmList[deviceIndex - outputsNumber];

                KeypadMenuAlarm *menu = new KeypadMenuAlarm("Contrôle de l'alarme " + alarm->getFriendlyName(), *this);
                menu->setAlarm(alarm);
                menu->setParentMenu(devicesMenu);

                subMenusInCurrentMenu[j] = menu;

                devicesInCurrentMenu[j] = alarm;
            }
        }

        devicesMenu->setDevices(devicesInCurrentMenu, subMenusInCurrentMenu, itemsInCurrentMenu);

        if (i == 0)
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

    m_devicesDefined = true;
}

void Keypad::setup()
{
    if (m_operational || !m_devicesDefined)
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

    while (m_keypad.available())
    {
        keypadEvent event = m_keypad.read();

        if (event.bit.EVENT == KEY_JUST_PRESSED)
        {
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
                m_currentMenu->keyPressed(event.bit.KEY, longClick);

            else
            {
                switch (event.bit.KEY)
                {
                case '*':
                    m_display.displayMessage(m_currentMenu->getFriendlyName(), "Menu");
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

void KeypadMenu::keyPressed(char key, bool longClick) {}

void KeypadMenu::displayHelp() {}

KeypadMenuOutputList::KeypadMenuOutputList(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_outputList(nullptr), m_outputMenuList(nullptr), m_outputsNumber(0) {}

void KeypadMenuOutputList::setDevices(Output *outputList[], KeypadMenu *outputMenuList[], int &outputsNumber)
{
    m_outputList = outputList;
    m_outputMenuList = outputMenuList;
    m_outputsNumber = outputsNumber;

    Serial.println("Les périphériques du menu ont été initialisés.");
}

void KeypadMenuOutputList::keyPressed(char key, bool longClick)
{
    int keyInt = key - '0';

    if (!longClick)
    {
        if (keyInt <= m_outputsNumber)
            m_outputList[keyInt - 1]->toggle(true);
    }

    else
    {
        if (m_outputMenuList[keyInt - 1] != nullptr)
            m_keypad.setMenu(m_outputMenuList[keyInt - 1]);
    }
}

void KeypadMenuOutputList::displayHelp()
{
    String list("Court : basculer ; Long : contrôle ; ");

    for (int i = 0; i < m_outputsNumber; i++)
        list += String(i + 1) + " : " + m_outputList[i]->getFriendlyName() + " ; ";

    m_keypad.getDisplay().displayMessage(list, "Aide");
}

KeypadMenuAlarm::KeypadMenuAlarm(String friendlyName, Keypad &keypad) : KeypadMenu(friendlyName, keypad), m_alarm(nullptr) {}

void KeypadMenuAlarm::setAlarm(Alarm *alarm)
{
    m_alarm = alarm;
}

void KeypadMenuAlarm::keyPressed(char key, bool longClick)
{
    if ((key == '1') && !longClick)
        m_alarm->toggleBuzzer();
}

void KeypadMenuAlarm::displayHelp()
{
    m_keypad.getDisplay().displayMessage("1 : basculer le buzzer", "Aide");
}
