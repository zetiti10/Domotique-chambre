/**
 * @file device/output/television.cpp
 * @author Louis L
 * @brief Classe gérant une télévision.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>
#include <EEPROM.h>

// Autres fichiers du programme.
#include "device/output/television.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "EEPROM.hpp"
#include "television.hpp"

const char unableToPerformError[] PROGMEM = "Impossible d'effectuer cette action.";
const char error[] PROGMEM = "Erreur";

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param servomotorPin La broche associée à celle du servomoteur.
/// @param IRLEDPin La broche associée à celle de la DEL infrarouge.
/// @param volume Le volume récupéré de l'EEPROM.
/// @param mode Le mode utilisé lors de vidéos animées pour contrôler le ruban de DEL RVB.
Television::Television(const __FlashStringHelper *friendlyName, int ID, HomeAssistant &connection, Display &display, int servomotorPin, int IRLEDPin, int volume, MusicsAnimationsMode &mode) : Output(friendlyName, ID, connection, display), m_servomotorPin(servomotorPin), m_IRLEDPin(IRLEDPin), m_IRSender(), m_volume(volume), m_volumeMuted(false), m_lastTime(0), m_waitingForTriggerSound(false), m_microphone(nullptr), m_musicStartTime(0), m_lastActionIndex(0), m_musicList(nullptr), m_currentMusicIndex(-1), m_musicsNumber(0), m_deviceList(nullptr), m_devicesNumber(0), m_mode(mode) {}

/// @brief Cette méthode permet d'enregistrer les périphériques du système qui pourront être contrôlés automatiquement lors de la lecture d'une vidéo.
/// @param deviceList La liste de périphériques "basiques" à utiliser (pour les allumer / éteindre).
/// @param devicesNumber Le nombre de périphériques de la liste `deviceList`.
/// @param stripList La liste de ruban de DEL RVB à utilise (pour le contrôle des couleurs).
/// @param stripsNumber Le nombre de rubans de la liste `stripList`.
/// @param connectedColorVariableLightList La liste des lampes connectées à couleur variable, à utiliser (pour le contrôle des couleurs).
/// @param connectedColorVariableLightsNumber Le nombre d'éléments de la liste `connectedColorVariableLightList`.
void Television::setMusicDevices(Output *deviceList[], int &devicesNumber)
{
    m_deviceList = deviceList;
    m_devicesNumber = devicesNumber;
}

/// @brief Méthode permettant de définir la liste des musiques disponibles à la lecture.
/// @param musicList La liste de musiques.
/// @param musicsNumber Le nombre de musiques de la liste de musiques.
void Television::setMusicsList(Music **musicList, int &musicsNumber)
{
    if (musicsNumber <= 0)
        return;

    m_musicList = musicList;
    m_musicsNumber = musicsNumber;
}

/// @brief Méthode permettant de définir le microphone utilisé par la télévision pour détecter le son clé du lancement d'une vidéo.
/// @param microphone L'objet du microphone.
void Television::setMicrophone(AnalogInput &microphone)
{
    m_microphone = &microphone;
}

/// @brief Initialise l'objet.
void Television::setup()
{
    if (m_operational || m_microphone == nullptr || m_musicsNumber == 0 || m_devicesNumber == 0)
        return;


    Output::setup();

    pinMode(m_servomotorPin, OUTPUT);
    m_IRSender.begin(m_IRLEDPin);

    m_lastTime = millis();

    m_operational = true;

    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void Television::reportState()
{
    if (!m_operational)
        return;

    Output::reportState();

    m_connection.updateTelevisionVolume(m_ID, 0, m_volume);

    if (m_volumeMuted)
        m_connection.updateTelevisionVolume(m_ID, 1);
}

/// @brief Boucle d'exécution des tâches liées à la télévision.
void Television::loop()
{
    if ((millis() - m_lastTime) >= 60000)
    {
        EEPROM.update(EEPROM_VOLUME, m_volume);

        m_lastTime = millis();
    }

    if (m_waitingForTriggerSound)
        detectTriggerSound();

    if (m_musicStartTime != 0)
        scheduleMusic();
}

/// @brief Met en marche la télévision.
/// @param shareInformation Affiche ou non l'animation d'allumage sur l'écran.
void Television::turnOn(bool shareInformation)
{
    if (!m_operational || m_locked || m_state)
        return;

    m_connection.updateOutputDeviceState(m_ID, true);

    switchDisplay();
    m_IRSender.sendNEC(0x44C1, 0x87, 3);

    m_state = true;

    if (shareInformation)
        m_display.displayDeviceState(true);
}

/// @brief Arrête la télévision.
/// @param shareInformation Affiche ou non l'animation d'arrêt sur l'écran.
void Television::turnOff(bool shareInformation)
{
    if (!m_operational || m_locked || !m_state)
        return;

    m_connection.updateOutputDeviceState(m_ID, false);

    stopMusic();
    switchDisplay();
    m_IRSender.sendNEC(0x44C1, 0x87, 3);

    m_state = false;

    if (shareInformation)
        m_display.displayDeviceState(false);
}

/// @brief Synchronise le volume virtuel avec celui de la sono.
/// @param shareInformation Affiche ou non les messages sur l'écran.
void Television::syncVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_currentMusicIndex != -1))
    {
        if (shareInformation)
            m_display.displayMessage(unableToPerformError, error);

        return;
    }

    if (shareInformation)
        m_display.displayMessage("Calibration du son...");

    IrSender.sendNEC(0x44C1, 0xC7, 50);

    for (int i = 0; i < m_volume; i++)
        increaseVolume();

    if (shareInformation)
        m_display.displayMessage("Calibration terminee !");
}

/// @brief Augmente le volume de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::increaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 25))
    {
        if (shareInformation)
            m_display.displayMessage(unableToPerformError, error);

        return;
    }

    for (int i = 0; i < 2; i++)
        m_IRSender.sendNEC(0x44C1, 0x47, 3);

    m_volume++;

    m_connection.updateTelevisionVolume(m_ID, 0, m_volume);

    if (shareInformation)
        m_display.displayVolume(INCREASE, m_volume);
}

/// @brief Diminue le volume de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::decreaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 0))
    {
        if (shareInformation)
            m_display.displayMessage(unableToPerformError, error);

        return;
    }

    for (int i = 0; i < 2; i++)
        m_IRSender.sendNEC(0x44C1, 0xC7, 3);

    m_volume--;

    m_connection.updateTelevisionVolume(m_ID, 0, m_volume);

    if (shareInformation)
        m_display.displayVolume(DECREASE, m_volume);
}

/// @brief Méthode permettant de récupérer le volume actuel de la télévision.
/// @return Le volume actuel de la télévision.
int Television::getVolume()
{
    return m_volume;
}

/// @brief Désactive le son de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::mute(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage(unableToPerformError, error);

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x77, 3);
    m_volumeMuted = true;

    m_connection.updateTelevisionVolume(m_ID, 1);

    if (shareInformation)
        m_display.displayVolume(MUTE, m_volume);
}

/// @brief Réetablie le son de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::unMute(bool shareInformation)
{
    if (!m_state || m_locked || !m_volumeMuted || !m_operational)
    {
        if (shareInformation)
            m_display.displayMessage(unableToPerformError, error);

        return;
    }

    m_IRSender.sendNEC(0x44C1, 0x77, 3);
    m_volumeMuted = false;

    m_connection.updateTelevisionVolume(m_ID, 2);

    if (shareInformation)
        m_display.displayVolume(UNMUTE, m_volume);
}

/// @brief Bascule le son de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::toggleMute(bool shareInformation)
{
    if (m_volumeMuted)
        unMute();

    else
        mute();
}

/// @brief Méthode permettant de récupérer l'état actuel du mode sourdinne de la télévision.
/// @return L'état actuel du mode sourdinne de la télévision.
bool Television::getMute()
{
    return m_volumeMuted;
}

/// @brief Méthode permettant d'obtenir la liste des musiques disponibles.
/// @return La liste des musiques disponibles.
Music **Television::getMusicsList()
{
    return m_musicList;
}

/// @brief Méthode permettant d'obtenir le nombre de musiques enregistré.
/// @return Le nombre de musiques enregistré.
int Television::getMusicNumber()
{
    return m_musicsNumber;
}

/// @brief Méthode permettant de démarrer la lecture d'une vidéo.
/// @param musicIndex La position de la musique dans la liste des musiques disponibles fournie par la méthode `Music **Television::getMusicsList()`.
void Television::playMusic(int musicIndex)
{
    // Étape 1 : vérification que toutes les conditions sont remplies pour démarrer la vidéo.
    if (m_locked || !m_operational || musicIndex > m_musicsNumber)
    {
        m_display.displayMessage(unableToPerformError, error);
        return;
    }

    for (int i = 0; i < m_devicesNumber; i++)
    {
        if (!m_deviceList[i]->getAvailability() || m_deviceList[i]->isLocked())
        {
            m_display.displayMessage(unableToPerformError, error);
            return;
        }
    }

    // Étape 2 : préparation du terrain pour la vidéo.
    for (int i = 0; i < m_devicesNumber; i++)
    {
        m_deviceList[i]->turnOff();
        m_deviceList[i]->lock();
    }

    if (!m_state)
        turnOn();

    m_connection.playVideo(m_musicList[musicIndex]->videoURL);
    m_waitingForTriggerSound = true;
    m_currentMusicIndex = musicIndex;
    m_display.displayMessage("Initialisation...");
}

/// @brief Méthode permettant de stopper la lecture de la vidéo en cours proprement.
void Television::stopMusic()
{
    if (m_locked || !m_operational || m_currentMusicIndex == -1)
        return;

    m_currentMusicIndex = -1;
    m_lastActionIndex = 0;
    m_musicStartTime = 0;
}

void Television::moveDisplayServo(int angle)
{
    int pulseWidth = map(angle, 0, 180, 600, 2400);
    digitalWrite(m_servomotorPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(m_servomotorPin, LOW);
    delay(2);
}

void Television::switchDisplay()
{
    for (int pos = 80; pos <= 130; pos++)
    {
        moveDisplayServo(pos);
        delay(1);
    }

    for (int i = 0; i < 100; i++)
        moveDisplayServo(130);

    for (int pos = 130; pos >= 80; pos--)
    {
        moveDisplayServo(pos);
        delay(1);
    }
}

void Television::detectTriggerSound()
{
    const int SAMPLES = 128;
    const double SAMPLING_FREQUENCY = 5000.0;

    unsigned int samplingPeriodUs = round(1000000 * (1.0 / SAMPLING_FREQUENCY));
    unsigned long microSeconds;

    double vReal[SAMPLES];
    double vImag[SAMPLES];

    ArduinoFFT<double> FFT = ArduinoFFT<double>();

    for (int i = 0; i < SAMPLES; i++)
    {
        microSeconds = micros();
        vReal[i] = m_microphone->getValue() - 287;
        vImag[i] = 0;
        while (micros() < (microSeconds + samplingPeriodUs))
        {
        }
    }

    FFT.windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
    FFT.compute(vReal, vImag, SAMPLES, FFT_FORWARD);
    FFT.complexToMagnitude(vReal, vImag, SAMPLES);

    double peakFrequency = FFT.majorPeak(vReal, SAMPLES, SAMPLING_FREQUENCY);

    if (abs(peakFrequency - 1000.0) < 50.0)
    {
        m_waitingForTriggerSound = false;
        m_musicStartTime = millis() - 1100;
    }
}

void Television::scheduleMusic()
{
    while (m_musicList[m_currentMusicIndex]->actionList[m_lastActionIndex].timecode <= (millis() - m_musicStartTime))
    {
        String action = m_musicList[m_currentMusicIndex]->actionList[m_lastActionIndex].action;

        // Récupération du périphérique de sortie à partir de son ID.
        Output *output = this->getDeviceFromID(this->getIntFromString(action, 0, 2));

        if (output == nullptr)
            continue;

        output->unLock();

        switch (getIntFromString(action, 2, 2))
        {
        // Gestion de l'alimentation.
        case 0:
        {
            switch (getIntFromString(action, 4, 1))
            {
            case 0:
                output->turnOff();
                break;

            case 1:
                output->turnOn();
                break;

            case 2:
                output->toggle();
                break;
            }

            break;
        }

        // Gestion des rubans de DEL.
        case 1:
        {
            RGBLEDStrip *strip = static_cast<RGBLEDStrip *>(output);
            strip->setMode(&m_mode);
            strip->turnOn();

            switch (getIntFromString(action, 4, 1))
            {
            case 0:
                m_mode.singleColor(this->getIntFromString(action, 5, 3), this->getIntFromString(action, 8, 3), this->getIntFromString(action, 11, 3));
                break;

            case 1:
            {
                MusicsAnimationsEasing type;

                switch (this->getIntFromString(action, 28, 1))
                {
                case 0:
                    type = LINEAR;
                    break;

                case 1:
                    type = IN_CUBIC;
                    break;

                case 2:
                    type = OUT_CUBIC;
                    break;

                case 3:
                    type = IN_OUT_CUBIC;
                    break;
                }

                m_mode.smoothTransition(this->getIntFromString(action, 5, 3), this->getIntFromString(action, 8, 3), this->getIntFromString(action, 11, 3), this->getIntFromString(action, 14, 3), this->getIntFromString(action, 17, 3), this->getIntFromString(action, 20, 3), this->getIntFromString(action, 23, 5), type);

                break;
            }

            case 2:
                m_mode.strobeEffect(this->getIntFromString(action, 5, 3), this->getIntFromString(action, 8, 3), this->getIntFromString(action, 11, 3), this->getIntFromString(action, 14, 4));
                break;
            }

            break;
        }

        case 2:
        {
            ConnectedTemperatureVariableLight *light = static_cast<ConnectedTemperatureVariableLight *>(output);

            // Implémenter le contrôle de la lumière.

            break;
        }

        case 3:
        {
            ConnectedColorVariableLight *light = static_cast<ConnectedColorVariableLight *>(output);

            // Implémenter le contrôle de la lumière.

            break;
        }
        }

        output->lock();

        m_lastActionIndex++;

        if (m_lastActionIndex >= m_musicList[m_currentMusicIndex]->actionsNumber)
        {
            stopMusic();
            break;
        }
    }
}

Output *Television::getDeviceFromID(int ID)
{
    for (int i = 0; i < m_devicesNumber; i++)
    {
        if (m_deviceList[i]->getID() == ID)
            return m_deviceList[i];
    }

    return nullptr;
}

String Television::addZeros(int number, int length)
{
    String result = String(number);
    while (result.length() < (unsigned int)length)
        result = "0" + result;

    return result;
}

int Television::getIntFromString(String &string, int position, int lenght)
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