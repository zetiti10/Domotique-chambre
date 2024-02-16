#ifndef BUZZER_DEFINITIONS
#define BUZZER_DEFINITIONS

// Autres fichiers du programme.
#include "../device.hpp"

// Classe intégrant les méthodes nécessaires pour émettre des sons avec le buzzer.
class Buzzer : public Device
{
public:
    Buzzer(String friendlyName, int ID, int pin);
    virtual void setup() override;
    virtual void clickSound();
    virtual void yesSound();
    virtual void noSound();
    virtual void doorbellMusic();

protected:
    const int m_pin;
};

#endif