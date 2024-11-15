/**
 * @file device/interface/HomeAssistant.cpp
 * @author Louis L
 * @brief Classe gérant la communication avec Home Assistant.
 * @version 2.0
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "HomeAssistant.hpp"
#include "device/interface/display.hpp"
#include "device/output/alarm.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/connectedOutput.hpp"
#include "device/output/output.hpp"
#include "device/output/RGBLEDStrip.hpp"
#include "device/output/television.hpp"
#include "device/output/tray.hpp"
#include "device/input/input.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param serial Le port série utilisé pour la communication entre l'Arduino et l'ESP.
/// @param display L'écran utilisé par le système de domotique.
HomeAssistant::HomeAssistant(const __FlashStringHelper *friendlyName, unsigned int ID, HardwareSerial &serial, Display &display) : Device(friendlyName, ID), m_serial(serial), m_display(display), m_deviceList(nullptr), m_devicesNumber(0), m_inputDeviceList(nullptr), m_inputDevicesNumber(0), m_remoteDeviceList(nullptr), m_remoteDevicesNumber(0), m_colorModeList(nullptr), m_rainbowModeList(nullptr), m_soundreactModeList(nullptr), m_alarmModeList(nullptr), m_RGBLEDStripModesNumber(0), m_initialisationFinishTime(0) {}

/// @brief Initialise la liste des périphériques connectés.
/// @param deviceList La liste des périphériques de sortie du système de domotique connectés à Home Assistant.
/// @param devicesNumber Le nombre d'élements de la liste `deviceList`.
/// @param inputDeviceList La liste des périphériques d'entrée du système de domotique connectés à Home Assistant.
/// @param inputDevicesNumber Le nombre d'élements de la liste `inputDeviceList`.
/// @param remoteDeviceList La liste des périphériques de sortie distants (provenant de Home Assistant).
/// @param remoteDevicesNumber Le nombre d'élements de la liste `remoteDeviceList`.
/// @param colorMode La liste de modes de couleur unique utilisés pour les rubans de DEL.
/// @param rainbowMode La liste de modes d'arc-en-ciel utilisés pour les rubans de DEL.
/// @param soundreactMode La liste de modes de son-réaction utilisés pour les rubans de DEL.
/// @param alarmMode La liste de modes des alarmes utilisés pour les rubans de DEL.
/// @param RGBLEDStripModesNumber Le nombre de modes dans chaque liste (correspondant au nombre de ruban de DEL RVB donné dans `deviceList`).
void HomeAssistant::setDevices(Output *deviceList[], int &devicesNumber, Input *inputDeviceList[], int &inputDevicesNumber, ConnectedOutput *remoteDeviceList[], int &remoteDevicesNumber, RGBLEDStripMode *colorModeList[], RGBLEDStripMode *rainbowModeList[], RGBLEDStripMode *soundreactModeList[], RGBLEDStripMode *alarmModeList[], int &RGBLEDStripModesNumber)
{
    m_deviceList = deviceList;
    m_devicesNumber = devicesNumber;

    m_inputDeviceList = inputDeviceList;
    m_inputDevicesNumber = inputDevicesNumber;

    m_remoteDeviceList = remoteDeviceList;
    m_remoteDevicesNumber = remoteDevicesNumber;

    m_colorModeList = colorModeList;
    m_rainbowModeList = rainbowModeList;
    m_soundreactModeList = soundreactModeList;
    m_alarmModeList = alarmModeList;
    m_RGBLEDStripModesNumber = RGBLEDStripModesNumber;
}

/// @brief Initialise la communication avec Home Assistant. Nécessite d'avoir défini les périphériques connectés auparavant. Méthode à exécuter avant d'initialiser les autres périphériques du système de domotique.
void HomeAssistant::setup()
{
    if (m_devicesNumber == 0 || m_remoteDevicesNumber == 0 || m_operational)
        return;

    // La communication est initialisée, et un message est envoyé à l'ESP pour signaler la présence de l'Arduino.
    m_serial.begin(9600);
    m_serial.println("301");

    m_display.setup();
    m_operational = true;
}

/// @brief Boucle d'exécution des tâches liées à la communication avec l'ESP : réception des messages.
void HomeAssistant::loop()
{
    if (!m_operational)
        return;

    if((m_initialisationFinishTime != 0) && (m_initialisationFinishTime < millis()))
    {
        for (int i = 0; i < m_remoteDevicesNumber; i++)
            m_remoteDeviceList[i]->reportState();

        m_initialisationFinishTime = 0;
    }

    // On récupère de manière non-bloquante les messages en provenance de l'ESP.
    while (m_serial.available() > 0)
    {
        char letter = m_serial.read();

        if (letter == '\r')
            continue;

        if (letter == '\n')
            this->processMessage();

        else
            m_receivedMessage += letter;
    }
}

/// @brief Méthode permettant de requêter l'allumage d'un périphérique distant auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
void HomeAssistant::turnOnConnectedDevice(unsigned int ID)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(1);
}

/// @brief Méthode permettant de requêter l'arrêt d'un périphérique distant auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
void HomeAssistant::turnOffConnectedDevice(unsigned int ID)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(0);
}

/// @brief Méthode permettant de requêter le basculement de l'état d'un périphérique distant auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
void HomeAssistant::toggleConnectedDevice(unsigned int ID)
{
    m_serial.print(0);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(2);
}

/// @brief Méthode permettant de changer la température de couleur d'une lampe à température de couleur variable auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param temperature La température désirée.
void HomeAssistant::setConnectedTemperatureVariableLightTemperature(unsigned int ID, int temperature)
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

/// @brief Méthode permettant de changer la luminosité d'une lampe à température de couleur variable auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param luminosity La luminosité désirée.
void HomeAssistant::setConnectedTemperatureVariableLightLuminosity(unsigned int ID, int luminosity)
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

/// @brief Méthode permettant de changer la couleur d'une lampe à couleur variable auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param r L'intensité de la composante rouge de la couleur.
/// @param g L'intensité de la composante verte de la couleur.
/// @param b L'intensité de la composante bleue de la couleur.
void HomeAssistant::setConnectedColorVariableLightColor(unsigned int ID, int r, int g, int b)
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

/// @brief Méthode permettant de changer la température de couleur d'une lampe à couleur variable auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param temperature La température désirée.
void HomeAssistant::setConnectedColorVariableLightTemperature(unsigned int ID, int temperature)
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

/// @brief Méthode permettant de changer la luminosité d'une lampe à couleur variable auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques connectés : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param luminosity La luminosité désirée.
void HomeAssistant::setConnectedColorVariableLightLuminosity(unsigned int ID, int luminosity)
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

/// @brief Méthode permettant de mettre à jour la disponibilité d'un périphérique du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param availability La disponibilité de l'appareil.
void HomeAssistant::updateDeviceAvailability(unsigned int ID, bool availability)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(0);
    m_serial.println(availability);
}

/// @brief Méthode permettant de mettre à jour l'état d'un périphérique du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param state L'état du périphérique.
void HomeAssistant::updateOutputDeviceState(unsigned int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(1);
    m_serial.println(state);
}

/// @brief Méthode permettant de mettre à jour la couleur d'un ruban de DEL RVB du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param r L'intensité de la composante rouge de la couleur.
/// @param g L'intensité de la composante verte de la couleur.
/// @param b L'intensité de la composante bleue de la couleur.
void HomeAssistant::updateRGBLEDStripMode(unsigned int ID, int mode, int r, int g, int b)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(2);
    m_serial.print(mode);

    if (mode == 0)
    {
        m_serial.print(this->addZeros(r, 3));
        m_serial.print(this->addZeros(g, 3));
        m_serial.print(this->addZeros(b, 3));
    }

    m_serial.println();
}

/// @brief Méthode permettant de mettre à jour l'état de la sonnerie d'une alarme du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param state L'état de la sonnerie de l'alarme.
void HomeAssistant::updateAlarmTriggeredState(unsigned int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(3);
    m_serial.println(state);
}

/// @brief Méthode permettant de mettre à jour l'angle de la base d'un lance-missile d'une alarme du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param angle L'angle de la base du lance-missile.
void HomeAssistant::updateAlarmMissileLauncherBaseAngle(unsigned int ID, int angle)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(3);
    m_serial.print(2);
    m_serial.println(this->addZeros(angle, 3));
}

/// @brief Méthode permettant de mettre à jour l'angle de l'inclinaison d'un lance-missile d'une alarme du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param angle L'angle de l'inclinaison du lance-missile.
void HomeAssistant::updateAlarmMissileLauncherAngleAngle(unsigned int ID, int angle)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(3);
    m_serial.print(3);
    m_serial.println(this->addZeros(angle, 3));
}

/// @brief Méthode permettant de mettre à jour les missiles chargés d'un lance-missile d'une alarme du système auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param firstMissile L'état du premier missile.
/// @param secondMissile L'état du second missile.
/// @param thirdMissile L'état du troisième missile.
void HomeAssistant::updateAlarmMissileLauncherMissilesState(unsigned int ID, bool firstMissile, bool secondMissile, bool thirdMissile)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(3);
    m_serial.print(4);
    m_serial.print(firstMissile);
    m_serial.print(secondMissile);
    m_serial.println(thirdMissile);
}

/// @brief Méthode permettant de mettre à jour le volume d'une télévision auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param mode Le mode actuel du son (`0` : volume modifié ; `1` : son coupé ; `2` : son réetabli).
/// @param volume Le volume actuel.
void HomeAssistant::updateTelevisionVolume(unsigned int ID, int mode, int volume)
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

/// @brief Méthode permettant de mettre à jour l'état d'un capteur binaire auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param state L'état du capteur.
void HomeAssistant::updateBinaryInput(unsigned int ID, bool state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(7);
    m_serial.println(state);
}

/// @brief Méthode permettant de mettre à jour l'état d'un capteur analogique auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param state L'état du capteur.
void HomeAssistant::updateAnalogInput(unsigned int ID, int state)
{
    m_serial.print(1);
    m_serial.print(this->addZeros(ID, 2));
    m_serial.print(0);
    m_serial.print(8);
    m_serial.println(this->addZeros(state, 4));
}

/// @brief Méthode permettant de mettre à jour l'état d'un capteur d'air (température & humidité) auprès de l'ESP (méthode à utiliser uniquement pas les classes de périphériques : la méthode ne doit pas être appelée depuis n'importe où).
/// @param ID L'identifiant unique du périphérique.
/// @param temperature La température actuelle.
/// @param humidity L'humidité relative actuelle.
void HomeAssistant::updateAirSensor(unsigned int ID, float temperature, float humidity)
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

/// @brief Méthode permettant de dire un message sur une enceinte connectée.
/// @param message Le message qui sera énoncé par une voix générée.
void HomeAssistant::sayMessage(String message)
{
    m_serial.print(2);
    m_serial.println(message);
}

/// @brief Méthode permettant de démarrer la lecture d'une vidéo sur une télévision
/// @param videoURL L'URL de la vidéo à lire.
void HomeAssistant::playVideo(String videoURL)
{
    m_serial.print(4);
    m_serial.println(videoURL);
}

/// @brief Méthode permettant d'éteidre l'alimentation du système de domotique.
/// @param restart Redémarre ou non l'alimentation quelques secondes plus tard.
void HomeAssistant::stopSystem(bool restart)
{
    m_serial.print(3);
    m_serial.print(0);
    m_serial.print(2);
    m_serial.println(restart);
}

/// @brief Traitement des messages reçus.
void HomeAssistant::processMessage()
{
    // Traitement du message reçu afin d'exécuter l'action demandée.
    switch (this->getIntFromString(m_receivedMessage, 0, 1))
    {
    // Requête d'un ordre.
    case 0:
    {
        // Récupération du périphérique de sortie à partir de son ID.
        Output *output = this->getDeviceFromID(this->getIntFromString(m_receivedMessage, 1, 2));

        if (output == nullptr)
            break;

        switch (this->getIntFromString(m_receivedMessage, 3, 2))
        {
        // Gestion de l'alimentation.
        case 0:
        {
            switch (this->getIntFromString(m_receivedMessage, 5, 1))
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

            switch (this->getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
            {
                ColorMode *mode = static_cast<ColorMode *>(this->getRGBLEDStripModeFromID(strip->getID(), m_colorModeList));
                mode->setColor(this->getIntFromString(m_receivedMessage, 6, 3), this->getIntFromString(m_receivedMessage, 9, 3), this->getIntFromString(m_receivedMessage, 12, 3));
                m_display.displayLEDState(mode->getR(), mode->getG(), mode->getB());
                strip->setMode(static_cast<RGBLEDStripMode *>(mode), true);
                break;
            }

            case 1:
            {
                strip->setMode(static_cast<RGBLEDStripMode *>(this->getRGBLEDStripModeFromID(strip->getID(), m_rainbowModeList)), true);
                break;
            }

            case 2:
            {
                strip->setMode(static_cast<RGBLEDStripMode *>(this->getRGBLEDStripModeFromID(strip->getID(), m_soundreactModeList)), true);
                break;
            }

            case 3:
            {
                strip->setMode(static_cast<RGBLEDStripMode *>(this->getRGBLEDStripModeFromID(strip->getID(), m_alarmModeList)), true);
                break;
            }
            }

            break;
        }

        // Gestion de l'alarme.
        case 2:
        {
            Alarm *alarm = static_cast<Alarm *>(output);

            switch (this->getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                alarm->stopRinging();
                break;

            case 1:
                alarm->trigger();
                break;

            case 2:
                alarm->getMissileLauncher().absoluteMove(BASE, this->getIntFromString(m_receivedMessage, 6, 3));
                break;

            case 3:
                alarm->getMissileLauncher().absoluteMove(ANGLE, this->getIntFromString(m_receivedMessage, 6, 3));
                break;

            case 4:
                alarm->getMissileLauncher().launchMissile(1);
                break;
            }

            break;
        }

        // Gestion de la télévision.
        case 3:
        {
            Television *television = static_cast<Television *>(output);

            switch (this->getIntFromString(m_receivedMessage, 5, 1))
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

        switch (this->getIntFromString(m_receivedMessage, 3, 2))
        {
        // Mise à jour de la disponibilité.
        case 0:
        {
            switch (this->getIntFromString(m_receivedMessage, 5, 1))
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
            switch (this->getIntFromString(m_receivedMessage, 5, 1))
            {
            case 0:
                output->updateOff();
                break;

            case 1:
                output->updateOn();
                break;
            }

            break;
        }

        // Gestion de l'ampoule à température de couleur variable.
        case 5:
        {
            ConnectedTemperatureVariableLight *light = static_cast<ConnectedTemperatureVariableLight *>(output);

            switch (this->getIntFromString(m_receivedMessage, 5, 1))
            {
            case 2:
                light->updateColorTemperature(this->getIntFromString(m_receivedMessage, 6, 4));
                break;

            case 3:
                light->updateLuminosity(this->getIntFromString(m_receivedMessage, 6, 3));
                break;
            }

            break;
        }

        // Gestion de l'ampoule à couleur variable.
        case 6:
        {
            ConnectedColorVariableLight *light = static_cast<ConnectedColorVariableLight *>(output);

            switch (this->getIntFromString(m_receivedMessage, 5, 1))
            {
            case 2:
                light->updateColor(this->getIntFromString(m_receivedMessage, 6, 3), this->getIntFromString(m_receivedMessage, 9, 3), this->getIntFromString(m_receivedMessage, 12, 3));
                break;

            case 3:
                light->updateColorTemperature(this->getIntFromString(m_receivedMessage, 6, 4));
                break;

            case 4:
                light->updateLuminosity(this->getIntFromString(m_receivedMessage, 6, 3));
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

    // Configuration de la connexion.
    case 3:
    {
        switch (this->getIntFromString(m_receivedMessage, 1, 2))
        {
        case 0:
        {
            for (int i = 0; i < m_devicesNumber; i++)
                m_deviceList[i]->reportState();

            for (int i = 0; i < m_inputDevicesNumber; i++)
                m_inputDeviceList[i]->reportState();

            for (int i = 0; i < m_remoteDevicesNumber; i++)
                m_remoteDeviceList[i]->reportState();

            // Les périphériques connectés ont leur état mis à jour en deux temps, la méthode sera de nouveau exécuté plus tard.
            m_initialisationFinishTime = millis() + 5000;

            break;
        }
        }

        break;
    }
    }

    m_receivedMessage = "";
}

/// @brief Méthode permettant de récupérer un périphérique entregistré à partir de son identifiant.
/// @param ID L'identifiant unique de communication du périphérique.
/// @return Un pointeur vers le périphérique s'il a été trouvé, ou vers `nullptr` si ce n'est pas le cas.
Output *HomeAssistant::getDeviceFromID(unsigned int ID)
{
    for (int i = 0; i < m_devicesNumber; i++)
    {
        if (m_deviceList[i]->getID() == ID)
            return m_deviceList[i];
    }

    return nullptr;
}

/// @brief Méthode permettant de récupérer un périphérique distant entregistré à partir de son identifiant.
/// @param ID L'identifiant unique de communication du périphérique distant.
/// @return Un pointeur vers le périphérique distant s'il a été trouvé, ou vers `nullptr` si ce n'est pas le cas.
ConnectedOutput *HomeAssistant::getRemoteDeviceFromID(unsigned int ID)
{
    for (int i = 0; i < m_remoteDevicesNumber; i++)
    {
        if (m_remoteDeviceList[i]->getID() == ID)
            return static_cast<ConnectedOutput *>(m_remoteDeviceList[i]);
    }

    return nullptr;
}

/// @brief Méthode permettant de récupérer un mode de ruban de DEL RVB entregistré à partir de l'identifiant de son ruban.
/// @param ID L'identifiant unique de communication du ruban.
/// @param list La liste contenant un mode spécifique (couleur unique, arc-en-ciel...) de tous les rubans de DEL.
/// @return Un pointeur vers le mode s'il a été trouvé, ou vers `nullptr` si ce n'est pas le cas.
RGBLEDStripMode *HomeAssistant::getRGBLEDStripModeFromID(unsigned int ID, RGBLEDStripMode **list)
{
    for (int i = 0; i < m_RGBLEDStripModesNumber; i++)
    {
        if (list[i]->getStripID() == ID)
            return list[i];
    }

    return nullptr;
}

/// @brief Méthode permettant de convertir un entier en une chaîne de caractère à longueur fixe, complétée de `0` si nécessaire.
/// @param number L'entier à convertir.
/// @param length La longueur de la chaîne de caractères désirée.
/// @return L'entier formaté.
String HomeAssistant::addZeros(int number, int length)
{
    String result = String(number);
    while (result.length() < (unsigned int)length)
        result = "0" + result;

    return result;
}

/// @brief Méthode permettant d'extraire un entier à partir d'une chaîne de caractères
/// @param string La chaîne de caractères contenant l'entier.
/// @param position La position du début de l'entier dans le `string`.
/// @param lenght La longueur maximale de l'entier (peut-être complété de `0`).
/// @return L'entier extrait.
int HomeAssistant::getIntFromString(const String &string, int position, int lenght)
{
    int result = 0;

    for (int i = 0; i < lenght; i++)
    {
        int power = 1;

        for (int j = 0; j < ((lenght - i) - 1); j++)
            power *= 10;

        result += (string.charAt(position + i) - '0') * power;
    }

    return result;
}