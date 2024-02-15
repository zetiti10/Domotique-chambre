/**
 * @file device/interface/display.cpp
 * @author Louis L
 * @brief Classe représentant l'écran du système de domotique.
 * @version 2.0 dev
 * @date 2024-01-20
 */

// Autres fichiers du programme.
#include "display.hpp"
#include "../../bitmaps.hpp"

/// @brief Constructeur de la classe.
/// @param friendlyName Le nom formaté pour être présenté à l'utilisateur du périphérique.
Display::Display(String friendlyName) : Device(friendlyName), m_display(128, 64, &Wire, -1), m_lastTime(0) {}

/// @brief Initialise l'objet.
void Display::setup()
{
    if (m_operational)
        return;

    if (m_display.begin(SSD1306_SWITCHCAPVCC, 0x3c))
    {
        m_display.clearDisplay();
        m_display.cp437(true);
        m_display.setTextColor(WHITE);

        m_operational = true;
    }
}

/// @brief Affiche la liste des périphériques indisponibles.
/// @param deviceList Un pointeur vers la liste à vérifier.
/// @param devicesNumber Le nombre d'éléments de la liste.
void Display::displayUnavailableDevices(Device *deviceList[], int &devicesNumber)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.setTextSize(1);
    m_display.setTextColor(WHITE);
    m_display.setCursor(0, 0);

    // Vérification pour chaque périphérique de la liste s'il est indisponible.
    int counter = 0;

    for (int i = 0; i < devicesNumber; i++)
    {
        if (!deviceList[i]->getAvailability())
        {
            m_display.setCursor(0, counter * 10 + 18);
            m_display.println(deviceList[i]->getFriendlyName());
            counter++;
        }
    }

    // Si aucune erreur n'a été détectée, affichage d'un message.
    if (counter == 0)
    {
        displayMessage("Initialisation terminee sans erreur.");
    }

    // Affichage de la liste des périphériques indisponibles.
    else
    {
        m_display.setCursor(0, 0);
        m_display.setTextSize(2);
        m_display.print("Erreur(s)");

        m_display.display();

        m_lastTime = millis();
    }
}

/// @brief Affiche un pictogramme de cloche à l'écran.
void Display::displayBell()
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, bellBitmap, 128, 64, WHITE);
    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche un message à l'écran avec un gros titre.
