#ifndef HOME_ASSISTANT_DEFINITIONS
#define HOME_ASSISTANT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/device.hpp"
#include "../output/RGBLEDStrip.hpp"
#include "../output/alarm.hpp"
#include "../output/television.hpp"

#define UART_WAITING_TIME 320 / 115200 + 1

// Classe de gestion de la communication avec l'ESP.
class HomeAssistant : public Device
{
public:
    HomeAssistant(String friendlyName, int ID, HardwareSerial &serial);
    virtual void setDevices(Output *deviceList[], int &devicesNumber, Output *remoteDeviceList[], int &remoteDeviceNumber, ColorMode &colorMode, RainbowMode &rainbowMode);
    virtual void setup();
    virtual void loop();
    virtual void setConnectedTemperatureVariableLightTemperature(int ID, int temperature);
    virtual void setConnectedTemperatureVariableLightLuminosity(int ID, int luminosity);
    virtual void setConnectedColorVariableLightColor(int ID, int r, int g, int b);
    virtual void setConnectedColorVariableLightTemperature(int ID, int temperature);
    virtual void setConnectedColorVariableLightLuminosity(int ID, int luminosity);
    virtual void updateDeviceAvailability(int ID, bool availability);
    virtual void updateOutputDeviceState(int ID, bool state);
    virtual void updateRGBLEDStripMode(int ID, int mode, int r = 0, int g = 0, int b = 0);
    virtual void updateAlarmTriggeredState(int ID, bool state);
    virtual void updateTelevisionVolume(int ID, int mode, int volume = 0);
    virtual void updateBinaryInput(int ID, bool state);
    virtual void updateAnalogInput(int ID, int state);
    virtual void updateAirSensor(int ID, float temperature, float humidity);

protected:
    virtual String addZeros(int number, int length);
    virtual Output *getDeviceFromID(int ID);
    virtual int getIntFromString(String &string, int position, int lenght);
    HardwareSerial &m_serial;
    Output **m_deviceList;
    int m_devicesNumber;
    Output **m_remoteDeviceList;
    int m_remoteDeviceNumber;
    ColorMode *m_colorMode;
    RainbowMode *m_rainbowMode;
};

#endif