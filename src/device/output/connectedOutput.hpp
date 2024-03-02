#ifndef CONNECTED_LIGHT_DEFINITIONS
#define CONNECTED_LIGHT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

// Classe représentant une lampe contrôlée depuis le réseau.
class ConnectedOutput : public Output
{
public:
    ConnectedOutput(String friendlyName, int ID, Display &display, HomeAssistant &connection);
    virtual void setup();
    virtual void turnOn(bool shareInformation = false);
    virtual void turnOff(bool shareInformation = false);

protected:
    virtual void updateOn(bool shareInformation = false);
    virtual void updateOff(bool shareInformation = false);
    virtual void setAvailable();
    virtual void setUnavailable();
    friend class HomeAssistant;
};

// Classe représentant une lampe à température de couleur variable contrôlée depuis le réseau.
class ConnectedTemperatureVariableLight : public ConnectedOutput
{
public:
    ConnectedTemperatureVariableLight(String friendlyName, int ID, Display &display, HomeAssistant &connection, int minimalColorTemperature, int maximalColorTemperature);
    virtual void setColorTemperature(int temperature, bool shareInformation = false);
    virtual void setLuminosity(int luminosity, bool shareInformation = false);
    virtual int getColorTemperature();
    virtual int getLuminosity();

protected:
    virtual void updateColorTemperature(int temperature, bool shareInformation = false);
    virtual void updateLuminosity(int luminosity, bool shareInformation = false);
    int m_minimalColorTemperature;
    int m_maximalColorTemperature;
    int m_colorTemperature;
    int m_luminosity;
    friend class HomeAssistant;
};

// Classe représentant une lampe à température de couleur variable contrôlée depuis le réseau.
class ConnectedColorVariableLight : public ConnectedTemperatureVariableLight
{
public:
    ConnectedColorVariableLight(String friendlyName, int ID, Display &display, HomeAssistant &connection, int minimalColorTemperature, int maximalColorTemperature);
    virtual void setColor(int r, int g, int b, bool shareInformation = false);
    virtual void setColorTemperature(int temperature, bool shareInformation = false) override;
    virtual void setLuminosity(int luminosity, bool shareInformation = false) override;
    virtual int getRLuminosity();
    virtual int getGLuminosity();
    virtual int getBLuminosity();

protected:
    virtual void updateColor(int r, int g, int b, bool shareInformation = false);
    int m_RColor;
    int m_GColor;
    int m_BColor;
    friend class HomeAssistant;
};

#endif