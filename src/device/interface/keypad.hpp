#ifndef KEYPAD_DEFINITIONS
#define KEYPAD_DEFINITIONS

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

class KeypadMenu;

// Classe gérant un petit clavier avec un système de sous-menus.
class Keypad : public Device
{
public:
    Keypad(String friendlyName, int ID, Display &display, byte *userKeymap, byte *row, byte *col, int numRows, int numCols);
    virtual void setDevices(Output *outputList[], int &outputsNumber, Alarm *alarmList[], int &alarmsNumber /*, RGBLEDStrip *RGBLEDStripList[], Television *televisionList[], ConnectedOutput *connectedBinaryOutputList[], ConnectedTemperatureVariableLight *connectedTemperatureVariableLightList[], ConnectedColorVariableLight *connectedColorVariableLightList[], Input *inputList[]*/);
    virtual void setup() override;
    virtual void loop();
    virtual Display &getDisplay();
    virtual void setMenu(KeypadMenu *menu);

protected:
    Adafruit_Keypad m_keypad;
    unsigned long m_keyPressTime;
    Display &m_display;
    KeypadMenu *m_mainMenu;
    KeypadMenu *m_currentMenu;
    bool m_devicesDefined;
};

class KeypadMenu
{
public:
    KeypadMenu(String friendlyName, Keypad &keypad);
    
    virtual String getFriendlyName() const;
    virtual void setParentMenu(KeypadMenu *menu);
    virtual KeypadMenu *getParentMenu() const;
    virtual void setPreviousMenu(KeypadMenu *menu);
    virtual KeypadMenu *getPreviousMenu() const;
    virtual void setNextMenu(KeypadMenu *menu);
    virtual KeypadMenu *getNextMenu() const;
    
    virtual void keyPressed(char key, bool longClick);
    virtual void displayHelp();

protected:
    String m_friendlyName;
    Keypad &m_keypad;

    KeypadMenu *m_parentMenu;
    KeypadMenu *m_previousMenu;
    KeypadMenu *m_nextMenu;
};

class KeypadMenuOutputList : public KeypadMenu
{
public:
    KeypadMenuOutputList(String friendlyName, Keypad &keypad);

    virtual void setDevices(Output *outputList[], KeypadMenu *outputMenuList[], int &outputsNumber);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;

protected:
    Output **m_outputList;
    KeypadMenu **m_outputMenuList;
    int m_outputsNumber;
};

class KeypadMenuAlarm : public KeypadMenu
{
public: 
    KeypadMenuAlarm(String friendlyName, Keypad &keypad);

    virtual void setAlarm(Alarm *alarm);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;

protected:
    Alarm *m_alarm;
};

#endif