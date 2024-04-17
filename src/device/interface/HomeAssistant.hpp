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
    HomeAssistant(String friendlyName, int ID, HardwareSerial &serial, Display &display);
    virtual void setDevices(Output *deviceList[], int &devicesNumber, Input *inputDeviceList[], int &inputDevicesNumber, Output *remoteDeviceList[], int &remoteDevicesNumber, ColorMode &colorMode, RainbowMode &rainbowMode, SoundreactMode &soundreactMode, AlarmMode &alarmMode);
    virtual void setup() override;
    virtual void loop();
    virtual void processMessage();
    virtual void turnOnConnectedDevice(int ID);
    virtual void turnOffConnectedDevice(int ID);
    virtual void toggleConnectedDevice(int ID);
    virtual void setConnectedTemperatureVariableLightTemperature(int ID, int temperature);
    virtual void setConnectedTemperatureVariableLightLuminosity(int ID, int luminosity);
    virtual void setConnectedColorVariableLightColor(int ID, int r, int g, int b);
    virtual void setConnectedColorVariableLightTemperature(int ID, int temperature);
    virtual void setConnectedColorVariableLightLuminosity(int ID, int luminosity);
    virtual void updateDeviceAvailability(int ID, bool availability);
    virtual void updateOutputDeviceState(int ID, bool state);
    virtual void updateRGBLEDStripMode(int ID, int mode, int r = 0, int g = 0, int b = 0);
    virtual void updateAlarmTriggeredState(int ID, bool state);
    virtual void updateAlarmMissileLauncherBaseAngle(int ID, int angle);
    virtual void updateAlarmMissileLauncherAngleAngle(int ID, int angle);
    virtual void updateAlarmMissileLauncherMissilesState(int ID, int firstMissile, int secondMissile, int thirdMissile);
    virtual void updateTelevisionVolume(int ID, int mode, int volume = 0);
    virtual void updateBinaryInput(int ID, bool state);
    virtual void updateAnalogInput(int ID, int state);
    virtual void updateAirSensor(int ID, float temperature, float humidity);
    virtual void sayMessage(String message);

protected:
    virtual Output *getDeviceFromID(int ID);
    virtual ConnectedOutput *getRemoteDeviceFromID(int ID);
    static String addZeros(int number, int length);
    static int getIntFromString(String &string, int position, int lenght);
    HardwareSerial &m_serial;
    String m_receivedMessage;
    Display &m_display;
    Output **m_deviceList;
    int m_devicesNumber;
    Input **m_inputDeviceList;
    int m_inputDevicesNumber;
    Output **m_remoteDeviceList;
    int m_remoteDevicesNumber;
    ColorMode *m_colorMode;
    RainbowMode *m_rainbowMode;
    SoundreactMode *m_soundreactMode;
    AlarmMode *m_alarmMode;
};

#endif