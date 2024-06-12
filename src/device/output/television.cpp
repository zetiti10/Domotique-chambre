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
#include "television.hpp"
#include "utils/readPROGMEMString.hpp"
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "EEPROM.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param display L'écran à utiliser pour afficher des informations / animations.
/// @param servomotorPin La broche associée à celle du servomoteur.
/// @param IRLEDPin La broche associée à celle de la DEL infrarouge.
/// @param volume Le volume récupéré de l'EEPROM.
/// @param mode Le mode utilisé lors de vidéos animées pour contrôler le ruban de DEL RVB.
Television::Television(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, int servomotorPin, int IRLEDPin, int volume, MusicsAnimationsMode &mode) : Output(friendlyName, ID, connection, display), m_servomotorPin(servomotorPin), m_IRLEDPin(IRLEDPin), m_IRSender(), m_volume(volume), m_volumeMuted(false), m_lastTime(0), m_waitingForTriggerSound(false), m_microphone(nullptr), m_musicStartTime(0), m_lastActionIndex(0), m_musicList(nullptr), m_currentMusicIndex(-1), m_musicsNumber(0), m_deviceList(nullptr), m_devicesNumber(0), m_mode(mode), m_detectionCounter(0) {}

/// @brief Cette méthode permet d'enregistrer les périphériques du système qui pourront être contrôlés automatiquement lors de la lecture d'une vidéo.
/// @param deviceList La liste de périphériques à utiliser.
/// @param devicesNumber Le nombre de périphériques de la liste `deviceList`.
void Television::setMusicDevices(Output *deviceList[], unsigned int devicesNumber)
{
    m_deviceList = deviceList;
    m_devicesNumber = devicesNumber;
}

/// @brief Méthode permettant de définir la liste des musiques disponibles à la lecture.
/// @param musicList La liste de musiques.
/// @param musicsNumber Le nombre de musiques de la liste de musiques.
void Television::setMusicsList(const Music *const *musicList, unsigned int musicsNumber)
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
    {
        if (detectTriggerSound())
        {
            m_waitingForTriggerSound = false;
            m_musicStartTime = millis() - 1250;
            m_display.displayMessage("C'est parti !");
        }

        m_detectionCounter++;

        if (m_detectionCounter >= 1000)
        {
            m_waitingForTriggerSound = false;
            m_detectionCounter = 0;
        }
    }

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
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    unsigned int savedVolume = m_volume;

    if (shareInformation)
        m_display.displayMessage("Calibration du son...");

    for (int i = 0; i < 25; i++)
    {
        for (int i = 0; i < 2; i++)
            m_IRSender.sendNEC(0x44C1, 0xC7, 3);

        delay(50);
    }

    for (unsigned int i = 0; i < savedVolume; i++)
    {
        for (int i = 0; i < 2; i++)
            m_IRSender.sendNEC(0x44C1, 0x47, 3);

        delay(50);
    }

    if (shareInformation)
        m_display.displayMessage("Calibration terminée !");
}

