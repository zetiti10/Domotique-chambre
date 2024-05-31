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
    friend class MusicsAnimationsMode;
};

// Classe représentant un mode pour un ruban de DEL.
class RGBLEDStripMode
{
public:
    RGBLEDStripMode(const __FlashStringHelper* friendlyName, int ID, RGBLEDStrip &strip);
    virtual const __FlashStringHelper* getFriendlyName() const;
    virtual bool isActivated() const;
    virtual int getID() const;
    virtual void shutdown();

protected:
    virtual void activate();
    virtual void desactivate();
    virtual void loop() = 0;
    const __FlashStringHelper* m_friendlyName;
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
    ColorMode(const __FlashStringHelper* friendlyName, int ID, RGBLEDStrip &strip, HomeAssistant &connection);
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
    AlarmMode(const __FlashStringHelper* friendlyName, int ID, RGBLEDStrip &strip);

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
    RainbowMode(const __FlashStringHelper* friendlyName, int ID, RGBLEDStrip &strip, int EEPROMSpeed);
    virtual void setAnimationSpeed(int speed);
    virtual int getAnimationSpeed();

protected:
    unsigned long m_lastTime;
    unsigned long m_lastSave;
    bool m_speedToSave;
    int m_step;
    int m_increment;
    int m_delay;
    int m_speed;
    const int m_EEPROMSpeed;

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
    SoundreactMode(const __FlashStringHelper* friendlyName, int ID, RGBLEDStrip &strip, AnalogInput &microphone, int EEPROMSensitivity);
    virtual void setSensitivity(int sensitivity);
    virtual int getSensitivity();

protected:
    AnalogInput &m_microphone;
    unsigned long m_lastSave;
    bool m_sensitivityToSave;
    int m_sensitivity;
    unsigned long m_lastColorChange;
    unsigned long m_lastTime;
    int m_maxSound;
    const int m_EEPROMSensitivity;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

enum MusicsAnimationsCurrentEffect {
    SINGLE_COLOR,
    SMOOTH_TRANSITION,
    STROBE_EFFECT,
};

enum MusicsAnimationsEasing {
    LINEAR,
    IN_CUBIC,
    OUT_CUBIC,
    IN_OUT_CUBIC,
};

// Classe du mode de couleur utilisé pour le mode de vidéo animée.
class MusicsAnimationsMode : public RGBLEDStripMode
{
public:
    MusicsAnimationsMode(const __FlashStringHelper* friendlyName, int ID, RGBLEDStrip &strip);
    virtual void singleColor(int r, int g, int b);
    virtual void smoothTransition(int initialR, int initialG, int initialB, int finalR, int finalG, int finalB, unsigned long duration, MusicsAnimationsEasing type = LINEAR);
    virtual void strobeEffect(int r, int g, int b, int speed);

protected:
    MusicsAnimationsCurrentEffect m_currentEffect;
    int m_smoothTransitionInitialR;
    int m_smoothTransitionInitialG;
    int m_smoothTransitionInitialB;
    int m_smoothTransitionFinalR;
    int m_smoothTransitionFinalG;
    int m_smoothTransitionFinalB;
    unsigned long m_smoothTransitionInitialMillis;
    unsigned long m_smoothTransitionDuration;
    MusicsAnimationsEasing m_smoothTransitionType;
    int m_strobeEffectR;
    int m_strobeEffectG;
    int m_strobeEffectB;
    int m_strobeEffectSpeed;
    bool m_strobeEffectStep;
    unsigned long m_strobeEffectLastMillis;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

#endif