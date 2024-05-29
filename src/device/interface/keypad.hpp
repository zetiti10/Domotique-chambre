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
#include "device/output/connectedOutput.hpp"
#include "device/input/binaryInput.hpp"
#include "device/input/analogInput.hpp"
#include "device/input/airSensor.hpp"

class KeypadMenu;

// Classe gérant un petit clavier avec un système de sous-menus.
class Keypad : public Device
{
public:
    Keypad(const __FlashStringHelper* friendlyName, int ID, Display &display, byte *userKeymap, byte *row, byte *col, int numRows, int numCols);
    virtual void setDevices(
        Output *deviceList[], int &devicesNumber,
        Output *lightList[], int &lightsNumber,
        RGBLEDStrip *RGBLEDStripList[], ColorMode *colorModeList[], RainbowMode *rainbowModeList[], SoundreactMode *soundreactModeList[], AlarmMode *alarmModeList[], int &RGBLEDStripsNumber,
        ConnectedTemperatureVariableLight *connectedTemperatureVariableLightList[], int &connectedTemperatureVariableLightsNumber,
        ConnectedColorVariableLight *connectedColorVariableLightList[], int &connectedColorVariableLightsNumber,
        Television *televisionList[], int &televisionsNumber,
        Alarm *alarmList[], int &alarmsNumber,
        BinaryInput *binaryInputList[], int &binaryInputsNumber,
        AnalogInput *analogInputList[], int &analogInputsNumber,
        AirSensor *airSensorList[], int &airSensorsNumber,
        WardrobeDoorSensor *wardrobeDoorSensorList[], int &wardrobeDoorSensorsNumber);
    virtual void setup() override;
    virtual void loop();
    virtual Display &getDisplay();
    virtual void setMenu(KeypadMenu *menu);

protected:
    Adafruit_Keypad m_keypad;
    unsigned long m_keyPressTime;
    unsigned long m_lastInteraction;
    Display &m_display;
    KeypadMenu *m_mainMenu;
    KeypadMenu *m_currentMenu;
    bool m_devicesDefined;
};