/// @brief Augmente le volume de la télévision.
/// @param shareInformation Affiche ou non l'animation sur l'écran.
void Television::increaseVolume(bool shareInformation)
{
    if (!m_state || m_locked || m_volumeMuted || !m_operational || (m_volume == 25))
    {
        if (shareInformation)
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

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
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

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
unsigned int Television::getVolume() const
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
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

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
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

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
        this->unMute();

    else
        this->mute();
}

/// @brief Méthode permettant de récupérer l'état actuel du mode sourdinne de la télévision.
/// @return L'état actuel du mode sourdinne de la télévision.
bool Television::getMute() const
{
    return m_volumeMuted;
}

/// @brief Méthode permettant d'obtenir la liste des musiques disponibles.
/// @return La liste des musiques disponibles.
const Music *const *Television::getMusicsList() const
{
    return m_musicList;
}

/// @brief Méthode permettant d'obtenir le nombre de musiques enregistré.
/// @return Le nombre de musiques enregistré.
unsigned int Television::getMusicNumber() const
{
    return m_musicsNumber;
}

Music Television::getMusicFromIndex(unsigned int index)
{
    if (index >= m_musicsNumber)
        return Music();

    const Music *musicPtr;
    memcpy_P(&musicPtr, &m_musicList[index], sizeof(Music *));
    Music music;
    memcpy_P(&music, musicPtr, sizeof(Music));
    return music;
}

/// @brief Méthode permettant de démarrer la lecture d'une vidéo.
/// @param musicIndex La position de la musique dans la liste des musiques disponibles fournie par la méthode `Music **Television::getMusicsList()`.
void Television::playMusic(unsigned int musicIndex)
{
    // Étape 1 : vérification que toutes les conditions sont remplies pour démarrer la vidéo.
    if (m_locked || !m_operational || musicIndex > m_musicsNumber)
    {
        m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

        return;
    }

    for (unsigned int i = 0; i < m_devicesNumber; i++)
    {
        m_deviceList[i]->setup();

        if (!m_deviceList[i]->getAvailability() || m_deviceList[i]->isLocked())
        {
            m_display.displayMessage("Impossible d'effectuer cette action.", "Erreur");

            return;
        }
    }

    m_display.displayMessage("Initialisation...");

    // Étape 2 : préparation du terrain pour la vidéo.
    for (unsigned int i = 0; i < m_devicesNumber; i++)
    {
        m_deviceList[i]->turnOff();
        m_deviceList[i]->lock();
    }

    if (!m_state)
    {
        this->turnOn();
        delay(1000);
    }

    if (m_volumeMuted)
    {
        this->unMute();
        delay(1000);
    }

    delay(1000);

    while (this->getVolume() < 18)
    {
        this->increaseVolume();
        delay(100);
    }

    delay(2000);

    m_connection.playVideo(readProgmemString(getMusicFromIndex(musicIndex).videoURL));
    m_waitingForTriggerSound = true;
    m_currentMusicIndex = musicIndex;
    m_display.displayMessage("En attente de la vidéo.");
}

/// @brief Méthode permettant de stopper la lecture de la vidéo en cours proprement.
void Television::stopMusic()
{
    if (m_locked || !m_operational || m_currentMusicIndex == -1)
        return;

    m_currentMusicIndex = -1;
    m_lastActionIndex = 0;
    m_musicStartTime = 0;

    for (unsigned int i = 0; i < m_devicesNumber; i++)
    {
        m_deviceList[i]->unLock();
        m_deviceList[i]->turnOff();
    }
}

/// @brief Méthode arrêtant le périphérique avant l'arrêt du système.
void Television::shutdown()
{
    EEPROM.update(EEPROM_VOLUME, m_volume);

    Output::shutdown();
}

/// @brief Envoie une impulsion pour contrôler le servomoteur.
/// @param angle L'angle sélectionné en degré.
void Television::moveDisplayServo(unsigned int angle)
{
    int pulseWidth = map(angle, 0, 180, 600, 2400);
    digitalWrite(m_servomotorPin, HIGH);
    delayMicroseconds(pulseWidth);
    digitalWrite(m_servomotorPin, LOW);
    delay(2);
}

/// @brief Effectue un clic sur le bouton de l'écran à l'aide d'un servomoteur.
void Television::switchDisplay()
{
    for (int pos = 80; pos <= 130; pos++)
    {
        this->moveDisplayServo(pos);
        delay(1);
    }

    for (int i = 0; i < 100; i++)
        this->moveDisplayServo(130);

    for (int pos = 130; pos >= 80; pos--)
    {
        this->moveDisplayServo(pos);
        delay(1);
    }
}

/// @brief Méthode permettant de détecter un son à une fréquence particulière.
/// @return Le résultat de l'analyse.
bool Television::detectTriggerSound()
{
    unsigned long startTime = millis();
    unsigned long previousPeakTime = millis();
    int previousValue = 0;
    int maxValue = 0;
    int counter = 0;
    while ((millis() - startTime) <= 500)
    {
        int currentValue = m_microphone->getValue() - 287;

        if (currentValue > maxValue)
            maxValue = currentValue;

        if (currentValue < previousValue)
        {
            if (previousValue >= (maxValue * 0.75))
            {
                unsigned long frequency = uint64_t(1.0 / double(double(millis() - previousPeakTime) / 1000.0));

                if (abs(int(frequency) - 1000) < 50)
                {
                    if (counter == 5)
                        return true;

                    counter++;
                }

                previousPeakTime = millis();
            }
        }

        previousValue = currentValue;
    }

    return false;
}

/// @brief Méthode d'exécution des tâches périodiques liées au mode musique animée.
void Television::scheduleMusic()
{
    Music currentMusic = getMusicFromIndex(m_currentMusicIndex);
    Action currentAction = getAction(currentMusic, m_lastActionIndex);

    while (currentAction.timecode <= (millis() - m_musicStartTime))
    {
        String action = currentAction.action;

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

            switch (getIntFromString(action, 4, 1))
            {
            case 0:
                light->setColorTemperature(getIntFromString(action, 5, 4));
                break;

            case 1:
                light->setLuminosity(getIntFromString(action, 5, 3));
                break;
            }

            break;
        }

        case 3:
        {
            ConnectedColorVariableLight *light = static_cast<ConnectedColorVariableLight *>(output);

            switch (getIntFromString(action, 4, 1))
            {
            case 0:
                light->setColor(getIntFromString(action, 5, 3), getIntFromString(action, 8, 3), getIntFromString(action, 11, 3));
                break;

            case 1:
                light->setColorTemperature(getIntFromString(action, 5, 4));
                break;

            case 2:
                light->setLuminosity(getIntFromString(action, 5, 3));
                break;
            }

            break;
        }
        }

        output->lock();

        m_lastActionIndex++;

        if (m_lastActionIndex >= currentMusic.actionsNumber)
        {
            stopMusic();
            break;
        }

        currentAction = getAction(currentMusic, m_lastActionIndex);
    }
}

