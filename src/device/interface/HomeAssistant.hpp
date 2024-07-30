#ifndef HOME_ASSISTANT_DEFINITIONS
#define HOME_ASSISTANT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/device.hpp"
#include "device/interface/display.hpp"

// Déclaration des classes utilisées par la classe HomeAssistant.
class Output;
class ColorMode;
class RainbowMode;
class SoundreactMode;
class AlarmMode;
class RGBLEDStrip;
class RGBLEDStripMode;
class Alarm;
class Television;
class Input;
class ConnectedOutput;
class ConnectedTemperatureVariableLight;
class ConnectedColorVariableLight;

// Classe de gestion de la communication avec l'ESP.
class HomeAssistant : public Device
{
public:
    HomeAssistant(const __FlashStringHelper *friendlyName, unsigned int ID, HardwareSerial &serial, Display &display);
    virtual void setDevices(Output *deviceList[], int &devicesNumber, Input *inputDeviceList[], int &inputDevicesNumber, ConnectedOutput *remoteDeviceList[], int &remoteDevicesNumber, RGBLEDStripMode *colorModeList[], RGBLEDStripMode *rainbowModeList[], RGBLEDStripMode *soundreactModeList[], RGBLEDStripMode *alarmModeList[], int &RGBLEDStripModesNumber);
    virtual void setup() override;
    virtual void loop() override;
    virtual void turnOnConnectedDevice(unsigned int ID);
    virtual void turnOffConnectedDevice(unsigned int ID);
    virtual void toggleConnectedDevice(unsigned int ID);
    virtual void setConnectedTemperatureVariableLightTemperature(unsigned int ID, int temperature);
    virtual void setConnectedTemperatureVariableLightLuminosity(unsigned int ID, int luminosity);
    virtual void setConnectedColorVariableLightColor(unsigned int ID, int r, int g, int b);
    virtual void setConnectedColorVariableLightTemperature(unsigned int ID, int temperature);
    virtual void setConnectedColorVariableLightLuminosity(unsigned int ID, int luminosity);
    virtual void updateDeviceAvailability(unsigned int ID, bool availability);
    virtual void updateOutputDeviceState(unsigned int ID, bool state);
    virtual void updateRGBLEDStripMode(unsigned int ID, int mode, int r = 0, int g = 0, int b = 0);
    virtual void updateAlarmTriggeredState(unsigned int ID, bool state);
    virtual void updateAlarmMissileLauncherBaseAngle(unsigned int ID, int angle);
    virtual void updateAlarmMissileLauncherAngleAngle(unsigned int ID, int angle);
    virtual void updateAlarmMissileLauncherMissilesState(unsigned int ID, bool firstMissile, bool secondMissile, bool thirdMissile);
    virtual void updateTelevisionVolume(unsigned int ID, int mode, int volume = 0);
    virtual void updateBinaryInput(unsigned int ID, bool state);
    virtual void updateAnalogInput(unsigned int ID, int state);
    virtual void updateAirSensor(unsigned int ID, float temperature, float humidity);
    virtual void sayMessage(String message);
    virtual void playVideo(String videoURL);
    virtual void stopSystem(bool restart = false);

protected:
    virtual void processMessage();
    virtual Output *getDeviceFromID(unsigned int ID);
    virtual ConnectedOutput *getRemoteDeviceFromID(unsigned int ID);
    virtual RGBLEDStripMode *getRGBLEDStripModeFromID(unsigned int ID, RGBLEDStripMode **list);
    static String addZeros(int number, int length);
    static int getIntFromString(const String &string, int position, int lenght);
    HardwareSerial &m_serial;
    String m_receivedMessage;
    Display &m_display;
    Output **m_deviceList;
    int m_devicesNumber;
    Input **m_inputDeviceList;
    int m_inputDevicesNumber;
    ConnectedOutput **m_remoteDeviceList;
    int m_remoteDevicesNumber;
    RGBLEDStripMode **m_colorModeList;
    RGBLEDStripMode **m_rainbowModeList;
    RGBLEDStripMode **m_soundreactModeList;
    RGBLEDStripMode **m_alarmModeList;
    int m_RGBLEDStripModesNumber;
    unsigned long m_reportStateMillis;
};

#endif