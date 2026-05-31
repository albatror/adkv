#pragma once
#include <string>
#include <unordered_map>
#include <algorithm>
#include "ItemManager.h"

// ---------------------------------------------------------------------------
// Model-folder -> display name map.
// Folder names inside the game's weapon model paths never change between
// patches, making this far more stable than m_weaponNameIndex IDs.
// Add new weapons here; no ID table update needed.
// ---------------------------------------------------------------------------
inline const std::unordered_map<std::string, std::string>& get_model_folder_map() {
    static const std::unordered_map<std::string, std::string> folder_map = {
        // R-301 / Carbine
        {"r301",                "R-301"},
        {"rspn101",             "R-301"},
        // R-99
        {"r97",                 "R-99"},
        {"r99",                 "R-99"},
        // P2020
        {"p2011",               "P2020"},
        {"p2020",               "P2020"},
        // RE-45
        {"p2011_auto",          "RE-45"},
        {"re45",                "RE-45"},
        // L-STAR
        {"lstar",               "L-STAR"},
        // Havoc
        {"beam_ar",             "Havoc"},
        {"havoc",               "Havoc"},
        // Devotion
        {"hemlock_br",          "Devotion"},
        {"devotion",            "Devotion"},
        // Triple Take
        {"doubletake",          "Triple Take"},
        {"tripletake",          "Triple Take"},
        // Volt SMG
        {"hemlok_smg",          "Volt SMG"},
        {"volt",                "Volt SMG"},
        // Nemesis
        {"nemesis",             "Nemesis"},
        // Mastiff
        {"mastiff",             "Mastiff"},
        {"mastiff_stgn",        "Mastiff"},
        // EVA-8
        {"eva8",                "EVA-8"},
        {"eva8_auto",           "EVA-8"},
        // Peacekeeper
        {"peacekeeper",         "Peacekeeper"},
        // Mozambique
        {"pstl_sa3",            "Mozambique"},
        {"mozambique",          "Mozambique"},
        // Flatline — in-game path is typically "vk47_flatline" or "flatline"
        {"flatline",            "Flatline"},
        {"vk47_flatline",       "Flatline"},
        // Hemlok
        {"m1a1_hemlok",         "Hemlok"},
        {"hemlok",              "Hemlok"},
        // Prowler
        {"prowler",             "Prowler"},
        {"prowler_smg",         "Prowler"},
        // 30-30 Repeater
        {"3030repeater",        "30-30"},
        {"repeater3030",        "30-30"},
        {"repeater_3030",       "30-30"},
        // Bocek Bow
        {"compound_bow",        "Bocek"},
        {"compound",            "Bocek"},
        // Rampage
        {"dragon",              "Rampage"},
        {"rampage",             "Rampage"},
        // Rampart Turret (Sheila)
        {"rampart_turret",      "Rampart Turret"},
        // C.A.R. SMG
        {"car",                 "C.A.R."},
        {"car_smg",             "C.A.R."},
        // Kraber
        {"at_rifle",            "Kraber"},
        {"kraber",              "Kraber"},
        // Longbow
        {"rspn101_dmr",         "Longbow"},
        {"longbow",             "Longbow"},
        // Charge Rifle
        {"defender",            "Charge Rifle"},
        {"chargerifle",         "Charge Rifle"},
        // Wingman
        {"b3wing",              "Wingman"},
        {"wingman",             "Wingman"},
        {"b3",                  "Wingman"},
        // Sentinel
        {"sentinel",            "Sentinel"},
        // G7 Scout
        {"g7",                  "G7 Scout"},
        {"g2",                  "G7 Scout"},
        // Alternator
        {"alternator_smg",      "Alternator"},
        {"alternator",          "Alternator"},
        // Spitfire
        {"lmg_hemlok",          "Spitfire"},
        {"spitfire",            "Spitfire"},
        // Vantage
        {"misc_vantage_ultimate","Vantage Ult"},
        // Grenades / throwables
        {"shuriken",            "Throwing Knife"},
        {"frag_grenade",        "Frag Grenade"},
        {"thermite_grenade",    "Thermite"},
        {"arc_star",            "Arc Star"},
    };
    return folder_map;
}

// ---------------------------------------------------------------------------
// Extract the weapon folder segment from a full model path.
// Handles all Apex path variants:
//   "mdl/Weapons/r301/..."       -> "r301"
//   "mdl/Weapons_r2/Flatline/..."-> "flatline"
//   "mdl/Weapons_r3/Nemesis/..." -> "nemesis"
// ---------------------------------------------------------------------------
inline std::string extract_weapon_folder(const std::string& modelPath) {
    if (modelPath.empty()) return "";
    std::string lower = modelPath;
    std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

    // Match any segment that starts with "weapons" (weapons/, weapons_r2/, weapons_r3/, etc.)
    size_t search = 0;
    while (search < lower.size()) {
        size_t wpos = lower.find("weapons", search);
        if (wpos == std::string::npos) break;

        // Must be preceded by a path separator or be at start of string
        if (wpos == 0 || lower[wpos - 1] == '/' || lower[wpos - 1] == '\\') {
            // Skip to the end of this "weapons*" folder name
            size_t seg_end = lower.find_first_of("/\\", wpos);
            if (seg_end != std::string::npos) {
                // The NEXT segment is the weapon folder (e.g. "r301", "flatline", "lmg_hemlok")
                size_t folder_start = seg_end + 1;
                size_t folder_end = lower.find_first_of("/\\", folder_start);
                if (folder_end != std::string::npos && folder_end > folder_start)
                    return lower.substr(folder_start, folder_end - folder_start);
            }
        }
        search = wpos + 1;
    }
    return "";
}