/// @brief Méthode permettant d'obtenir un objet à partir de son identifiant unique.
/// @param ID L'identifiant unique du périphérique cherché.
/// @return Un pointeur vers l'objet cherche (renvoie `nullptr` s'il na pas été trouvé).
Output *Television::getDeviceFromID(unsigned int ID)
{
    for (unsigned int i = 0; i < m_devicesNumber; i++)
    {
        if (m_deviceList[i]->getID() == ID)
            return m_deviceList[i];
    }

    return nullptr;
}

Action Television::getAction(Music music, unsigned int actionIndex)
{
    if (actionIndex >= music.actionsNumber)
        return Action();

    Action action;
    memcpy_P(&action, &music.actionList[actionIndex], sizeof(Action));
    return action;
}

/// @brief Méthode permettant de convertir un entier en un `String` complété de zéros pour avoir une longueur fixée.
/// @param number Le nombre à convertir.
/// @param length La longueur de la chaîne de caractères voulue. Le `String` sera complété de zéros si nécessaire.
/// @return Le `String` demandé.
String Television::addZeros(unsigned int number, unsigned int length)
{
    String result = String(number);
    while (result.length() < (unsigned int)length)
        result = "0" + result;

    return result;
}

/// @brief Méthode permettant d'extraire un entier à partir d'un `String`.
/// @param string La chaîne de caractères en entrée.
/// @param position La position de l'entier dans la chaîne de caractères.
/// @param lenght La longueur de l'entier.
/// @return L'entier voulu.
unsigned int Television::getIntFromString(String &string, unsigned int position, unsigned int lenght)
{
    int result = 0;

    for (unsigned int i = 0; i < lenght; i++)
    {
        int power = 1;

        for (unsigned int j = 0; j < ((lenght - i) - 1); j++)
            power *= 10;

        result += (string.charAt(position + i) - '0') * power;
    }

    return result;
}