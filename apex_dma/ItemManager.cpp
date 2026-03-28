#include "ItemManager.h"
#include "ItemID.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

ItemManager::ItemManager() {
    LoadItems("items.ini");
    if (itemDefs.empty()) LoadItems("apex_dma/items.ini");

    LoadWeapons("weapons.ini");
    if (itemDefs.size() < 100) LoadWeapons("apex_dma/weapons.ini"); // items.ini has ~117 lines, weapons.ini ~50
}

ItemCategory ItemManager::DetermineCategory(const std::string& name, int glowId) {
    if (name.find("AMMO") != std::string::npos) return ItemCategory::AMMO;
    if (name.find("WEAPON") != std::string::npos) return ItemCategory::WEAPON;

    switch (glowId) {
        case 37: return ItemCategory::COMMON;
        case 32: return ItemCategory::RARE;
        case 51: return ItemCategory::RARE;
        case 53: return ItemCategory::RARE;
        case 33: return ItemCategory::EPIC;
        case 34: return ItemCategory::EPIC;
        case 81: return ItemCategory::EPIC;
        case 80: return ItemCategory::LEGENDARY;
        case 4:  return ItemCategory::MYTHIC;
        case 65: return ItemCategory::WEAPON;
        default: return ItemCategory::OTHER;
    }
}

void ItemManager::LoadItems(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        ItemDef def;
        if (ss >> def.name >> def.glowId >> def.model) {
            if (def.model.front() == '/' && def.model.back() == '/') {
                def.model = def.model.substr(1, def.model.length() - 2);
            }
            def.category = DetermineCategory(def.name, def.glowId);
            itemDefs.push_back(def);
        }
    }
}

void ItemManager::LoadWeapons(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open " << path << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string model, speed, gvt, dly, ads, name;
        if (ss >> model >> speed >> gvt >> dly >> ads >> name) {
            if (name.front() == '#') name = name.substr(1);
            if (model.front() == '/' && model.back() == '/') {
                model = model.substr(1, model.length() - 2);
            }

            ItemDef def;
            def.name = name;
            def.glowId = -1;
            def.model = model;
            def.category = ItemCategory::WEAPON;
            itemDefs.push_back(def);
        }
    }
}

bool ItemManager::GetItemInfo(const std::string& modelName, std::string& name, ItemCategory& category) {
    auto it = cache.find(modelName);
    if (it != cache.end()) {
        name = it->second.first;
        category = it->second.second;
        return true;
    }

    for (const auto& def : itemDefs) {
        if (modelName.find(def.model) != std::string::npos) {
            name = def.name;
            category = def.category;
            cache[modelName] = {name, category};
            return true;
        }
    }
    return false;
}

bool ItemManager::GetItemInfoByID(int id, std::string& name, ItemCategory& category) {
    name = ItemID::getItemName(id);
    if (name == "Unknown") return false;

    // Default categories based on name or ID ranges
    if (id >= ItemID::mp_weapon_sniper && id <= ItemID::mp_weapon_nemesis) category = ItemCategory::WEAPON;
    else if (id >= ItemID::mp_weapon_dragon_lmg && id <= ItemID::mp_weapon_car_gold) category = ItemCategory::WEAPON;
    else if (id >= ItemID::bullet && id <= ItemID::arrowsBundle) category = ItemCategory::AMMO;
    else if (id >= ItemID::health_pickup_ultimate && id <= ItemID::health_pickup_combo_small) category = ItemCategory::EPIC; // Syringe/Cells are small but often lumped in EPIC esp
    else if (id >= ItemID::helmet_pickup_lv1 && id <= ItemID::helmet_pickup_lv3) category = ItemCategory::RARE;
    else if (id >= ItemID::helmet_pickup_lv4_abilities && id <= ItemID::helmet_pickup_lv5_sunglasses) category = ItemCategory::LEGENDARY;
    else if (id >= ItemID::armor_pickup_lv1 && id <= ItemID::armor_pickup_lv2) category = ItemCategory::RARE;
    else if (id >= ItemID::armor_pickup_lv3 && id <= ItemID::armor_pickup_lv4_all_fast) category = ItemCategory::EPIC;
    else if (id >= ItemID::armor_pickup_lv5_evolving) category = ItemCategory::MYTHIC;
    else if (id >= ItemID::armor_core_pickup_lv1 && id <= ItemID::armor_core_pickup_lv2) category = ItemCategory::RARE;
    else if (id == ItemID::armor_core_pickup_lv3) category = ItemCategory::EPIC;
    else if (id == ItemID::armor_core_pickup_lv5) category = ItemCategory::MYTHIC;
    else category = ItemCategory::OTHER;

    return true;
}