// ---------------------------------------------------------------------------
// Resolve a display name from a full model path.
// Priority:
//   1. ItemManager (weapons.ini substring match – fast, cached)
//   2. Folder extraction + static folder_map lookup
//   3. Raw folder name (ugly but never "Unknown" if path was valid)
// ---------------------------------------------------------------------------
inline std::string get_weapon_name_by_model(const std::string& modelName) {
    if (modelName.empty() || modelName == "unknown") return "Unknown";

    // 1. ItemManager cache / weapons.ini
    std::string itemName;
    ItemCategory category;
    if (ItemManager::getInstance().GetItemInfo(modelName, itemName, category))
        return itemName;

    // 2. Parse the path and look up the folder in the stable folder map
    std::string folder = extract_weapon_folder(modelName);
    if (!folder.empty()) {
        const auto& fm = get_model_folder_map();
        auto it = fm.find(folder);
        if (it != fm.end()) return it->second;
        // 3. Unknown folder – return it as-is so it's at least informative
        return folder;
    }

    return "Unknown";
}

// ---------------------------------------------------------------------------
// ID-based fallback (last resort – IDs change each patch).
// Only called when the model path could not be read at all.
// ---------------------------------------------------------------------------
namespace WeaponIDs {
    inline constexpr int R301            = 0;
    inline constexpr int SENTINEL        = 2;
    inline constexpr int BOCEK           = 3;
    inline constexpr int RAMPAGE         = 7;
    inline constexpr int ALTERNATOR      = 89;
    inline constexpr int RE45            = 92;
    inline constexpr int CHARGE_RIFLE    = 93;
    inline constexpr int HAVOC           = 97;
    inline constexpr int EVA8            = 99;
    inline constexpr int FLATLINE        = 101;
    inline constexpr int G7_SCOUT        = 103;
    inline constexpr int DEVOTION        = 94;
    inline constexpr int HEMLOCK         = 106;
    inline constexpr int KRABER          = 107;
    inline constexpr int LSTAR           = 110;
    inline constexpr int MASTIFF         = 111;
    inline constexpr int MOZAMBIQUE      = 113;
    inline constexpr int LONGBOW         = 96;
    inline constexpr int PROWLER         = 119;
    inline constexpr int PEACEKEEPER     = 121;
    inline constexpr int R99             = 123;
    inline constexpr int P2020           = 125;
    inline constexpr int SPITFIRE        = 127;
    inline constexpr int TRIPLE_TAKE     = 128;
    inline constexpr int WINGMAN         = 130;
    inline constexpr int VOLT            = 132;
    inline constexpr int REPEATER_3030   = 133;
    inline constexpr int CAR             = 136;
    inline constexpr int NEMESIS         = 138;
    inline constexpr int FRAG_GRENADE    = 139;
    inline constexpr int ARC_STAR        = 140;
    inline constexpr int THERMITE        = 141;
    inline constexpr int MELEE           = 142;
    inline constexpr int MELEE_2         = 143;
    inline constexpr int KNIFE           = 223;
    inline constexpr int EVAC_TOWER      = 226;
    inline constexpr int HEAL            = 242;
}

inline std::string get_weapon_name(int id) {
    static const std::unordered_map<int, std::string> weapon_map = {
        {WeaponIDs::R301,          "R-301"},
        {WeaponIDs::SENTINEL,      "Sentinel"},
        {WeaponIDs::BOCEK,         "Bocek"},
        {WeaponIDs::RAMPAGE,       "Rampage"},
        {WeaponIDs::ALTERNATOR,    "Alternator"},
        {WeaponIDs::RE45,          "RE-45"},
        {WeaponIDs::CHARGE_RIFLE,  "Charge Rifle"},
        {WeaponIDs::HAVOC,         "Havoc"},
        {WeaponIDs::EVA8,          "EVA-8"},
        {WeaponIDs::FLATLINE,      "Flatline"},
        {WeaponIDs::G7_SCOUT,      "G7 Scout"},
        {WeaponIDs::DEVOTION,      "Devotion"},
        {WeaponIDs::HEMLOCK,       "Hemlok"},
        {WeaponIDs::KRABER,        "Kraber"},
        {WeaponIDs::LSTAR,         "L-STAR"},
        {WeaponIDs::MASTIFF,       "Mastiff"},
        {WeaponIDs::MOZAMBIQUE,    "Mozambique"},
        {WeaponIDs::LONGBOW,       "Longbow"},
        {WeaponIDs::PROWLER,       "Prowler"},
        {WeaponIDs::PEACEKEEPER,   "Peacekeeper"},
        {WeaponIDs::R99,           "R-99"},
        {WeaponIDs::P2020,         "P2020"},
        {WeaponIDs::SPITFIRE,      "Spitfire"},
        {WeaponIDs::TRIPLE_TAKE,   "Triple Take"},
        {WeaponIDs::WINGMAN,       "Wingman"},
        {WeaponIDs::VOLT,          "Volt SMG"},
        {WeaponIDs::REPEATER_3030, "30-30"},
        {WeaponIDs::CAR,           "C.A.R."},
        {WeaponIDs::NEMESIS,       "Nemesis"},
        {WeaponIDs::FRAG_GRENADE,  "Frag Grenade"},
        {WeaponIDs::ARC_STAR,      "Arc Star"},
        {WeaponIDs::THERMITE,      "Thermite"},
        {WeaponIDs::MELEE,         "Melee"},
        {WeaponIDs::MELEE_2,       "Melee"},
        {WeaponIDs::KNIFE,         "Throwing Knife"},
        {WeaponIDs::EVAC_TOWER,    "Evac Tower"},
        {WeaponIDs::HEAL,          "Heal Drone"},
    };
    auto it = weapon_map.find(id);
    if (it != weapon_map.end()) return it->second;
    return "Unknown";
}
