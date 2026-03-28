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

int ItemManager::GetModelIndex(const std::string& search_str) {
    auto it = modelIndexCache.find(search_str);
    if (it != modelIndexCache.end()) {
        return it->second;
    }

    extern uint64_t g_Base;
    extern Memory apex_mem;
    uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

    for (int i = 0; i < 2000; i++) {
        uint64_t centity = 0;
        apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
        if (centity == 0) continue;

        uint64_t m_ptr = 0;
        apex_mem.Read<uint64_t>(centity + OFFSET_MODELNAME, m_ptr);
        if (m_ptr == 0) continue;

        char modelName[256] = { 0 };
        apex_mem.ReadArray<char>(m_ptr, modelName, 256);
        std::string model_str = std::string(modelName);

        if (model_str.find(search_str) != std::string::npos) {
            int modelIndex = 0;
            apex_mem.Read<int>(centity + OFFSET_MODEL_INDEX, modelIndex);
            if (modelIndex != 0) {
                modelIndexCache[search_str] = modelIndex;
                return modelIndex;
            }
        }
    }

    return 0;
}

void ItemManager::ResetModelCache() {
    modelIndexCache.clear();
}
