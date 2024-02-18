#ifndef HOME_ASSISTANT_DEFINITIONS
#define HOME_ASSISTANT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "../device.hpp"

#define UART_WAITING_TIME 320 / 115200 + 1

// Classe de gestion de la communication avec l'ESP.
class HomeAssistant
{
public:
    HomeAssistant(HardwareSerial &serial);
    virtual void setDevices(Device *deviceList[], int &devicesNumber, Device *remoteDeviceList[], int &remoteDeviceNumber);
    virtual void setup();
    virtual void loop();

protected:
    HardwareSerial &m_serial;
    Device **m_deviceList;
    int m_devicesNumber;
    Device **m_remoteDeviceList;
    int m_remoteDeviceNumber;
};

#endif