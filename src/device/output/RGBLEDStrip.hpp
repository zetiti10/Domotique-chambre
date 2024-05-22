#ifndef RGB_LED_STRIP_DEFINITIONS
#define RGB_LED_STRIP_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/input/analogInput.hpp"

// Classe gérant un ruban de DEL.
class RGBLEDStrip : public Output
{
public:
    RGBLEDStrip(const __FlashStringHelper* friendlyName, int ID, HomeAssistant &connection, Display &display, int RPin, int GPin, int BPin);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void loop();
    virtual void setMode(RGBLEDStripMode *mode, bool shareInformation = false);
    virtual RGBLEDStripMode *getMode() const;
    virtual int getR() const;
    virtual int getG() const;
    virtual int getB() const;

protected:
    const int m_RPin;
    const int m_GPin;
    const int m_BPin;
    int m_RState;
    int m_GState;
    int m_BState;
    RGBLEDStripMode *m_mode;

private:
    virtual void setColor(int r, int g, int b);
    friend class RGBLEDStripMode;
    friend class ColorMode;
    friend class AlarmMode;
    friend class RainbowMode;
    friend class SoundreactMode;
};

// Classe représentant un mode pour un ruban de DEL.
class RGBLEDStripMode
{
public:
    RGBLEDStripMode(String friendlyName, int ID, RGBLEDStrip &strip);
    virtual String getFriendlyName() const;
    virtual bool isActivated() const;
    virtual int getID() const;

protected:
    virtual void activate();
    virtual void desactivate();
    virtual void loop() = 0;
    String m_friendlyName;
    int m_ID;
    RGBLEDStrip &m_strip;
    bool m_activated;

private:
    friend class RGBLEDStrip;
};

// Classe du mode couleur unique.
class ColorMode : public RGBLEDStripMode
{
public:
    ColorMode(String friendlyName, int ID, RGBLEDStrip &strip, HomeAssistant &connection);
    virtual void setColor(int r, int g, int b);
    virtual int getR() const;
    virtual int getG() const;
    virtual int getB() const;

protected:
    HomeAssistant &m_connection;
    int m_R;
    int m_G;
    int m_B;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

// Classe du mode de couleur de l'alarme qui sonne.
class AlarmMode : public RGBLEDStripMode
{
public:
    AlarmMode(String friendlyName, int ID, RGBLEDStrip &strip);

protected:
    unsigned long m_lastTime;

private:
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

// Classe du mode de couleur arc-en-ciel.
class RainbowMode : public RGBLEDStripMode
{
public:
    RainbowMode(String friendlyName, int ID, RGBLEDStrip &strip, int speed);
    virtual void setAnimationSpeed(int speed);
    virtual int getAnimationSpeed();

protected:
    unsigned long m_lastTime;
    int m_step;
    int m_increment;
    int m_delay;
    int m_speed;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

// Classe du mode de couleur son-réaction.
class SoundreactMode : public RGBLEDStripMode
{
public:
    SoundreactMode(String friendlyName, int ID, RGBLEDStrip &strip, AnalogInput &microphone, int sensitivity);
    virtual void setSensitivity(int sensitivity);
    virtual int getSensitivity();

protected:
    AnalogInput &m_microphone;
    int m_sensitivity;
    unsigned long m_lastColorChange;
    unsigned long m_lastTime;
    int m_maxSound;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

#endif