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
const char worldsSmallestViolinVideoURL[] PROGMEM = "https://www.udrop.com/LLUx/Worlds_Smallest_Violin.mp4?download_token=8db514c04dd1eb8aab4381e5dbfbe91481864bec379e8640c5b0058d230bfb01";
const Music worldsSmallestViolinMusic PROGMEM = {
    worldsSmallestViolinName,
    worldsSmallestViolinVideoURL,
    worldsSmallestViolinActions,
    25};

const Action CruelSummerActions[] PROGMEM = {
    {16497, "09010128128128"},
    {16498, "09010122122122"},
    {16499, "09010138138138"},
    {18419, "09000"},
    {18422, "09000"},
    {18454, "09000"},
    {18498, "09010128128128"},
    {18499, "09010136136136"},
    {18500, "09010122122122"},
    {20393, "09000"},
    {20413, "09000"},
    {20425, "09000"},
    {20498, "09010122122122"},
    {20499, "09010114114114"},
    {20500, "09010132132132"},
    {22395, "09000"},
    {22410, "09000"},
    {22417, "09000"},
    {22497, "09010124124124"},
    {22498, "09010132132132"},
    {22499, "09010114114114"},
    {24495, "09000"},
    {24496, "09000"},
    {24497, "09000"},
    {24498, "09010128128128"},
    {24499, "09010122122122"},
    {24500, "09010138138138"},
    {26419, "09000"},
    {26422, "09000"},
    {26454, "09000"},
    {26498, "09010128128128"},
    {26499, "09010136136136"},
    {26500, "09010122122122"},
    {28393, "09000"},
    {28413, "09000"},
    {28425, "09000"},
    {28498, "09010122122122"},
    {28499, "09010114114114"},
    {28500, "09010132132132"},
    {30395, "09000"},
    {30410, "09000"},
    {30417, "09000"},
    {30498, "09010124124124"},
    {30499, "09010132132132"},
    {30500, "09010114114114"},
    {32493, "09000"},
    {32496, "09000"},
    {32497, "09000"},
};
const char CruelSummerName[] PROGMEM = "Cruel Summer";
const char CruelSummerVideoURL[] PROGMEM = "https://files.catbox.moe/confn2.mp4";
const Music CruelSummerMusic PROGMEM = {
    CruelSummerName,
    CruelSummerVideoURL,
    CruelSummerActions,
    48
};

#endif