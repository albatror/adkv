#pragma once
#include <string>
#include <unordered_map>

namespace WeaponIDs {
    inline constexpr int P2020 = 119;
    inline constexpr int ALTERNATOR = 86;
    inline constexpr int R99 = 117;
    inline constexpr int R301 = 0;
    inline constexpr int SPITFIRE = 121;
    inline constexpr int G7_SCOUT = 100;
    inline constexpr int FLATLINE = 98;
    inline constexpr int HEMLOCK = 101;
    inline constexpr int PROWLER = 113;
    inline constexpr int REPEATER_3030 = 127;
    inline constexpr int RAMPAGE = 7;
    inline constexpr int CAR = 130;
    inline constexpr int RE45 = 87;
    inline constexpr int HAVOC = 94;
    inline constexpr int DEVOTION = 91;
    inline constexpr int LSTAR = 104;
    inline constexpr int TRIPLE_TAKE = 122;
    inline constexpr int VOLT = 126;
    inline constexpr int NEMESIS = 131;
    inline constexpr int MOZAMBIQUE = 108;
    inline constexpr int PEACEKEEPER = 115;
    inline constexpr int MASTIFF = 106;
    inline constexpr int EVA8 = 96;
    inline constexpr int LONGBOW = 93;
    inline constexpr int CHARGE_RIFLE = 90;
    inline constexpr int SENTINEL = 2;
    inline constexpr int WINGMAN = 124;
    inline constexpr int BOCEK = 3;
    inline constexpr int KRABER = 103;
    inline constexpr int KNIFE = 198;
}

inline std::string get_weapon_name(int id) {
    static const std::unordered_map<int, std::string> weapon_map = {
        {WeaponIDs::P2020, "P2020"},
        {WeaponIDs::ALTERNATOR, "Alternator SMG"},
        {WeaponIDs::R99, "R-99 SMG"},
        {WeaponIDs::R301, "R-301 Carbine"},
        {WeaponIDs::SPITFIRE, "M600 Spitfire"},
        {WeaponIDs::G7_SCOUT, "G7 Scout"},
        {WeaponIDs::FLATLINE, "VK-47 Flatline"},
        {WeaponIDs::HEMLOCK, "Hemlock Burst AR"},
        {WeaponIDs::PROWLER, "Prowler Burst PDW"},
        {WeaponIDs::REPEATER_3030, "30-30 Repeater"},
        {WeaponIDs::RAMPAGE, "Rampage LMG"},
        {WeaponIDs::CAR, "C.A.R. SMG"},
        {WeaponIDs::RE45, "RE-45 Auto"},
        {WeaponIDs::HAVOC, "Havoc Rifle"},
        {WeaponIDs::DEVOTION, "Devotion LMG"},
        {WeaponIDs::LSTAR, "L-STAR EMG"},
        {WeaponIDs::TRIPLE_TAKE, "Triple Take"},
        {WeaponIDs::VOLT, "Volt SMG"},
        {WeaponIDs::NEMESIS, "Nemesis Burst AR"},
        {WeaponIDs::MOZAMBIQUE, "Mozambique"},
        {WeaponIDs::PEACEKEEPER, "Peacekeeper"},
        {WeaponIDs::MASTIFF, "Mastiff Shotgun"},
        {WeaponIDs::EVA8, "EVA-8 Auto"},
        {WeaponIDs::LONGBOW, "Longbow DMR"},
        {WeaponIDs::CHARGE_RIFLE, "Charge Rifle"},
        {WeaponIDs::SENTINEL, "Sentinel"},
        {WeaponIDs::WINGMAN, "Wingman"},
        {WeaponIDs::BOCEK, "Bocek Compound Bow"},
        {WeaponIDs::KRABER, "Kraber .50-Cal Sniper"},
        {WeaponIDs::KNIFE, "Throwing Knife"}
    };
    auto it = weapon_map.find(id);
    if (it != weapon_map.end()) return it->second;
    return "Unknown";
}
