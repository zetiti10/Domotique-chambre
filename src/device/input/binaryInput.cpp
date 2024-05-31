/**
 * @file device/input/binaryInput.cpp
 * @author Louis L
 * @brief Classe représentant un capteur dont l'état est binaire.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "binaryInput.hpp"
#include "device/input/input.hpp"
#include "device/output/binaryOutput.hpp"
#include "device/output/alarm.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/interface/buzzer.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param ID L'identifiant unique du périphérique utilisé pour communiquer avec Home Assistant.
/// @param connection L'instance utilisée pour la communication avec Home Assistant.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
BinaryInput::BinaryInput(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup) : Input(friendlyName, ID, connection), m_state(false), m_pin(pin), m_reverted(revert), m_pullup(pullup) {}

/// @brief Initialise l'objet.
void BinaryInput::setup()
{
    if (m_operational)
        return;

    Input::setup();

    if (m_pullup)
        pinMode(m_pin, INPUT_PULLUP);

    else
        pinMode(m_pin, INPUT);

    m_operational = true;
    m_connection.updateDeviceAvailability(m_ID, true);
}

/// @brief Envoie l'état du périphérique à Home Assistant pour initialiser son état dans l'interface.
void BinaryInput::reportState()
{
    if (!m_operational)
        return;

    m_connection.updateBinaryInput(m_ID, m_state);
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void BinaryInput::loop()
{
    bool previousState = m_state;

    this->getState();

    if (previousState != m_state)
        m_connection.updateBinaryInput(m_ID, m_state);
}

/// @brief Méthode permettant de lire l'état de l'entrée, en mettant à jour son état. Une protection anti-erreur est intégrée : plusieurs vérifications sont effectuées si un état différent est détecté.
/// @return L'état actuel de l'entrée.
bool BinaryInput::getState()
{
    if ((!m_reverted && (digitalRead(m_pin) == m_state)) || (m_reverted && (!digitalRead(m_pin) == m_state)))
        return m_state;

    bool previous_state = m_state;
    int counter = 0;

    while (1)
    {
        bool new_state;

        if (m_reverted)
            new_state = !digitalRead(m_pin);

        else
            new_state = digitalRead(m_pin);

        if (new_state == previous_state)
        {
            if (counter >= 3)
            {
                m_state = new_state;

                return m_state;
            }

            else
                counter++;
        }

        else
        {
            previous_state = new_state;

            counter = 0;
        }

        delay(1);
    }
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
/// @param output L'armoire à contrôler
WardrobeDoorSensor::WardrobeDoorSensor(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup, BinaryOutput &output) : BinaryInput(friendlyName, ID, connection, pin, revert, pullup), m_output(output), m_activated(true) {}

/// @brief Initialise l'objet.
void WardrobeDoorSensor::setup()
{
    BinaryInput::setup();

    m_output.setup();
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void WardrobeDoorSensor::loop()
{
    bool previousState = m_state;

    BinaryInput::loop();

    if ((previousState != m_state) && m_activated && m_output.getAvailability())
    {
        if (m_state)
            m_output.turnOn(true);

        else
            m_output.turnOff(true);
    }
}

/// @brief Active la gestion automatique de l'armoire.
void WardrobeDoorSensor::activate()
{
    m_activated = true;
}

/// @brief Désactive la gestion automatique de l'armoire.
void WardrobeDoorSensor::desactivate()
{
    m_activated = false;
}

void WardrobeDoorSensor::toggleActivation()
{
    if (m_activated)
        this->desactivate();

    else
        this->activate();
}

/// @brief Méthode permettant de savoir si le mode automatique est activé ou non.
/// @return L'état du mode automatique.
bool WardrobeDoorSensor::getActivation() const
{
    return m_activated;
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
/// @param alarm L'alarme liée au capteur.
DoorSensor::DoorSensor(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup, Alarm &alarm) : BinaryInput(friendlyName, ID, connection, pin, revert, pullup), m_alarm(alarm) {}

/// @brief Initialise l'objet.
void DoorSensor::setup()
{
    BinaryInput::setup();
    m_alarm.setup();
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void DoorSensor::loop()
{
    BinaryInput::loop();

    if (m_state && m_alarm.getAvailability() && m_alarm.getState())
        m_alarm.trigger();
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
/// @param display L'écran utilisé pour l'animation.
/// @param buzzer Le buzzer à faire sonner.
Doorbell::Doorbell(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, unsigned int pin, bool revert, bool pullup, Display &display, Buzzer &buzzer) : BinaryInput(friendlyName, ID, connection, pin, revert, pullup), m_display(display), m_buzzer(buzzer), m_lastTime(0) {}

/// @brief Initialise l'objet.
void Doorbell::setup()
{
    BinaryInput::setup();
    m_display.setup();
    m_buzzer.setup();
    m_lastTime = millis();
}

/// @brief Méthode d'exécution des tâches liées au capteur. Envoi à Home Assistant de l'état lorque la sonnette est déclenchée.
void Doorbell::loop()
{
    this->getState();

    if (m_state && ((millis() - m_lastTime) >= 10000))
    {
        m_connection.updateBinaryInput(m_ID, true);

        m_display.displayBell();
        m_buzzer.doorbellMusic();

        m_connection.updateBinaryInput(m_ID, false);

        m_lastTime = millis();
    }
}