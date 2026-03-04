#pragma once
#include <string>
#include <unordered_map>

enum WeaponIDs {
    R301 = 0,
    SENTINEL = 1,
    THROWING_KNIFE = 2,
    RAMPAGE = 6,
    ALTERNATOR = 84,
    RE45 = 85,
    CHARGE_RIFLE = 88,
    LONGBOW = 89,
    DEVOTION = 90,
    HAVOC = 91,
    EVA8 = 92,
    FLATLINE = 94,
    G7_SCOUT = 95,
    HEMLOK = 96,
    KRABER = 98,
    LSTAR = 100,
    MASTIFF = 101,
    MOZAMBIQUE = 103,
    PROWLER = 110,
    PEACEKEEPER = 111,
    R99 = 113,
    P2020 = 114,
    TRIPLE_TAKE = 116,
    WINGMAN = 117,
    SPITFIRE = 118,
    CAR = 119,
    REPEATER_3030 = 120,
    VOLT = 121,
    NEMESIS = 122,
    BOCEK = 182,
};

inline std::string get_weapon_name(int id) {
    static const std::unordered_map<int, std::string> weapon_map = {
        {WeaponIDs::R301, "R-301 Carbine"},
        {WeaponIDs::SENTINEL, "Sentinel"},
        {WeaponIDs::THROWING_KNIFE, "Throwing Knife"},
        {WeaponIDs::RAMPAGE, "Rampage LMG"},
        {WeaponIDs::ALTERNATOR, "Alternator SMG"},
        {WeaponIDs::RE45, "RE-45 Auto"},
        {WeaponIDs::CHARGE_RIFLE, "Charge Rifle"},
        {WeaponIDs::LONGBOW, "Longbow DMR"},
        {WeaponIDs::DEVOTION, "Devotion LMG"},
        {WeaponIDs::HAVOC, "Havoc Rifle"},
        {WeaponIDs::EVA8, "EVA-8 Auto"},
        {WeaponIDs::FLATLINE, "VK-47 Flatline"},
        {WeaponIDs::G7_SCOUT, "G7 Scout"},
        {WeaponIDs::HEMLOK, "Hemlok Burst AR"},
        {WeaponIDs::KRABER, "Kraber .50-Cal Sniper"},
        {WeaponIDs::LSTAR, "L-STAR EMG"},
        {WeaponIDs::MASTIFF, "Mastiff Shotgun"},
        {WeaponIDs::MOZAMBIQUE, "Mozambique Shotgun"},
        {WeaponIDs::PROWLER, "Prowler Burst PDW"},
        {WeaponIDs::PEACEKEEPER, "Peacekeeper"},
        {WeaponIDs::R99, "R-99 SMG"},
        {WeaponIDs::P2020, "P2020"},
        {WeaponIDs::TRIPLE_TAKE, "Triple Take"},
        {WeaponIDs::WINGMAN, "Wingman"},
        {WeaponIDs::SPITFIRE, "M600 Spitfire"},
        {WeaponIDs::CAR, "C.A.R. SMG"},
        {WeaponIDs::REPEATER_3030, "30-30 Repeater"},
        {WeaponIDs::VOLT, "Volt SMG"},
        {WeaponIDs::NEMESIS, "Nemesis Burst AR"},
        {WeaponIDs::BOCEK, "Bocek Compound Bow"}
    };
    auto it = weapon_map.find(id);
    if (it != weapon_map.end()) return it->second;
    return "Unknown";
}
