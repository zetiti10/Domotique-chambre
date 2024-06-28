#ifndef RGB_LED_STRIP_DEFINITIONS
#define RGB_LED_STRIP_DEFINITIONS

// Ajout des bibilothèques au programme.
#include <Arduino.h>

// Autres fichiers du programme.
#include "device/output/output.hpp"
#include "device/interface/display.hpp"
#include "device/interface/HomeAssistant.hpp"
#include "device/input/analogInput.hpp"

/// @brief Classe gérant un ruban de DEL.
class RGBLEDStrip : public Output
{
public:
    RGBLEDStrip(const __FlashStringHelper *friendlyName, unsigned int ID, HomeAssistant &connection, Display &display, unsigned int RPin, unsigned int GPin, unsigned int BPin);
    virtual void setup() override;
    virtual void reportState() override;
    virtual void turnOn(bool shareInformation = false) override;
    virtual void turnOff(bool shareInformation = false) override;
    virtual void loop() override;
    virtual void setMode(RGBLEDStripMode *mode, bool shareInformation = false);
    virtual RGBLEDStripMode *getMode() const;
    virtual unsigned int getR() const;
    virtual unsigned int getG() const;
    virtual unsigned int getB() const;

protected:
    const unsigned int m_RPin;
    const unsigned int m_GPin;
    const unsigned int m_BPin;
    unsigned int m_RState;
    unsigned int m_GState;
    unsigned int m_BState;
    RGBLEDStripMode *m_mode;

private:
    virtual void setColor(unsigned int r, unsigned int g, unsigned int b);

    friend class RGBLEDStripMode;
    friend class ColorMode;
    friend class AlarmMode;
    friend class RainbowMode;
    friend class SoundreactMode;
    friend class MusicsAnimationsMode;
};

/// @brief Classe représentant un mode pour un ruban de DEL.
class RGBLEDStripMode
{
public:
    RGBLEDStripMode(const __FlashStringHelper *friendlyName, unsigned int ID, RGBLEDStrip &strip);
    virtual const __FlashStringHelper *getFriendlyName() const;
    virtual bool isActivated() const;
    virtual unsigned int getID() const;
    virtual unsigned int getStripID() const;

protected:
    virtual void activate();
    virtual void desactivate();
    virtual void loop() = 0;

    const __FlashStringHelper *m_friendlyName;
    unsigned const int m_ID;
    RGBLEDStrip &m_strip;
    bool m_activated;

private:
    friend class RGBLEDStrip;
};

/// @brief Classe du mode couleur unique.
class ColorMode : public RGBLEDStripMode
{
public:
    ColorMode(const __FlashStringHelper *friendlyName, unsigned int ID, RGBLEDStrip &strip, HomeAssistant &connection);
    virtual void setColor(unsigned int r, unsigned int g, unsigned int b);
    virtual unsigned int getR() const;
    virtual unsigned int getG() const;
    virtual unsigned int getB() const;

protected:
    HomeAssistant &m_connection;
    unsigned int m_R;
    unsigned int m_G;
    unsigned int m_B;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;

    friend class RGBLEDStrip;
};

/// @brief Classe du mode de couleur de l'alarme qui sonne.
class AlarmMode : public RGBLEDStripMode
{
public:
    AlarmMode(const __FlashStringHelper *friendlyName, unsigned int ID, RGBLEDStrip &strip);

protected:
    unsigned long m_lastTime;

private:
    virtual void desactivate() override;
    virtual void loop() override;
    friend class RGBLEDStrip;
};

/// @brief Classe du mode de couleur arc-en-ciel.
class RainbowMode : public RGBLEDStripMode
{
public:
    RainbowMode(const __FlashStringHelper *friendlyName, unsigned int ID, RGBLEDStrip &strip, unsigned int EEPROMSpeed);
    virtual void setAnimationSpeed(unsigned int speed);
    virtual unsigned int getAnimationSpeed() const;

protected:
    unsigned long m_lastTime;
    unsigned long m_lastSave;
    bool m_speedToSave;
    unsigned int m_step;
    unsigned int m_increment;
    unsigned int m_delay;
    unsigned int m_speed;
    const unsigned int m_EEPROMSpeed;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;

    friend class RGBLEDStrip;
};

/// @brief Classe du mode de couleur son-réaction.
class SoundreactMode : public RGBLEDStripMode
{
public:
    SoundreactMode(const __FlashStringHelper *friendlyName, unsigned int ID, RGBLEDStrip &strip, AnalogInput &microphone, unsigned int EEPROMSensitivity);
    virtual void setSensitivity(unsigned int sensitivity);
    virtual unsigned int getSensitivity() const;

protected:
    AnalogInput &m_microphone;
    unsigned long m_lastSave;
    bool m_sensitivityToSave;
    unsigned int m_sensitivity;
    unsigned long m_lastColorChange;
    unsigned long m_lastTime;
    unsigned int m_maxSound;
    const unsigned int m_EEPROMSensitivity;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;

    friend class RGBLEDStrip;
};

/// @brief Structure utilisée par le mode de couleur `MusicsAnimationsMode` pour enregistrer l'animation en cours d'exécution.
enum MusicsAnimationsCurrentEffect
{
    SINGLE_COLOR,
    SMOOTH_TRANSITION,
    STROBE_EFFECT,
};

/// @brief Différents effets possibles pour la transition d'une couleur à l'autre.
enum MusicsAnimationsEasing
{
    LINEAR,
    IN_CUBIC,
    OUT_CUBIC,
    IN_OUT_CUBIC,
};

/// @brief Classe du mode de couleur utilisé pour le mode de vidéo animée.
class MusicsAnimationsMode : public RGBLEDStripMode
{
public:
    MusicsAnimationsMode(const __FlashStringHelper *friendlyName, unsigned int ID, RGBLEDStrip &strip);
    virtual void singleColor(unsigned int r, unsigned int g, unsigned int b);
    virtual void smoothTransition(unsigned int initialR, unsigned int initialG, unsigned int initialB, unsigned int finalR, unsigned int finalG, unsigned int finalB, unsigned int duration, MusicsAnimationsEasing type = LINEAR);
    virtual void strobeEffect(unsigned int r, unsigned int g, unsigned int b, unsigned int speed);

protected:
    MusicsAnimationsCurrentEffect m_currentEffect;
    unsigned int m_smoothTransitionInitialR;
    unsigned int m_smoothTransitionInitialG;
    unsigned int m_smoothTransitionInitialB;
    unsigned int m_smoothTransitionFinalR;
    unsigned int m_smoothTransitionFinalG;
    unsigned int m_smoothTransitionFinalB;
    unsigned long m_smoothTransitionInitialMillis;
    unsigned int m_smoothTransitionDuration;
    MusicsAnimationsEasing m_smoothTransitionType;
    unsigned int m_strobeEffectR;
    unsigned int m_strobeEffectG;
    unsigned int m_strobeEffectB;
    unsigned int m_strobeEffectSpeed;
    bool m_strobeEffectStep;
    unsigned long m_strobeEffectLastMillis;

private:
    virtual void activate() override;
    virtual void desactivate() override;
    virtual void loop() override;

    friend class RGBLEDStrip;
};

#endif