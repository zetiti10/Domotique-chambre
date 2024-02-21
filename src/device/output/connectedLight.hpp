#ifndef CONNECTED_LIGHT_DEFINITIONS
#define CONNECTED_LIGHT_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

// Classe représentant une lampe contrôlée depuis le réseau.
class ConnectedLight : public Output
{
public:
    ConnectedLight(String friendlyName, int ID, Display &display);
    virtual void setup();
    virtual void turnOn(bool shareInformation = false);
    virtual void turnOff(bool shareInformation = false);

private:
    virtual void updateOn(bool shareInformation = false);
    virtual void updateOff(bool shareInformation = false);
};

// Classe représentant une lampe à température de couleur variable contrôlée depuis le réseau.
class ConnectedTemperatureVariableLight : public ConnectedLight
{
public:
    ConnectedTemperatureVariableLight(String friendlyName, int ID, Display &display);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void setColorTemperature(int temperature, bool shareInformation = false);
    virtual void setLuminosity(int luminosity, bool shareInformation = false);
    virtual int getColorTemperature();
    virtual int getLuminosity();

private:
    virtual void updateOn(bool shareInformation = false) override;
    virtual void updateOff(bool shareInformation = false) override;
    virtual void updateColorTemperature(int temperature, bool shareInformation = false);
    virtual void updateLuminosity(int luminosity, bool shareInformation = false);
    int m_colorTemperature;
    int m_luminosity;
};

// Classe représentant une lampe à température de couleur variable contrôlée depuis le réseau.
class ConnectedColorVariableLight : public ConnectedTemperatureVariableLight
{
public:
    ConnectedColorVariableLight(String friendlyName, int ID, Display &display);
    virtual void setup() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void setColor(int temperature, bool shareInformation = false);
    virtual void setColorTemperature(int temperature, bool shareInformation = false) override;
    virtual void setLuminosity(int luminosity, bool shareInformation = false) override;
    virtual int getRLuminosity();
    virtual int getGLuminosity();
    virtual int getBLuminosity();
    virtual int getColorTemperature();
    virtual int getLuminosity();


private:
    virtual void updateOn(bool shareInformation = false) override;
    virtual void updateOff(bool shareInformation = false) override;
    virtual void updateColor(int r, int g, int b, boolean shareInformation = false);
    virtual void updateColorTemperature(int temperature, bool shareInformation = false);
    virtual void updateLuminosity(int luminosity, bool shareInformation = false);
    int m_color;
};

#endif