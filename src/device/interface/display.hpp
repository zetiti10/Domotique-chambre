#ifndef DISPLAY_DEFINITIONS
#define DISPLAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Autres fichiers du programme.
#include "../device.hpp"

// Types d'affichages pour le volume de la télévision.
enum VolumeType
{
    INCREASE,
    DECREASE,
    MUTE,
    UNMUTE
};

// Classe regroupant les méthodes de contrôle de l'écran.
class Display : public Device
{
public:
    Display(String friendlyName, int ID);
    virtual void setup() override;
    virtual void displayUnavailableDevices(Device* deviceList[], int &devicesNumber);
    virtual void displayBell();
    virtual void displayMessage(String message, String title = "Info");
    virtual void displayVolume(VolumeType action = UNMUTE, int volume = 0);
    virtual void displayAlarmTriggered(bool colorsInverted = false);
    virtual void displayAirValues(float temperature, float humidity);
    virtual void displayLuminosityMotionSensorValues(int luminosity, bool motionDetected);
    virtual void displayLEDState(int r, int g, int b);
    virtual void displayDeviceState(bool on);
    virtual void displayKeypadMenu();
    virtual void displayTray(bool on, bool shareInformation = false);
    virtual void displayLightColorTemperature(int minimum, int maximum, int temperature);
    virtual void displayLuminosity(int luminosity);
    virtual void loop();

protected:
    Adafruit_SSD1306 m_display;
    unsigned long m_lastTime;
};

#endif