int ItemManager::GetItemIconID(int id, ItemCategory category) {
    switch (id) {
        // Weapons
        case ItemID::mp_weapon_sniper: return 221; // Kraber
        case ItemID::mp_weapon_mastiff:
        case ItemID::mp_weapon_mastiff_whiteset:
        case ItemID::mp_weapon_mastiff_blueset:
        case ItemID::mp_weapon_mastiff_purpleset:
        case ItemID::mp_weapon_mastiff_gold:
        case ItemID::mp_weapon_mastiff_crate: return 263; // Mastiff
        case ItemID::mp_weapon_lstar:
        case ItemID::mp_weapon_lstar_crate:
        case ItemID::mp_weapon_lstar_whiteset:
        case ItemID::mp_weapon_lstar_blueset:
        case ItemID::mp_weapon_lstar_purpleset:
        case ItemID::mp_weapon_lstar_gold: return 222; // L-Star
        case ItemID::mp_weapon_energy_ar:
        case ItemID::mp_weapon_energy_ar_whiteset:
        case ItemID::mp_weapon_energy_ar_blueset:
        case ItemID::mp_weapon_energy_ar_purpleset:
        case ItemID::mp_weapon_energy_ar_gold:
        case ItemID::mp_weapon_energy_ar_crate: return 184; // Havoc
        case ItemID::mp_weapon_esaw:
        case ItemID::mp_weapon_esaw_crate:
        case ItemID::mp_weapon_esaw_whiteset:
        case ItemID::mp_weapon_esaw_blueset:
        case ItemID::mp_weapon_esaw_purpleset:
        case ItemID::mp_weapon_esaw_gold: return 112; // Devotion
        case ItemID::mp_weapon_doubletake:
        case ItemID::mp_weapon_doubletake_crate:
        case ItemID::mp_weapon_doubletake_whiteset:
        case ItemID::mp_weapon_doubletake_blueset:
        case ItemID::mp_weapon_doubletake_purpleset:
        case ItemID::mp_weapon_doubletake_gold: return 529; // TripleTake
        case ItemID::mp_weapon_vinson:
        case ItemID::mp_weapon_vinson_whiteset:
        case ItemID::mp_weapon_vinson_blueset:
        case ItemID::mp_weapon_vinson_purpleset:
        case ItemID::mp_weapon_vinson_gold: return 544; // Flatline
        case ItemID::mp_weapon_hemlok:
        case ItemID::mp_weapon_hemlok_crate:
        case ItemID::mp_weapon_hemlok_whiteset:
        case ItemID::mp_weapon_hemlok_blueset:
        case ItemID::mp_weapon_hemlok_purpleset:
        case ItemID::mp_weapon_hemlok_gold:
        case ItemID::mp_weapon_auto_hemlok:
        case ItemID::mp_weapon_auto_hemlok_whiteset:
        case ItemID::mp_weapon_auto_hemlok_blueset:
        case ItemID::mp_weapon_auto_hemlok_purpleset:
        case ItemID::mp_weapon_auto_hemlok_gold: return 192; // Hemlok
        case ItemID::mp_weapon_g2:
        case ItemID::mp_weapon_g2_crate:
        case ItemID::mp_weapon_g2_whiteset:
        case ItemID::mp_weapon_g2_blueset:
        case ItemID::mp_weapon_g2_purpleset:
        case ItemID::mp_weapon_g2_gold: return 165; // G7 Scout
        case ItemID::mp_weapon_alternator_smg:
        case ItemID::mp_weapon_alternator_smg_crate:
        case ItemID::mp_weapon_alternator_smg_whiteset:
        case ItemID::mp_weapon_alternator_smg_blueset:
        case ItemID::mp_weapon_alternator_smg_purpleset:
        case ItemID::mp_weapon_alternator_smg_gold: return 16; // Alternator
        case ItemID::mp_weapon_r97:
        case ItemID::mp_weapon_r97_whiteset:
        case ItemID::mp_weapon_r97_blueset:
        case ItemID::mp_weapon_r97_purpleset:
        case ItemID::mp_weapon_r97_gold:
        case ItemID::mp_weapon_r97_crate: return 360; // R-99
        case ItemID::mp_weapon_pdw:
        case ItemID::mp_weapon_pdw_crate:
        case ItemID::mp_weapon_pdw_whiteset:
        case ItemID::mp_weapon_pdw_blueset:
        case ItemID::mp_weapon_pdw_purpleset:
        case ItemID::mp_weapon_pdw_gold: return 350; // Prowler
        case ItemID::mp_weapon_volt_smg:
        case ItemID::mp_weapon_volt_smg_whiteset:
        case ItemID::mp_weapon_volt_smg_blueset:
        case ItemID::mp_weapon_volt_smg_purpleset:
        case ItemID::mp_weapon_volt_smg_gold: return 547; // Volt
        case ItemID::mp_weapon_dmr:
        case ItemID::mp_weapon_dmr_whiteset:
        case ItemID::mp_weapon_dmr_blueset:
        case ItemID::mp_weapon_dmr_purpleset:
        case ItemID::mp_weapon_dmr_gold: return 237; // Longbow
        case ItemID::mp_weapon_defender:
        case ItemID::mp_weapon_defender_whiteset:
        case ItemID::mp_weapon_defender_blueset:
        case ItemID::mp_weapon_defender_purpleset:
        case ItemID::mp_weapon_defender_gold: return 79; // Charge Rifle
        case ItemID::mp_weapon_lmg:
        case ItemID::mp_weapon_lmg_whiteset:
        case ItemID::mp_weapon_lmg_blueset:
        case ItemID::mp_weapon_lmg_purpleset:
        case ItemID::mp_weapon_lmg_gold: return 242; // Spitfire
        case ItemID::mp_weapon_rspn101:
        case ItemID::mp_weapon_rspn101_whiteset:
        case ItemID::mp_weapon_rspn101_blueset:
        case ItemID::mp_weapon_rspn101_purpleset:
        case ItemID::mp_weapon_rspn101_gold: return 359; // R-301
        case ItemID::mp_weapon_shotgun:
        case ItemID::mp_weapon_shotgun_crate:
        case ItemID::mp_weapon_shotgun_whiteset:
        case ItemID::mp_weapon_shotgun_blueset:
        case ItemID::mp_weapon_shotgun_purpleset:
        case ItemID::mp_weapon_shotgun_gold: return 132; // EVA-8
        case ItemID::mp_weapon_energy_shotgun:
        case ItemID::mp_weapon_energy_shotgun_crate:
        case ItemID::mp_weapon_energy_shotgun_whiteset:
        case ItemID::mp_weapon_energy_shotgun_blueset:
        case ItemID::mp_weapon_energy_shotgun_purpleset:
        case ItemID::mp_weapon_energy_shotgun_gold: return 310; // Peacekeeper
        case ItemID::mp_weapon_shotgun_pistol:
        case ItemID::mp_weapon_shotgun_pistol_whiteset:
        case ItemID::mp_weapon_shotgun_pistol_blueset:
        case ItemID::mp_weapon_shotgun_pistol_purpleset:
        case ItemID::mp_weapon_shotgun_pistol_gold:
        case ItemID::mp_weapon_shotgun_pistol_akimbo_whiteset:
        case ItemID::mp_weapon_shotgun_pistol_akimbo_blueset:
        case ItemID::mp_weapon_shotgun_pistol_akimbo_purpleset:
        case ItemID::mp_weapon_shotgun_pistol_akimbo_gold:
        case ItemID::mp_weapon_shotgun_pistol_light:
        case ItemID::mp_weapon_shotgun_pistol_heavy:
        case ItemID::mp_weapon_shotgun_pistol_sniper:
        case ItemID::mp_weapon_shotgun_pistol_energy: return 277; // Mozambique
        case ItemID::mp_weapon_wingman:
        case ItemID::mp_weapon_wingman_crate:
        case ItemID::mp_weapon_wingman_whiteset:
        case ItemID::mp_weapon_wingman_blueset:
        case ItemID::mp_weapon_wingman_purpleset:
        case ItemID::mp_weapon_wingman_gold: return 557; // Wingman
        case ItemID::mp_weapon_semipistol:
        case ItemID::mp_weapon_semipistol_whiteset:
        case ItemID::mp_weapon_semipistol_blueset:
        case ItemID::mp_weapon_semipistol_purpleset:
        case ItemID::mp_weapon_semipistol_gold:
        case ItemID::mp_weapon_semipistol_crate:
        case ItemID::mp_weapon_semipistol_crate_akimbo:
        case ItemID::mp_weapon_semipistol_akimbo_whiteset:
        case ItemID::mp_weapon_semipistol_akimbo_blueset:
        case ItemID::mp_weapon_semipistol_akimbo_purpleset:
        case ItemID::mp_weapon_semipistol_akimbo_gold: return 303; // P2020
        case ItemID::mp_weapon_autopistol:
        case ItemID::mp_weapon_autopistol_crate:
        case ItemID::mp_weapon_autopistol_fusion:
        case ItemID::mp_weapon_autopistol_fusion_whiteset:
        case ItemID::mp_weapon_autopistol_fusion_blueset:
        case ItemID::mp_weapon_autopistol_fusion_purpleset:
        case ItemID::mp_weapon_autopistol_fusion_gold:
        case ItemID::mp_weapon_autopistol_whiteset:
        case ItemID::mp_weapon_autopistol_blueset:
        case ItemID::mp_weapon_autopistol_purpleset:
        case ItemID::mp_weapon_autopistol_gold: return 373; // RE-45
        case ItemID::mp_weapon_sentinel:
        case ItemID::mp_weapon_sentinel_whiteset:
        case ItemID::mp_weapon_sentinel_blueset:
        case ItemID::mp_weapon_sentinel_purpleset:
        case ItemID::mp_weapon_sentinel_gold: return 436; // Sentinel
        case ItemID::mp_weapon_bow:
        case ItemID::mp_weapon_bow_crate:
        case ItemID::mp_weapon_bow_cupid:
        case ItemID::mp_weapon_bow_whiteset:
        case ItemID::mp_weapon_bow_blueset:
        case ItemID::mp_weapon_bow_purpleset:
        case ItemID::mp_weapon_bow_gold: return 61; // Bocek
        case ItemID::mp_weapon_3030:
        case ItemID::mp_weapon_3030_whiteset:
        case ItemID::mp_weapon_3030_blueset:
        case ItemID::mp_weapon_3030_purpleset:
        case ItemID::mp_weapon_3030_gold: return 6; // 30-30
        case ItemID::mp_weapon_nemesis:
        case ItemID::mp_weapon_nemesis_whiteset:
        case ItemID::mp_weapon_nemesis_blueset:
        case ItemID::mp_weapon_nemesis_purpleset:
        case ItemID::mp_weapon_nemesis_gold: return 288; // Nemesis
        case ItemID::mp_weapon_dragon_lmg:
        case ItemID::mp_weapon_dragon_lmg_crate:
        case ItemID::mp_weapon_dragon_lmg_whiteset:
        case ItemID::mp_weapon_dragon_lmg_blueset:
        case ItemID::mp_weapon_dragon_lmg_purpleset:
        case ItemID::mp_weapon_dragon_lmg_gold: return 363; // Rampage
        case ItemID::mp_weapon_car:
        case ItemID::mp_weapon_car_crate:
        case ItemID::mp_weapon_car_whiteset:
        case ItemID::mp_weapon_car_blueset:
        case ItemID::mp_weapon_car_purpleset:
        case ItemID::mp_weapon_car_gold: return 67; // C.A.R. SMG

        // Ammo
        case ItemID::bullet: return 233; // Light Ammo
        case ItemID::special: return 130; // Energy Ammo
        case ItemID::shotgun: return 445; // Shotgun Ammo
        case ItemID::highcal: return 188; // Heavy Ammo
        case ItemID::sniper: return 459; // Sniper Ammo
        case ItemID::arrows:
        case ItemID::arrowsBundle: return 29; // Arrows

        // Health/Regen
        case ItemID::health_pickup_ultimate: return 532; // Ult Accelerant
        case ItemID::health_pickup_combo_full: return 325; // Phoenix Kit
        case ItemID::health_pickup_health_large: return 265; // Med Kit
        case ItemID::health_pickup_health_small: return 485; // Syringe
        case ItemID::health_pickup_combo_large: return 442; // Shield Battery
        case ItemID::health_pickup_combo_small: return 443; // Shield Cell

        // Gear
        case ItemID::helmet_pickup_lv1:
        case ItemID::helmet_pickup_lv2:
        case ItemID::helmet_pickup_lv3:
        case ItemID::helmet_pickup_lv4_abilities:
        case ItemID::helmet_pickup_lv4_armor_upgrade: return 191; // Helmet
        case ItemID::armor_pickup_lv1:
        case ItemID::armor_pickup_lv2:
        case ItemID::armor_pickup_lv3:
        case ItemID::armor_pickup_lv4_all_fast:
        case ItemID::armor_pickup_lv0_evolving:
        case ItemID::armor_pickup_lv1_evolving:
        case ItemID::armor_pickup_lv2_evolving:
        case ItemID::armor_pickup_lv3_evolving:
        case ItemID::armor_pickup_lv5_evolving: return 138; // Evo Shield
        case ItemID::armor_core_pickup_lv1:
        case ItemID::armor_core_pickup_lv2:
        case ItemID::armor_core_pickup_lv3:
        case ItemID::armor_core_pickup_lv5: return 62; // Body Shield (Armor Core)
        case ItemID::incapshield_pickup_lv1:
        case ItemID::incapshield_pickup_lv2:
        case ItemID::incapshield_pickup_lv3:
        case ItemID::incapshield_pickup_lv4_selfrevive: return 218; // Knockdown Shield
        case ItemID::backpack_pickup_lv1: return 37;
        case ItemID::backpack_pickup_lv2: return 38;
        case ItemID::backpack_pickup_lv3:
        case ItemID::backpack_pickup_lv4_revive_boost: return 39;

        // Throwables
        case ItemID::mp_weapon_thermite_grenade: return 491;
        case ItemID::mp_weapon_frag_grenade: return 162;
        case ItemID::mp_weapon_grenade_emp: return 25; // Arc Star

        // Miscellaneous
        case ItemID::mp_ability_mobile_respawn_beacon: return 272;
        case ItemID::mp_ability_void_ring: return 186; // Heat Shield
        case ItemID::mp_ability_redeploy_balloon: return 133; // Evac Tower
        case ItemID::treasure_box: return 526;
        case ItemID::vault_key: return 540;
    }

    // Default icon per category if specific ID not handled
    switch (category) {
        case ItemCategory::WEAPON: return 360; // R99 as default weapon icon
        case ItemCategory::AMMO: return 188; // Heavy ammo as default
        case ItemCategory::COMMON:
        case ItemCategory::RARE:
        case ItemCategory::EPIC:
        case ItemCategory::LEGENDARY:
        case ItemCategory::MYTHIC: return 22; // Apex pack as default loot
        default: return -1;
    }
}