class KeypadMenu
{
public:
    KeypadMenu(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual String getFriendlyName() const;
    virtual void setParentMenu(KeypadMenu *menu);
    virtual KeypadMenu *getParentMenu() const;
    virtual void setPreviousMenu(KeypadMenu *menu);
    virtual KeypadMenu *getPreviousMenu() const;
    virtual void setNextMenu(KeypadMenu *menu);
    virtual KeypadMenu *getNextMenu() const;

    virtual void keyPressed(char key, bool longClick) = 0;
    virtual void keyReleased(char key);
    virtual void displayHelp() = 0;
    virtual void displayMenu() = 0;
    virtual bool advancedClickControl();

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
    KeypadMenuOutputList(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setDevices(Output *outputList[], int &outputsNumber);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    Output **m_outputList;
    int m_outputsNumber;
};

class KeypadMenuLightList : public KeypadMenu
{
public:
    KeypadMenuLightList(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setLights(Output *lightList[], KeypadMenu *lightMenuList[], int &lightsNumber);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    Output **m_lightList;
    KeypadMenu **m_lightMenuList;
    int m_lightsNumber;
};

class KeypadMenuRGBLEDStripColorModeControl : public KeypadMenu
{
public:
    KeypadMenuRGBLEDStripColorModeControl(const __FlashStringHelper* friendlyName, Keypad &keypad, ColorMode &colorMode);

    virtual ColorMode &getColorMode();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    ColorMode &m_colorMode;
};

class KeypadMenuRGBLEDStripRainbowModeControl : public KeypadMenu
{
public:
    KeypadMenuRGBLEDStripRainbowModeControl(const __FlashStringHelper* friendlyName, Keypad &keypad, RainbowMode &rainbowMode);

    virtual RainbowMode &getRainbowMode();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    RainbowMode &m_rainbowMode;
};

class KeypadMenuRGBLEDStripSoundreactModeControl : public KeypadMenu
{
public:
    KeypadMenuRGBLEDStripSoundreactModeControl(const __FlashStringHelper* friendlyName, Keypad &keypad, SoundreactMode &soundreactMode);

    virtual SoundreactMode &getSoundreactMode();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    SoundreactMode &m_soundreactMode;
};

class KeypadMenuRGBLEDStripAlarmModeControl : public KeypadMenu
{
public:
    KeypadMenuRGBLEDStripAlarmModeControl(const __FlashStringHelper* friendlyName, Keypad &keypad, AlarmMode &alarmMode);

    virtual AlarmMode &getAlarmMode();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    AlarmMode &m_alarmMode;
};

class KeypadMenuRGBLEDStripControl : public KeypadMenu
{
public:
    KeypadMenuRGBLEDStripControl(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setStrip(RGBLEDStrip *strip, ColorMode *colorMode, RainbowMode *rainbowMode, SoundreactMode *soundreactMode, AlarmMode *alarmMode);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    RGBLEDStrip *m_strip;

    KeypadMenuRGBLEDStripColorModeControl *m_colorMode;
    KeypadMenuRGBLEDStripRainbowModeControl *m_rainbowMode;
    KeypadMenuRGBLEDStripSoundreactModeControl *m_soundreactMode;
    KeypadMenuRGBLEDStripAlarmModeControl *m_alarmMode;
};



class KeypadMenuConnectedLightLuminosityControl : public KeypadMenu
{
public:
    KeypadMenuConnectedLightLuminosityControl(const __FlashStringHelper* friendlyName, Keypad &keypad, ConnectedTemperatureVariableLight &light);

    virtual ConnectedTemperatureVariableLight &getLight();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    ConnectedTemperatureVariableLight &m_light;
};

class KeypadMenuConnectedLightTemperatureControl : public KeypadMenu
{
public:
    KeypadMenuConnectedLightTemperatureControl(const __FlashStringHelper* friendlyName, Keypad &keypad, ConnectedTemperatureVariableLight &light);

    virtual ConnectedTemperatureVariableLight &getLight();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    ConnectedTemperatureVariableLight &m_light;
};

class KeypadMenuConnectedLightColorControl : public KeypadMenu
{
public:
    KeypadMenuConnectedLightColorControl(const __FlashStringHelper* friendlyName, Keypad &keypad, ConnectedColorVariableLight &light);

    virtual ConnectedColorVariableLight &getLight();

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    ConnectedColorVariableLight &m_light;
};

class KeypadMenuConnectedTemperatureVariableLightControl : public KeypadMenu
{
public:
    KeypadMenuConnectedTemperatureVariableLightControl(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setLight(ConnectedTemperatureVariableLight &light);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    KeypadMenuConnectedLightTemperatureControl *m_temperatureMenu;
    KeypadMenuConnectedLightLuminosityControl *m_luminosityMenu;
};

class KeypadMenuConnectedColorVariableLightControl : public KeypadMenu
{
public:
    KeypadMenuConnectedColorVariableLightControl(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setLight(ConnectedColorVariableLight &light);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    KeypadMenuConnectedLightTemperatureControl *m_temperatureMenu;
    KeypadMenuConnectedLightLuminosityControl *m_luminosityMenu;
    KeypadMenuConnectedLightColorControl *m_colorMenu;
};

class KeypadMenuTelevisionMusicSelector;

class KeypadMenuTelevision : public KeypadMenu
{
public:
    KeypadMenuTelevision(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setTelevision(Television *television);
    virtual void setMusicSelectionMenu(KeypadMenuTelevisionMusicSelector *menu);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    Television *m_television;
    KeypadMenuTelevisionMusicSelector *m_musicSelectionMenu;
};

class KeypadMenuTelevisionMusicSelector : public KeypadMenu
{
public:
    KeypadMenuTelevisionMusicSelector(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setTelevision(Television *television);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    Television *m_television;
    int m_index;
};

class KeypadMenuAlarmMissileLauncherControl;

class KeypadMenuAlarm : public KeypadMenu
{
public:
    KeypadMenuAlarm(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setAlarm(Alarm *alarm);
    virtual void setMissileLauncherControlMenu(KeypadMenuAlarmMissileLauncherControl *menu);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    Alarm *m_alarm;
    KeypadMenuAlarmMissileLauncherControl *m_missileLauncherControlMenu;
};

class KeypadMenuAlarmMissileLauncherControl : public KeypadMenu
{
public:
    KeypadMenuAlarmMissileLauncherControl(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setAlarm(Alarm *alarm);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void keyReleased(char key) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;
    virtual bool advancedClickControl() override;

protected:
    Alarm *m_alarm;
    MissileLauncher *m_missileLauncher;
};

enum KeypadMenuSensorType
{
    ANALOG,
    BINARY,
    WARDROBE_DOOR_SENSOR,
    AIR
};

class KeypadMenuInputList : public KeypadMenu
{
public:
    KeypadMenuInputList(const __FlashStringHelper* friendlyName, Keypad &keypad);

    virtual void setInputs(Input **inputList, KeypadMenu **menuList, KeypadMenuSensorType *sensorTypeList, int sensorsNumber);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    Input **m_inputList;
    KeypadMenu **m_menuList;
    KeypadMenuSensorType *m_sensorTypeList;
    int m_sensorsNumber;
};

class KeypadMenuWardrobeControl : public KeypadMenu
{
public:
    KeypadMenuWardrobeControl(const __FlashStringHelper* friendlyName, Keypad &keypad, WardrobeDoorSensor &sensor);

    virtual void keyPressed(char key, bool longClick) override;
    virtual void displayHelp() override;
    virtual void displayMenu() override;

protected:
    WardrobeDoorSensor &m_sensor;
};

#endif