#ifndef RGB_LED_STRIP_DEFINITIONS
#define RGB_LED_STRIP_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "output.hpp"

class RGBLEDStripMode;

class RGBLEDStrip : public Output
{
public:
    RGBLEDStrip(String friendlyName, Display &display, int RPin, int GPin, int BPin);
    virtual void setup() override;
    virtual void turnOn(boolean shareInformation = false) override;
    virtual void turnOff(boolean shareInformation = false) override;
    virtual void loop();
    virtual void setMode(RGBLEDStripMode &mode, boolean shareInformation = false);
    virtual RGBLEDStripMode &getMode() const;
    virtual int getR() const;
    virtual int getG() const;
    virtual int getB() const;

protected:
    int m_RPin;
    int m_GPin;
    int m_BPin;
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
};

class RGBLEDStripMode
{
public:
    RGBLEDStripMode(String friendlyName, RGBLEDStrip &strip);
    virtual String getFriendlyName() const;
    virtual boolean isActivated() const;

protected:
    virtual void activate();
    virtual void desactivate();
    virtual void loop() = 0;
    String m_friendlyName;
    RGBLEDStrip &m_strip;
    boolean m_activated;

private:
    friend class RGBLEDStrip;
};

class ColorMode : public RGBLEDStripMode
{
public:
    ColorMode(String friendlyName, RGBLEDStrip &strip);
    virtual void setColor(int r, int g, int b);

protected:
    int m_R;
    int m_G;
    int m_B;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

class AlarmMode : public RGBLEDStripMode
{
public:
    AlarmMode(String friendlyName, RGBLEDStrip &strip);

protected:
    unsigned long m_counter;

private:
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

class RainbowMode : RGBLEDStripMode
{
public:
    RainbowMode(String friendlyName, RGBLEDStrip &strip);
    virtual void setAnimationSpeed(int speed);
    virtual int getAnimationSpeed();

protected:
    unsigned long m_counter;
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

#endif