/// @param message Le message à afficher.
/// @param title Le titre du message (par défaut `INFO`).
void Display::displayMessage(String message, String title)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();

    m_display.setCursor(0, 0);
    m_display.setTextSize(2);
    m_display.print(title);

    m_display.setCursor(0, 18);
    m_display.setTextSize(1);
    m_display.print(message);

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche le volume actuel.
/// @param action Le type de message à afficher (modifie le picrogramme ainsi que les informations affichées ; par défaut, le volume actuel).
/// @param volume Le volume actuel (par défaut `0`, pour les actions qui n'affichent pas le volume actuel).
void Display::displayVolume(VolumeType action, int volume)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();

    // Affichage d'un pictogramme en fonction de l'action sélectionnée.
    if (action == DECREASE)
        m_display.drawBitmap(0, 0, volumeDecreaseBitmap, 128, 64, WHITE);

    else if (action == INCREASE)
        m_display.drawBitmap(0, 0, volumeIncreaseBitmap, 128, 64, WHITE);

    else if (action == MUTE)
        m_display.drawBitmap(0, 0, muteBitmap, 128, 64, WHITE);

    else if (action == UNMUTE)
        m_display.drawBitmap(0, 0, unmuteBitmap, 128, 64, WHITE);

    // Affichage du volume selon le mode sélectionné.
    if (action == DECREASE || action == INCREASE || action == UNMUTE)
    {
        m_display.drawRect(50, 52, 27, 3, WHITE);

        if (volume > 0)
            m_display.drawLine(51, 53, 51 + volume, 53, WHITE);

        m_display.setTextSize(1);
        m_display.setCursor(80, 52);
        m_display.print(volume);
    }

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche un pictogramme d'alerte (point d'exclamation).
/// @param colorsInverted Inversion ou non des couleurs de l'écran.
void Display::displayAlarmTriggered(bool colorsInverted)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, alarmTriggeredBitmap, 128, 64, 1);

    if (!colorsInverted)
        m_display.invertDisplay(false);

    else
        m_display.invertDisplay(true);

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche les valeurs des capteurs de température et d'humidité, avec deux pictogrammes.
/// @param temperature La valeur de la température à afficher.
/// @param humidity La valeur de l'humidité à afficher.
void Display::displayAirValues(float temperature, float humidity)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, airBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);

    m_display.setCursor(40, 42);
    m_display.print(temperature);
    // Code pour insérer un "°".
    m_display.write(0xF8);
    m_display.print("C");

    m_display.setCursor(40, 10);
    m_display.print(humidity);
    // Code pour insérer un "%".
    m_display.write(0x25);

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche les valeurs des capteurs de luminosité et de mouvement, avec deux pictogrammes.
/// @param luminosity La valeur de la luminosité à afficher.
/// @param motionDetected La valeur du capteur de mouvement à afficher.
void Display::displayLuminosityMotionSensorValues(int luminosity, bool motionDetected)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.drawBitmap(0, 0, luminosityMotionSensorsBitmap, 128, 64, WHITE);
    m_display.setTextSize(2);

    m_display.setCursor(40, 42);
    m_display.print(luminosity);

    m_display.setCursor(40, 10);

    if (motionDetected)
        m_display.print("OUI");

    else
        m_display.print("NON");

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche trois jauges correspondant à l'intensité des trois couleurs.
/// @param r La valeur de l'intensité du rouge.
/// @param g La valeur de l'intensité du vert.
/// @param b La valeur de l'intensité du bleu.
void Display::displayLEDState(int r, int g, int b)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.setTextSize(2);

    m_display.setCursor(22, 0);
    m_display.print("R");
    m_display.fillRect(25, 18, 4, int(map(r, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(24, 17, 6, 47, WHITE);

    m_display.setCursor(62, 0);
    m_display.print("V");
    m_display.fillRect(65, 18, 4, int(map(g, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(64, 17, 6, 47, WHITE);

    m_display.setCursor(101, 0);
    m_display.print("B");
    m_display.fillRect(102, 18, 4, int(map(b, 0, 255, 0, 45)), WHITE);
    m_display.drawRect(101, 17, 6, 47, WHITE);

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche une animation illustrant la mise en marche ou l'arrêt d'un périphérique.
/// @param on Mise en marche ou errêt.
void Display::displayDeviceState(bool on)
{
    if (!m_operational)
        return;

    m_display.clearDisplay();

    // Animation de mise en marche.
    if (on == true)
    {
        for (int i = 51; i < 75; i += 4)
        {
            m_display.clearDisplay();
            m_display.drawRoundRect(41, 14, 46, 24, 12, WHITE);
            m_display.fillCircle(i, 26, 8, WHITE);
            m_display.display();
        }
    }

    // Animation d'arrêt.
    else
    {
        for (int i = 75; i > 51; i -= 4)
        {
            m_display.clearDisplay();
            m_display.drawRoundRect(41, 14, 46, 24, 12, WHITE);
            m_display.drawCircle(i, 26, 8, WHITE);
            m_display.display();
        }
    }

    m_lastTime = millis();
}

/// @brief Affiche les informations sur le menu sélectionné.
void Display::displayKeypadMenu()
{
    if (!m_operational)
        return;

    m_display.clearDisplay();
    m_display.setTextSize(1);
    m_display.setCursor(0, 0);

    /*switch (keypadMenu)
    {
    case LIGHTS_MENU:
        m_display.drawBitmap(0, 0, lightsMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(20, 55);
        m_display.print("Menu : lumieres");
        break;

    case SOFA_LIGHT_CONTROL_SUBMENU:
        m_display.print("Lampe canape");

        m_display.setCursor(0, 15);
        m_display.println("1. Temperature");
        m_display.println("2. Luminosite");
        m_display.print("3. Effet");
        break;

    case SOFA_LIGHT_TEMPERATURE_CONTROL_SUBMENU:
        m_display.print("Temperature : PAS CONFIGURE");
        // Afficher rectangle temperature.
        break;

    case SOFA_LIGHT_LUMINOSITY_CONTROL_SUBMENU:
        m_display.print("Luminosite : PAS CONFIGURE");
        // Afficher rectangle luminosite.
        break;

    case SOFA_LIGHT_EFFECT_CONTROL_SUBMENU:
        m_display.print("1. Test - 2. Test - 3. Test PAS CONFIGURE");
        break;

    case BEDSIDE_LIGHT_CONTROL_SUBMENU:
        m_display.print("Lampe de chevet");

        m_display.setCursor(0, 15);
        m_display.println("1. Temperature");
        m_display.println("2. Couleur");
        m_display.println("3. Luminosite");
        m_display.print("4. Effet");
        break;

    case BEDSIDE_LIGHT_TEMPERATURE_CONTROL_SUBMENU:
        m_display.print("Temperature : PAS CONFIGURE");
        // Afficher rectangle temperature.
        break;

    case BEDSIDE_LIGHT_COLOR_CONTROL_SUBMENU:
        m_display.print("Couleur : PAS CONFIGURE");
        // Afficher les 3 rectangles de la couleur.
        break;

    case BEDSIDE_LIGHT_LUMINOSITY_CONTROL_SUBMENU:
        m_display.print("Luminosite : PAS CONFIGURE");
        // Afficher rectangle luminosite.
        break;

    case BEDSIDE_LIGHT_EFFECT_CONTROL_SUBMENU:
        m_display.print("1. Test - 2. Test - 3. Test PAS CONFIGURE");
        break;

    case RGB_STRIP_CONTROL_SUBMENU:
        m_display.print("Rubans de DEL");

        m_display.setCursor(0, 15);
        m_display.println("1. Couleur");
        m_display.print("2. Effet");
        break;

    case RGB_STRIP_COLOR_CONTROL_SUBMENU:
        displayLEDState();
        break;

    case RGB_STRIP_EFFECT_CONTROL_SUBMENU:
        m_display.print("Effet");

        m_display.setCursor(0, 15);
        m_display.println("1. Multicolore");
        m_display.print("2. Son reaction");
        break;

    case DEVICES_MENU:
        m_display.drawBitmap(0, 0, devicesMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(4, 55);
        m_display.print("Menu : peripheriques");
        break;

    case SENSORS_MENU:
        m_display.drawBitmap(0, 0, sensorsMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(20, 55);
        m_display.print("Menu : capteurs");
        break;

    case TV_MENU:
        m_display.drawBitmap(0, 0, TVMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(15, 55);
        m_display.print("Menu : television");
        break;

    case CONFIGURATION_MENU:
        m_display.drawBitmap(0, 0, configurationMenuBitmap, 128, 64, WHITE);
        m_display.setCursor(5, 55);
        m_display.print("Menu : configuration");
        break;

    case ALARM_CODE_CONFIGURATION_SUBMENU:
        m_display.print("Entrez le mot de passe pour acceder au menu de gestion de l'alarme.");
        break;

    case ALARM_CONFIGURATION_SUBMENU:
        m_display.print("Alarme");

        m_display.setCursor(0, 15);
        m_display.println("1. Son de l'alarme");
        m_display.println("2. Ajouter une carte");
        m_display.print("3. Suprimer les cartes");
        break;

    default:
        break;
    }*/

    m_display.display();
    m_lastTime = millis();
}

/// @brief Affiche l'animation d'ouverture ou de fermeture du plateau.
/// @param on Ouverture ou fermeture du plateau.
/// @param shareInformation Affichage ou non de l'animation (permet d'obtenir un délai sans rien afficher).
void Display::displayTray(bool on, bool shareInformation)
{
    if (!m_operational)
        return;

    if (on)
    {
        m_display.clearDisplay();
        m_display.fillRoundRect(5, -10, 118, 20, 5, WHITE);
        for (int i = 0; i < 40; i++)
        {
            m_display.drawLine(28, 11 + i, 100, 11 + i, WHITE);
            if (shareInformation)
                m_display.display();
            delay(15);
        }

        if (shareInformation)
            m_lastTime = millis();
    }

    else
    {
        m_display.fillRoundRect(5, -10, 118, 20, 5, WHITE);
        for (int i = 0; i < 40; i++)
            m_display.drawLine(28, 11 + i, 100, 11 + i, WHITE);

        if (shareInformation)
            m_display.display();

        for (int i = 0; i < 40; i++)
        {
            m_display.drawLine(28, 50 - i, 100, 50 - i, BLACK);
            if (shareInformation)
                m_display.display();
            delay(15);
        }

        if (shareInformation)
            m_lastTime = millis();
    }
}

/// @brief Méthode d'exécution des tâches liées à l'écran : mise en veille de l'écran au bout d'un certain temps.
void Display::loop()
{
    if (m_operational && (m_lastTime != 0) && ((millis() - m_lastTime) >= 8000))
    {
        m_lastTime = 0;
        m_display.clearDisplay();
        m_display.display();
    }
}
