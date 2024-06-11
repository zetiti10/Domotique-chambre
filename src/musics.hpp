#ifndef MUSICS_DEFINITIONS
#define MUSICS_DEFINITIONS

#include "device/output/television.hpp"

// Musiques.
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
const char friendlyName[] PROGMEM = "Test 1";
const char videoURL[] PROGMEM = "https://www.udrop.com/LK18/Test_1.mp4?download_token=6ae5d678b2fc48fd8943f475f48412619ea441890ddad19467a3482be3a73928";

const Music test1 PROGMEM = {
    friendlyName,
    videoURL,
    test1Music,
    9};

#endif