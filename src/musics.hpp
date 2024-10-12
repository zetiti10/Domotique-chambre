#ifndef MUSICS_DEFINITIONS
#define MUSICS_DEFINITIONS

// Autres fichiers du programme.
#include "device/output/television.hpp"

// Musique : World Smallest Violin.
const Action worldsSmallestViolinActions[] PROGMEM = {
    {3900, "09011000000000255255255041002"},
    {8000, "09000"},
    {9000, "96001"},
    {11000, "96000"},
    {11001, "97001"},
    {13000, "97000"},
    {13001, "98001"},
    {11000, "96000"},
    {15000, "09011000050050255000000020003"},
    {17500, "09010200000000"},
    {19225, "09010125000000"},
    {21225, "09010050000000"},
    {21226, "98000"},
    {23225, "01001"},
    {25000, "04001"},
    {27400, "05001"},
    {41750, "97021050"},
    {41751, "090120342550890255"},
    {49600, "03001"},
    {58000, "01000"},
    {58001, "02000"},
    {58002, "03000"},
    {58003, "04000"},
    {58004, "09000"},
    {58005, "97000"},
};
const char worldsSmallestViolinName[] PROGMEM = "World's Smallest Violin";
const char worldsSmallestViolinVideoURL[] PROGMEM = "https://files.catbox.moe/cq6ib0.mp4";
const Music worldsSmallestViolinMusic PROGMEM = {
    worldsSmallestViolinName,
    worldsSmallestViolinVideoURL,
    worldsSmallestViolinActions,
    25};

#endif