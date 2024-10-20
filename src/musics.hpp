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

// Musique : Cruel Summer.
const Action CruelSummerActions[] PROGMEM = {
    {2280, "09010255000000"},
    {4360, "98030255000000"},
    {5800, "970206535"},
    {7240, "98000"},
    {7241, "97000"},
    {7242, "09011255000000000000000004002"},
    {7960, "96001"},
    {8640, "96000"},
    {8641, "05001"},
    {10080, "05000"},
    {10081, "07001"},
    {11480, "07000"},
    {11481, "09010070255110"},
    {12920, "09000"},
    {12921, "97001"},
    {14320, "97000"},
    {14321, "980315000"},
    {15680, "98000"},
    {15681, "09010216100255"},
    {17120, "09000"},
    {17121, "04001"},
    {18520, "04000"},
    {18521, "01001"},
    {19960, "02001"},
    {21360, "04001"},
    {22760, "05001"},
    {24160, "09011000000000255178023010001"},
    {25600, "98030255178023"},
    {27080, "97001"},
    {28360, "96001"},
    {29880, "96000"},
    {30480, "03001"},
    {31280, "09011255178023243255023010003"},
    {34120, "07001"},
    {35480, "07000"},
    {35481, "09011243255023255178023010003"},
    {38240, "09011255178023243255023010003"},
    {39720, "09011243255023255178023010003"},
    {41120, "09011255178023243255023010003"},
    {41880, "02000"},
    {41881, "04000"},
    {41882, "05000"},
    {41883, "07000"},
    {41884, "09000"},
    {41885, "96000"},
    {41886, "97000"},
    {41887, "98000"},
    {42520, "0901225522103301400"},
    {53200, "03000"},
    {53201, "09000"},
    {55920, "09010091066044"},
    {57240, "09010058110167"},
    {59440, "09010191166144"},
    {60880, "09010091066044"}, 
    {60880, "05001"},
    {62280, "02001"},
    {66520, "09011191166144255221192010001"},
    {67880, "09000"},
    {68120, "98030000000255"},
    {69000, "05000"},
    {70760, "02000"},
    {71520, "09011000000000005011118005001"},
    {72680, "01001"},
    {74680, "01000"},
    {76880, "97001"},
    {78560, "03001"},
    {78561, "07000"},
    {78561, "09010255000200"},
    {79320, "09010000000255"},
    {80400, "09010000255000"},
    {82040, "09000"},
    {83480, "96001"},
    {83880, "96000"},
    {86960, "09011000000000105077036005002"},
    {89760, "09010255255255"},
    {90800, "09000"},
    {90801, "970204000"},
    {91640, "980314000"},
    {92400, "96001"},
    {93040, "96000"},
    {93041, "97000"},
    {93042, "98000"},
    {95440, "05001"},
    {97360, "05000"},
    {101200, "03000"},
    {109280, "04001"},
    {112320, "04000"},
    {115080, "09011000000000031171214010000"},
    {118240, "09000"},
    {118241, "98001"},
    {120880, "98000"},
    {121000, "0901201407509400100"},
    {123960, "09000"},
    {126520, "09011000000000189255241010001"},
    {127520, "96001"},
    {131440, "97001"},
    {131441, "98001"},
    {134320, "03001"},
    {136360, "96000"},
    {136361, "97000"},
    {137362, "98000"},
    {137363, "09010119120106"},
    {138960, "02001"},
    {142720, "05001"},
    {144160, "04001"},
    {145480, "98001"},
    {149200, "02000"},
    {149201, "05000"},
    {149202, "04000"},
    {149203, "96001"},
    {151360, "02001"},
    {151361, "96000"},
    {153840, "09011106181232050085109010000"},
    {154440, "09011106181232050085109010000"},
    {155120, "09000"},
    {155121, "98001"},
    {156240, "01001"},
    {158880, "01000"},
    {159920, "09011106181232050085109010000"},
    {165400, "05001"},
    {165401, "99001"},
    {171760, "09011050085109000000000010000"},
    {173120, "03000"},
    {177600, "99000"},
};
const char CruelSummerName[] PROGMEM = "Cruel Summer";
const char CruelSummerVideoURL[] PROGMEM = "https://files.catbox.moe/z0b1qk.mp4";
const Music CruelSummerMusic PROGMEM = {
    CruelSummerName,
    CruelSummerVideoURL,
    CruelSummerActions,
    124};

// Musique : Test.
const Action testActions[] PROGMEM = {
    {2000, "05001"},
    {2480, "05000"},
    {2960, "05001"},
    {3440, "05000"},
    {3441, "96001"},
    {3920, "96000 "},
    {4400, "09011255000255000255000050003"},
};
const char testName[] PROGMEM = "Test";
const char testVideoURL[] PROGMEM = "https://files.catbox.moe/zeq9bn.mp4";
const Music testMusic PROGMEM = {
    testName,
    testVideoURL,
    testActions,
    7};

#endif