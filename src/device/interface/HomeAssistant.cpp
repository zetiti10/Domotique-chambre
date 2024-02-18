/**
 * @file device/interface/HomeAssistant.cpp
 * @author Louis L
 * @brief Classe gérant la communication avec Home Assistant.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "HomeAssistant.hpp"

HomeAssistant::HomeAssistant(String friendlyName, int ID, HardwareSerial &serial) : Device(friendlyName, ID), m_serial(serial), m_deviceList(nullptr), m_devicesNumber(0), m_remoteDeviceList(nullptr), m_remoteDeviceNumber(0), m_colorMode(nullptr), m_rainbowMode(nullptr) {}

void HomeAssistant::setDevices(Output *deviceList[], int &devicesNumber, Output *remoteDeviceList[], int &remoteDeviceNumber, ColorMode &colorMode, RainbowMode &rainbowMode)
{
    m_deviceList = deviceList;
    m_devicesNumber = devicesNumber;

    m_remoteDeviceList = remoteDeviceList;
    m_remoteDeviceNumber = remoteDeviceNumber;

    m_colorMode = &colorMode;
    m_rainbowMode = &rainbowMode;
}

void HomeAssistant::setup()
{
    if (m_devicesNumber == 0 || m_remoteDeviceNumber == 0)
        return;

    m_serial.begin(9600);

    m_serial.println("2");

    unsigned long initialTime = millis();

    while (!m_serial.available())
    {
        if ((millis() - initialTime) >= 5000)
            return;
    }

    while (m_serial.available() > 0)
    {
        char letter = m_serial.read();

        if (letter == '\n')
            break;
    }

    m_operational = true;
}

void HomeAssistant::loop()
{
    if (!m_operational)
        return;

    if (!m_serial.available())
        return;

    delay(UART_WAITING_TIME);

    String receivedMessage;
    while (m_serial.available() > 0)
    {
        char letter = m_serial.read();

        if (letter == '\n')
            break;

        receivedMessage += letter;
    }

    Serial.println(receivedMessage);

    int ID = ((receivedMessage.charAt(1) - '0') * 10) + (receivedMessage.charAt(2) - '0');

    if (receivedMessage.charAt(0) == '0')
    {
        if (receivedMessage.charAt(3) == '0')
        {
            switch (receivedMessage.charAt(4))
            {
            case '0':
                this->getDeviceFromID(ID)->turnOff(true);
                break;

            case '1':
                this->getDeviceFromID(ID)->turnOn(true);
                break;

            case '2':
                this->getDeviceFromID(ID)->toggle(true);
                break;

            default:
                break;
            }

            return;
        }

        RGBLEDStrip *strip = dynamic_cast<RGBLEDStrip *>(this->getDeviceFromID(ID));
        if (strip)
        {
            if (receivedMessage.charAt(3) == '0')
            {
                switch (receivedMessage.charAt(4))
                {
                case '0':
                    int r = ((receivedMessage.charAt(5) - '0') * 100) + ((receivedMessage.charAt(6) - '0') * 10) + (receivedMessage.charAt(7) - '0');
                    int g = ((receivedMessage.charAt(8) - '0') * 100) + ((receivedMessage.charAt(9) - '0') * 10) + (receivedMessage.charAt(10) - '0');
                    int b = ((receivedMessage.charAt(11) - '0') * 100) + ((receivedMessage.charAt(12) - '0') * 10) + (receivedMessage.charAt(13) - '0');
                    m_colorMode->setColor(r, g, b);
                    strip->setMode(*dynamic_cast<RGBLEDStripMode *>(m_colorMode), true);
                    break;

                case '1':
                    strip->setMode(*dynamic_cast<RGBLEDStripMode *>(m_rainbowMode), true);
                    break;

                case '2':
                    //strip->setMode(*dynamic_cast<RGBLEDStripMode *>(m_rainbowMode), true);
                    // Mode son-réaction.
                    break;

                default:
                    break;
                }

                return;
            }
        }
    }
}

void HomeAssistant::setConnectedTemperatureVariableLightTemperature(int ID, int temperature)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(1);

    if (temperature < 2000)
        temperature = 2000;

    if (temperature > 5000)
        temperature = 5000;

    m_serial.println(this->addZeros(temperature, 4));
}

void HomeAssistant::setConnectedTemperatureVariableLightLuminosity(int ID, int luminosity)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);

    if (luminosity < 0)
        luminosity = 0;

    if (luminosity > 255)
        luminosity = 255;

    m_serial.println(this->addZeros(luminosity, 3));
}

void HomeAssistant::setConnectedColorVariableLightColor(int ID, int r, int g, int b)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(1);

    if (r < 0)
        r = 0;

    if (r > 255)
        r = 255;

    if (g < 0)
        g = 0;

    if (g > 255)
        g = 255;

    if (b < 0)
        b = 0;

    if (b > 255)
        b = 255;

    m_serial.print(this->addZeros(r, 3));
    m_serial.print(this->addZeros(g, 3));
    m_serial.println(this->addZeros(b, 3));
}

void HomeAssistant::setConnectedColorVariableLightTemperature(int ID, int temperature)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);

    if (temperature < 2202)
        temperature = 2202;

    if (temperature > 6535)
        temperature = 6535;

    m_serial.println(this->addZeros(temperature, 4));
}

void HomeAssistant::setConnectedColorVariableLightLuminosity(int ID, int luminosity)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(3);

    if (luminosity < 0)
        luminosity = 0;

    if (luminosity > 255)
        luminosity = 255;

    m_serial.println(this->addZeros(luminosity, 3));
}

void HomeAssistant::updateDeviceAvailability(int ID, bool availability)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.println(availability);
}

void HomeAssistant::updateOutputDeviceState(int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(1);
    m_serial.println(state);
}

void HomeAssistant::updateRGBLEDStripMode(int ID, int mode, int r, int g, int b)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);
    m_serial.println(mode);

    if (mode == 0)
    {
        m_serial.print(this->addZeros(r, 3));
        m_serial.print(this->addZeros(g, 3));
        m_serial.print(this->addZeros(b, 3));
    }

    m_serial.println();
}

void HomeAssistant::updateAlarmTriggeredState(int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);
    m_serial.println(state);
}

void HomeAssistant::updateTelevisionVolume(int ID, int mode, int volume)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);
    m_serial.print(mode);

    if (mode == 0)
        m_serial.print(this->addZeros(volume, 2));

    m_serial.println();
}

void HomeAssistant::updateBinaryInput(int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);
    m_serial.println(state);
}

void HomeAssistant::updateAnalogInput(int ID, int state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);
    m_serial.println(state);
}

void HomeAssistant::updateAirSensor(int ID, float temperature, float humidity)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(2);

    int temp = temperature * 100;
    int hum = humidity * 100;

    m_serial.print(temp);
    m_serial.println(hum);
}

String HomeAssistant::addZeros(int number, int length)
{
    String result = String(number);
    while (result.length() < (unsigned int)length)
    {
        result = "0" + result;
    }
    return result;
}

Output *HomeAssistant::getDeviceFromID(int ID)
{
    for (int i = 0; i < m_devicesNumber; i++)
    {
        if (m_deviceList[i]->getID() == ID)
            return m_deviceList[i];
    }

    return nullptr;
}
