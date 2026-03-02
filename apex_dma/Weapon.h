#pragma once
#include <string>
#include <unordered_map>

enum WeaponIDs {
    SENTINEL = 1,
    THROWING_KNIFE = 2,
    LONGBOW = 89,
    EVA8 = 92,
    G7_SCOUT = 95,
    HEMLOK = 96,
    KRABER = 98,
    MASTIFF = 101,
    MOZAMBIQUE = 103,
    PEACEKEEPER = 111,
    P2020 = 114,
    TRIPLE_TAKE = 116,
    WINGMAN = 117,
    REPEATER_3030 = 120,
    NEMESIS = 122,
    BOCEK = 182,
};

inline std::string get_weapon_name(int id) {
    static const std::unordered_map<int, std::string> weapon_map = {
        {WeaponIDs::SENTINEL, "Sentinel"},
        {WeaponIDs::THROWING_KNIFE, "Throwing Knife"},
        {WeaponIDs::LONGBOW, "Longbow"},
        {WeaponIDs::EVA8, "EVA-8 Auto"},
        {WeaponIDs::G7_SCOUT, "G7 Scout"},
        {WeaponIDs::HEMLOK, "Hemlok"},
        {WeaponIDs::KRABER, "Kraber"},
        {WeaponIDs::MASTIFF, "Mastiff"},
        {WeaponIDs::MOZAMBIQUE, "Mozambique"},
        {WeaponIDs::PEACEKEEPER, "Peacekeeper"},
        {WeaponIDs::P2020, "P2020"},
        {WeaponIDs::TRIPLE_TAKE, "Triple Take"},
        {WeaponIDs::WINGMAN, "Wingman"},
        {WeaponIDs::REPEATER_3030, "30-30 Repeater"},
        {WeaponIDs::NEMESIS, "Nemesis"},
        {WeaponIDs::BOCEK, "Bocek"}
    };
    auto it = weapon_map.find(id);
    if (it != weapon_map.end()) return it->second;
    return "Unknown";
}
