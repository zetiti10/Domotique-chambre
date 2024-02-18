/**
 * @file device/interface/HomeAssistant.cpp
 * @author Louis L
 * @brief Classe gérant la communication avec Home Assistant.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "HomeAssistant.hpp"

HomeAssistant::HomeAssistant(HardwareSerial &serial) : m_serial(serial), m_deviceList(nullptr), m_devicesNumber(0), m_remoteDeviceList(nullptr), m_remoteDeviceNumber(0) {}

void HomeAssistant::setDevices(Device *deviceList[], int &devicesNumber, Device *remoteDeviceList[], int &remoteDeviceNumber)
{
    m_deviceList = deviceList;
    m_devicesNumber = devicesNumber;

    m_remoteDeviceList = remoteDeviceList;
    m_remoteDeviceNumber = remoteDeviceNumber;
}

void HomeAssistant::setup()
{
    if (m_devicesNumber == 0 || m_remoteDeviceNumber == 0)
        return;

    m_serial.begin(9600);
}

void HomeAssistant::loop()
{
    if (m_serial.available())
    {
        delay(UART_WAITING_TIME);

        String receivedMessage;
        while (m_serial.available() > 0)
        {
            char letter = m_serial.read();

            if(letter == '\n')
                break;

            receivedMessage += letter;
        }

        Serial.println(receivedMessage);
    }

    // Juste pour le test
    if (Serial.available())
    {
        delay(UART_WAITING_TIME);

        String receivedMessage;
        while (Serial.available() > 0)
        {
            char letter = Serial.read();

            if(letter == '\n')
                break;

            receivedMessage += letter;
        }

        m_serial.println(receivedMessage);
    }
}