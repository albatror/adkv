#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "ItemManager.h"

namespace WeaponIDs {
    inline constexpr int SHEYLA_MOUNTED = 11;
    inline constexpr int P2020 = 120;
    inline constexpr int ALTERNATOR = 86;
    inline constexpr int R99 = 118;
    inline constexpr int R301 = 0;
    inline constexpr int SPITFIRE = 122;
    inline constexpr int G7_SCOUT = 100;
    inline constexpr int FLATLINE = 98;
    inline constexpr int HEMLOCK = 101;
    inline constexpr int PROWLER = 114;
    inline constexpr int REPEATER_3030 = 128;
    inline constexpr int RAMPAGE = 7;
    inline constexpr int CAR = 131;
    inline constexpr int RE45 = 89;
    inline constexpr int HAVOC = 94;
    inline constexpr int DEVOTION = 91;
    inline constexpr int LSTAR = 105;
    inline constexpr int TRIPLE_TAKE = 123;
    inline constexpr int VOLT = 127;
    inline constexpr int NEMESIS = 132;
    inline constexpr int MOZAMBIQUE = 109;
    inline constexpr int PEACEKEEPER = 116;
    inline constexpr int MASTIFF = 107;
    inline constexpr int EVA8 = 96;
    inline constexpr int LONGBOW = 93;
    inline constexpr int CHARGE_RIFLE = 90;
    inline constexpr int SENTINEL = 2;
    inline constexpr int WINGMAN = 125;
    inline constexpr int BOCEK = 3;
    inline constexpr int KRABER = 103;
    inline constexpr int FISTS = 136;
    inline constexpr int KNIFE = 198;
}

inline std::string get_weapon_name(int id) {
    static const std::unordered_map<int, std::string> weapon_map = {
        {WeaponIDs::SHEYLA_MOUNTED, "Sheyla Mounted"},
        {WeaponIDs::P2020, "P2020 Akimbo"},
        {WeaponIDs::ALTERNATOR, "Alternator"},
        {WeaponIDs::R99, "R-99"},
        {WeaponIDs::R301, "R301"},
        {WeaponIDs::SPITFIRE, "Spitfire"},
        {WeaponIDs::G7_SCOUT, "G7 Scout"},
        {WeaponIDs::FLATLINE, "Flatline"},
        {WeaponIDs::HEMLOCK, "Hemlok"},
        {WeaponIDs::PROWLER, "Prowler"},
        {WeaponIDs::REPEATER_3030, "30-30"},
        {WeaponIDs::RAMPAGE, "Rampage"},
        {WeaponIDs::CAR, "C.A.R"},
        {WeaponIDs::RE45, "RE-45"},
        {WeaponIDs::HAVOC, "Havoc"},
        {WeaponIDs::DEVOTION, "Devotion"},
        {WeaponIDs::LSTAR, "L-STAR"},
        {WeaponIDs::TRIPLE_TAKE, "TripleTake"},
        {WeaponIDs::VOLT, "Volt SMG"},
        {WeaponIDs::NEMESIS, "Nemesis"},
        {WeaponIDs::MOZAMBIQUE, "Mozambique"},
        {WeaponIDs::PEACEKEEPER, "Peacekeeper"},
        {WeaponIDs::MASTIFF, "Mastiff"},
        {WeaponIDs::EVA8, "EVA-8"},
        {WeaponIDs::LONGBOW, "Longbow"},
        {WeaponIDs::CHARGE_RIFLE, "Charge Rifle"},
        {WeaponIDs::SENTINEL, "Sentinel"},
        {WeaponIDs::WINGMAN, "Wingman"},
        {WeaponIDs::BOCEK, "BOCEK"},
        {WeaponIDs::KRABER, "Kraber"},
        {WeaponIDs::FISTS, "Fists"},
        {WeaponIDs::KNIFE, "Throwing Knife"}
    };
    auto it = weapon_map.find(id);
    if (it != weapon_map.end()) return it->second;
    return "Unknown";
}

inline bool isRapidFireWeapon(const std::string& name) {
    static const std::vector<std::string> rapid_fire_weapons = {
        "P2020", "R-301", "G7 Scout", "Flatline", "Hemlok", "Prowler", "Nemesis", "Mozambique", "Wingman", "EVA-8"
    };
    for (const auto& w : rapid_fire_weapons) {
        if (name.find(w) != std::string::npos) return true;
    }
    return false;
}

inline bool isFlickbotWeapon(const std::string& name) {
    static const std::vector<std::string> flickbot_weapons = {
        "P2020", "RE-45", "Alternator", "R-99", "R-301", "Spitfire", "G7 Scout", "Flatline", "Hemlok", "30-30", "Rampage", "C.A.R", "Havoc", "Devotion", "L-STAR", "TripleTake", "Volt SMG", "Nemesis", "Mozambique", "EVA-8", "Peacekeeper", "Mastiff", "Longbow", "Charge Rifle", "Sentinel", "Wingman", "Prowler", "BOCEK", "Kraber", "Throwing Knife"
    };
    for (const auto& w : flickbot_weapons) {
        if (name.find(w) != std::string::npos) return true;
    }
    return false;
}

inline std::string get_weapon_name_by_model(const std::string& modelName) {
    std::string itemName;
    ItemCategory category;
    if (ItemManager::getInstance().GetItemInfo(modelName, itemName, category)) {
        return itemName;
    }
    return "Unknown";
}
