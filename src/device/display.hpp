#ifndef DISPLAY_DEFINITIONS
#define DISPLAY_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <Adafruit_SSD1306.h>

// Autres fichiers du programme.
#include "device.hpp"

enum volumeType
{
    INCREASE,
    DECREASE,
    MUTE,
    UNMUTE
};

class Display : public Device
{
public:
    Display(String friendlyName);
    virtual void setup() override;
    virtual void displayUnavailableDevices();
    virtual void displayBell();
    virtual void displayMessage(String message, String title = "Info");
    virtual void displayVolume(volumeType action = UNMUTE, int volume = 0);
    virtual void displayAlarmTriggered(bool colorsInverted = false);
    virtual void displayAirValues(float temperature, float humidity);
    virtual void displayLuminosityMotionSensorValues(int luminosity, bool motionDetected);
    virtual void displayLEDState(int r, int g, int b);
    virtual void displayDeviceState(bool on);
    virtual void displayKeypadMenu();
    virtual void displayTray(bool shareInformation, bool on);
    virtual void loop();

protected:
    Adafruit_SSD1306 m_display;
    unsigned long m_displayCounter;
};

#endif