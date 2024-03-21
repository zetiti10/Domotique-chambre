/**
 * @file device/interface/HomeAssistant.cpp
 * @author Louis L
 * @brief Classe gérant la communication avec Home Assistant.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/interface/HomeAssistant.hpp"
#include "device/interface/display.hpp"
#include "device/output/alarm.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/connectedOutput.hpp"
#include "device/output/output.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/television.hpp"
#include "device/output/tray.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param serial Le port série utilisé pour la communication entre l'Arduino et l'ESP.
HomeAssistant::HomeAssistant(String friendlyName, int ID, HardwareSerial &serial, Display &display) : Device(friendlyName, ID), m_serial(serial), m_display(display), m_deviceList(nullptr), m_devicesNumber(0), m_remoteDeviceList(nullptr), m_remoteDevicesNumber(0), m_colorMode(nullptr), m_rainbowMode(nullptr) {}

/// @brief Initialise la liste des périphériques connectés.
/// @param deviceList La liste des périphériques de sortie du système de domotique connectés à Home Assistant.
/// @param devicesNumber Le nombre d'élements de la liste `deviceList`.
/// @param remoteDeviceList La liste des périphériques de sortie distants (provenant de Home Assistant).
/// @param remoteDevicesNumber Le nombre d'élements de la liste `remoteDeviceList`.
/// @param colorMode Le mode de couleur unique utilisé pour le ruban de DEL.
/// @param colorMode Le mode multicolore utilisé pour le ruban de DEL.
void HomeAssistant::setDevices(Output *deviceList[], int &devicesNumber, Output *remoteDeviceList[], int &remoteDevicesNumber, ColorMode &colorMode, RainbowMode &rainbowMode)
{
    m_deviceList = deviceList;
    m_devicesNumber = devicesNumber;

    m_remoteDeviceList = remoteDeviceList;
    m_remoteDevicesNumber = remoteDevicesNumber;

    m_colorMode = &colorMode;
    m_rainbowMode = &rainbowMode;
    // Mode son-réaction.
}

/// @brief Initialise la communication avec Home Assistant. Nécessite d'avoir défini les périphériques connectés auparavant. Méthode à exécuter avant d'initialiser les autres périphériques du système de domotique.
void HomeAssistant::setup()
{
    if (m_devicesNumber == 0 || m_remoteDevicesNumber == 0 || m_operational)
        return;

    m_serial.begin(9600);

    m_display.setup();

    m_operational = true;
}

/// @brief Boucle d'exécution des tâches liées à la communication avec l'ESP : réception des messages.
void HomeAssistant::loop()
{
    if (!m_operational)
        return;

    while (m_serial.available() > 0)
    {
        char letter = m_serial.read();

        if (letter == '\r')
            continue;

        if (letter == '\n')
            processMessage();

        else
            m_receivedMessage += letter;
    }
}

/// @brief Traitement des messages reçus
void HomeAssistant::processMessage()
{
    // Traitement du message reçu afin d'exécuter l'action demandée.
    switch (getIntFromString(m_receivedMessage, 0, 1))
    {
    // Requête d'un ordre.
    case 0:
    {
        // Récupération du périphérique de sortie à partir de son ID.
        Output *output = this->getDeviceFromID(this->getIntFromString(m_receivedMessage, 1, 2));

        if (output == nullptr)
            break;

        switch (getIntFromString(m_receivedMessage, 3, 2))
        {
        // Gestion de l'alimentation.
        case 0:
        {
            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                output->turnOff(true);
                break;

            case 1:
                output->turnOn(true);
                break;

            case 2:
                output->toggle(true);
                break;
            }

            break;
        }

        // Gestion des rubans de DEL.
        case 1:
        {
            RGBLEDStrip *strip = static_cast<RGBLEDStrip *>(output);

            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                m_colorMode->setColor(this->getIntFromString(m_receivedMessage, 5, 3), this->getIntFromString(m_receivedMessage, 8, 3), this->getIntFromString(m_receivedMessage, 11, 3));
                strip->setMode(static_cast<RGBLEDStripMode *>(m_colorMode), true);
                break;

            case 1:
                strip->setMode(static_cast<RGBLEDStripMode *>(m_rainbowMode), true);
                break;

            case 2:
                // strip->setMode(*static_cast<RGBLEDStripMode *>(m_rainbowMode), true);
                // Mode son-réaction.
                break;
            }

            break;
        }

        // Gestion de l'alarme.
        case 2:
        {
            Alarm *alarm = static_cast<Alarm *>(output);

            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                alarm->stopRinging();
                break;

            case 1:
                alarm->trigger();
                break;
            }

            break;
        }

        // Gestion de la télévision.
        case 3:
        {
            Television *television = static_cast<Television *>(output);

            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                television->decreaseVolume(true);
                break;

            case 1:
                television->increaseVolume(true);
                break;

            case 2:
                television->mute(true);
                break;

            case 3:
                television->unMute(true);
                break;

            case 4:
                television->toggleMute(true);
                break;
            }

            break;
        }
        }

        break;
    }

    // Mise à jour d'un état.
    case 1:
    {
        // Récupération du périphérique distant à partir de son ID.
        ConnectedOutput *output = this->getRemoteDeviceFromID(this->getIntFromString(m_receivedMessage, 1, 2));

        if (output == nullptr)
            break;

        switch (getIntFromString(m_receivedMessage, 3, 2))
        {
        // Mise à jour de la disponibilité.
        case 0:
        {
            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                output->setUnavailable();
                break;

            case 1:
                output->setAvailable();
                break;
            }

            break;
        }

        // Gestion de l'alimentation.
        case 1:
        {
            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                output->updateOff(true);
                break;

            case 1:
                output->updateOn(true);
                break;
            }

            break;
        }

        // Gestion de l'ampoule à température de couleur variable.
        case 5:
        {
            ConnectedTemperatureVariableLight *light = static_cast<ConnectedTemperatureVariableLight *>(output);

            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 2:
                light->updateColorTemperature(this->getIntFromString(m_receivedMessage, 6, 4), true);
                break;

            case 3:
                light->updateLuminosity(this->getIntFromString(m_receivedMessage, 6, 3), true);
                break;
            }

            break;
        }

        // Gestion de l'ampoule à couleur variable.
        case 6:
        {
            ConnectedColorVariableLight *light = static_cast<ConnectedColorVariableLight *>(output);

            switch (getIntFromString(m_receivedMessage, 5, 1))
            {
            case 2:
                light->updateColor(this->getIntFromString(m_receivedMessage, 6, 3), this->getIntFromString(m_receivedMessage, 9, 3), this->getIntFromString(m_receivedMessage, 12, 3), true);
                break;

            case 3:
                light->updateColorTemperature(this->getIntFromString(m_receivedMessage, 6, 4), true);
                break;

            case 4:
                light->updateLuminosity(this->getIntFromString(m_receivedMessage, 6, 3), true);
                break;
            }

            break;
        }
        }

        break;
    }

    // Affichage d'un message sur l'écran.
    case 2:
    {
        // Détection du "/" qui délimite le titre du message.
        for (unsigned int i = 0; i < m_receivedMessage.length(); i++)
        {
            if (m_receivedMessage.charAt(i) == '/')
            {
                m_display.displayMessage(m_receivedMessage.substring(i + 1), m_receivedMessage.substring(1, i));

                break;
            }
        }

        break;
    }
    }

    m_receivedMessage = "";
}

void HomeAssistant::turnOnConnectedDevice(int ID)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(1);
}

void HomeAssistant::turnOffConnectedDevice(int ID)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(0);
}

void HomeAssistant::toggleConnectedDevice(int ID)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(2);
}

void HomeAssistant::setConnectedTemperatureVariableLightTemperature(int ID, int temperature)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(40);

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
    m_serial.print(0);
    m_serial.print(41);

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
    m_serial.print(0);
    m_serial.print(50);

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
    m_serial.print(0);
    m_serial.print(51);

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
    m_serial.print(0);
    m_serial.print(52);

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
    m_serial.print(0);
    m_serial.println(availability);
}

void HomeAssistant::updateOutputDeviceState(int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(1);
    m_serial.println(state);
}

void HomeAssistant::updateRGBLEDStripMode(int ID, int mode, int r, int g, int b)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
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
    m_serial.print(0);
    m_serial.print(3);
    m_serial.println(state);
}

void HomeAssistant::updateTelevisionVolume(int ID, int mode, int volume)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(4);
    m_serial.print(mode);

    if (mode == 0)
        m_serial.print(this->addZeros(volume, 2));

    m_serial.println();
}

void HomeAssistant::updateBinaryInput(int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(7);
    m_serial.println(state);
}

void HomeAssistant::updateAnalogInput(int ID, int state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(8);
    m_serial.println(this->addZeros(state, 4));
}

void HomeAssistant::updateAirSensor(int ID, float temperature, float humidity)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(9);

    int temp = temperature * 100;
    int hum = humidity * 100;

    m_serial.print(temp);
    m_serial.println(hum);
}

void HomeAssistant::sayMessage(String message)
{
    m_serial.print(2);
    m_serial.println(message);
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

ConnectedOutput *HomeAssistant::getRemoteDeviceFromID(int ID)
{
    for (int i = 0; i < m_remoteDevicesNumber; i++)
    {
        if (m_remoteDeviceList[i]->getID() == ID)
            return static_cast<ConnectedOutput *>(m_remoteDeviceList[i]);
    }

    return nullptr;
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

int HomeAssistant::getIntFromString(String &string, int position, int lenght)
{
    int result = 0;

    for (int i = 0; i < lenght; i++)
        result += (string.charAt(position + i) - '0') * pow(10, ((lenght - i) - 1));

    return result;
}