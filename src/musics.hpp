#ifndef MUSICS_DEFINITIONS
#define MUSICS_DEFINITIONS

#include "device/output/television.hpp"

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
    {58005, "97000"},
};
const char worldsSmallestViolinName[] PROGMEM = "Test PPAP";
const char worldsSmallestViolinVideoURL[] PROGMEM = "https://www.udrop.com/LLUx/Worlds_Smallest_Violin.mp4?download_token=8db514c04dd1eb8aab4381e5dbfbe91481864bec379e8640c5b0058d230bfb01";
const Music worldsSmallestViolinMusic PROGMEM = {
    worldsSmallestViolinName,
    worldsSmallestViolinVideoURL,
    worldsSmallestViolinActions,
    25};

#endif