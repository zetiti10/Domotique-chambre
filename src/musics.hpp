#ifndef MUSICS_DEFINITIONS
#define MUSICS_DEFINITIONS

#include "device/output/television.hpp"

const Action test1Music[] PROGMEM = {
    {8800, "09011000000000255255255010001"},
    {9800, "09011255255255000000000010001"},
    {11000, "09011000000000255255255010003"},
    {17000, "02001"},
    {17000, "090122552552550100"},
    {20000, "09011255100000000200255030003"},
    {25000, "03001"},
    {25000, "02000"},
    {30000, "03000"},
};
const char friendlyName1[] PROGMEM = "Test PPAP";
const char videoURL1[] PROGMEM = "https://www.udrop.com/";

const Music test1 PROGMEM = {
    friendlyName1,
    videoURL1,
    test1Music,
    9};

const Action test2Music[] PROGMEM = {
    {1600, "03001"},
    {3600, "03000"},
    {5600, "03001"},
    {7600, "03000"},
    {8600, "03001"},
    {9600, "03000"},
    {10600, "09011000000000255255255010001"},
    {11600, "09011255255255000000000010001"},
};
const char friendlyName2[] PROGMEM = "Test Couleurs";
const char videoURL2[] PROGMEM = "https://www.udrop.com/";

const Music test2 PROGMEM = {
    friendlyName2,
    videoURL2,
    test2Music,
    8};

#endif