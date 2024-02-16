/**
 * @file device/input/binaryInput.cpp
 * @author Louis L
 * @brief Classe représentant un capteur dont l'état est binaire.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "binaryInput.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
BinaryInput::BinaryInput(String friendlyName, int ID, int pin, bool revert, bool pullup) : Input(friendlyName, ID), m_state(false), m_pin(pin), m_reverted(revert), m_pullup(pullup) {}

/// @brief Initialise l'objet.
void BinaryInput::setup()
{
    if (m_operational)
        return;

    if (m_pullup)
        pinMode(m_pin, INPUT_PULLUP);

    else
        pinMode(m_pin, INPUT);

    m_operational = true;
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void BinaryInput::loop()
{
    getState();
}

// Retourne l'état actuel du capteur.
bool BinaryInput::getState()
{
    if (m_reverted)
        m_state = !digitalRead(m_pin);

    else
        m_state = digitalRead(m_pin);

    return m_state;
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
/// @param output L'armoire à contrôler
WardrobeDoorSensor::WardrobeDoorSensor(String friendlyName, int ID, int pin, bool revert, bool pullup, BinaryOutput &output) : BinaryInput(friendlyName, pin, revert, pullup), m_output(output), m_activated(true) {}

/// @brief Initialise l'objet.
void WardrobeDoorSensor::setup()
{
    BinaryInput::setup();

    m_output.setup();
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void WardrobeDoorSensor::loop()
{
    if (m_activated && m_output.getAvailability() && (getState() != m_output.getState()))
    {
        if (getState())
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
DoorSensor::DoorSensor(String friendlyName, int ID, int pin, bool revert, bool pullup, Alarm &alarm) : BinaryInput(friendlyName, pin, revert, pullup), m_alarm(alarm) {}

/// @brief Initialise l'objet.
void DoorSensor::setup()
{
    BinaryInput::setup();

    m_alarm.setup();
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void DoorSensor::loop()
{
    if (getState() && m_alarm.getAvailability() && m_alarm.getState())
        m_alarm.trigger();
}

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
/// @param pin La broche liée au capteur.
/// @param revert Inversionou non de l'état du capteur.
/// @param pullup Activation ou non du mode `PULLUP`.
/// @param display L'écran utilisé pour l'animation.
/// @param buzzer Le buzzer à faire sonner.
Doorbell::Doorbell(String friendlyName, int ID, int pin, bool revert, bool pullup, Display &display, Buzzer &buzzer) : BinaryInput(friendlyName, pin, revert, pullup), m_display(display), m_buzzer(buzzer), m_lastTime(0) {}

/// @brief Initialise l'objet.
void Doorbell::setup()
{
    BinaryInput::setup();

    m_display.setup();
    m_buzzer.setup();

    m_lastTime = millis();
}

/// @brief Méthode d'exécution des tâches liées au capteur.
void Doorbell::loop()
{
    if (getState() && ((millis() - m_lastTime) >= 10000))
    {
        m_display.displayBell();
        m_buzzer.doorbellMusic();

        m_lastTime = millis();
    }
}