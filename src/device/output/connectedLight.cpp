/**
 * @file device/output/connectedLight.cpp
 * @author Louis L
 * @brief Objet gérant une lampe connectée connectée en réseau.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "connectedLight.hpp"

ConnectedLight::ConnectedLight(String friendlyName, int ID, Display &display) : Output(friendlyName, ID, display) {}

void ConnectedLight::setup()
{
}

void ConnectedLight::turnOn(bool shareInformation)
{
}

void ConnectedLight::turnOff(bool shareInformation)
{
}

void ConnectedLight::updateOn(bool shareInformation)
{
}

void ConnectedLight::updateOff(bool shareInformation)
{
}