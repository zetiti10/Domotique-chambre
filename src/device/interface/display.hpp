#ifndef DISPLAY_DEFINITIONS
#define DISPLAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Autres fichiers du programme.
#include "device/device.hpp"

// Types d'affichages du volume de la télévision.
enum VolumeType
{
    INCREASE,
    DECREASE,
    MUTE,
    UNMUTE
};

// Types de pictogrammes disponibles pour les menus.
enum MenuIcons
{
    OUTPUTS,
    LIGHTS,
    TELEVISIONS,
    ALARMS,
    INPUTS,
    CONTROLS
};

struct Music;
class Television;

// Classe regroupant les méthodes de contrôle de l'écran.
class Display : public Device
{
public:
    Display(const __FlashStringHelper* friendlyName, unsigned int ID);
    virtual void setup() override;
    virtual void displayUnavailableDevices(Device* deviceList[], int &devicesNumber);
    virtual void displayBell();
    virtual void displayMessage(const String &message, const String &title = "Info");
    virtual void displayVolume(VolumeType action = UNMUTE, int volume = 0);
    virtual void displayAlarmTriggered(bool colorsInverted = false);
    virtual void displayAirValues(float temperature, float humidity);
    virtual void displayAnalogSensorValue(int value);
    virtual void displayBinarySensorValue(bool value);
    virtual void displayLEDState(int r, int g, int b);
    virtual void displayDeviceState(bool on);
    virtual void displayKeypadMenu(MenuIcons menuIcon, String &menuName);
    virtual void displayKeypadMenuHelp(String *menuHelpList, String &menuName);
    virtual void displayTray(bool on, bool shareInformation = false);
    virtual void displayLightColorTemperature(int minimum, int maximum, int temperature);
    virtual void displayLuminosity(int luminosity);
    virtual void displayPercentage(String name, int value);
    virtual void displaySelectedMusic(Television &television, unsigned int musicIndex);
    virtual void loop();
    virtual void shutdown() override;

protected:
    virtual void printAccents(const String &string);
    virtual void printCenteredAccents(const String &string, int textSize, int y);
    virtual void resetDisplay(bool resetHelpMenu = true);
    virtual void display();

    Adafruit_SSD1306 m_display;
    unsigned long m_lastTime;
    unsigned long m_lastStateAnimation;
    String *m_menuHelpList;
    int m_menuHelpMenu;
};

#endif