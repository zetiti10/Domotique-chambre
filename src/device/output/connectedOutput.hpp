#ifndef CONNECTED_LIGHT_DEFINITIONS
#define CONNECTED_LIGHT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"

/// @brief Classe représentant un périphérique contrôlé depuis le réseau.
class ConnectedOutput : public Output
{
public:
    ConnectedOutput(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;

protected:
    virtual void updateOn(bool shareInformation = false);
    virtual void updateOff(bool shareInformation = false);

    virtual void setAvailable();
    virtual void setUnavailable();

    friend class HomeAssistant;
};

/// @brief Classe représentant une lampe à température de couleur variable contrôlée depuis le réseau.
class ConnectedTemperatureVariableLight : public ConnectedOutput
{
public:
    ConnectedTemperatureVariableLight(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int minimalColorTemperature, unsigned int maximalColorTemperature);
    virtual void setColorTemperature(unsigned int temperature, bool shareInformation = false);
    virtual void setLuminosity(unsigned int luminosity, bool shareInformation = false);
    virtual unsigned int getColorTemperature() const;
    virtual unsigned int getLuminosity() const;

protected:
    virtual void updateColorTemperature(unsigned int temperature, bool shareInformation = false);
    virtual void updateLuminosity(unsigned int luminosity, bool shareInformation = false);

    unsigned int m_minimalColorTemperature;
    unsigned int m_maximalColorTemperature;
    unsigned int m_colorTemperature;
    unsigned int m_luminosity;

    friend class HomeAssistant;
};

/// @brief Classe représentant une lampe à température de couleur variable contrôlée depuis le réseau.
class ConnectedColorVariableLight : public ConnectedTemperatureVariableLight
{
public:
    ConnectedColorVariableLight(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int minimalColorTemperature, unsigned int maximalColorTemperature);
    virtual void setColor(unsigned int r, unsigned int g, unsigned int b, bool shareInformation = false);
    virtual void setColorTemperature(unsigned int temperature, bool shareInformation = false) override;
    virtual void setLuminosity(unsigned int luminosity, bool shareInformation = false) override;
    virtual unsigned int getRLuminosity() const;
    virtual unsigned int getGLuminosity() const;
    virtual unsigned int getBLuminosity() const;

protected:
    virtual void updateColor(unsigned int r, unsigned int g, unsigned int b, bool shareInformation = false);

    unsigned int m_RColor;
    unsigned int m_GColor;
    unsigned int m_BColor;

    friend class HomeAssistant;
};

#endif