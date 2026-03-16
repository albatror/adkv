#include "ItemManager.h"
#include "Game.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cstring>

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

static std::unordered_map<std::string, int> modelIndexCache;
static std::unordered_map<std::string, std::chrono::steady_clock::time_point> lastScanTime;

int ItemManager::GetModelIndex(const char* partialModelName) {
    extern Memory apex_mem;
    extern uint64_t g_Base;

    auto now = std::chrono::steady_clock::now();

    if (modelIndexCache.count(partialModelName)) {
        int idx = modelIndexCache[partialModelName];
        if (idx != -1) return idx;

        // If not found, only retry every 10 seconds to save performance
        if (std::chrono::duration_cast<std::chrono::seconds>(now - lastScanTime[partialModelName]).count() < 10) {
            return -1;
        }
    }

    lastScanTime[partialModelName] = now;

    for (int i = 0; i < 2000; i++) {
        uint64_t entity = 0;
        if (!apex_mem.Read<uint64_t>(g_Base + OFFSET_ENTITYLIST + (i << 5), entity)) continue;
        if (entity == 0) continue;

        char modelName[256] = { 0 };
        uint64_t model_name_ptr = 0;
        if (!apex_mem.Read<uint64_t>(entity + OFFSET_MODELNAME, model_name_ptr)) continue;
        if (model_name_ptr) {
            apex_mem.ReadArray<char>(model_name_ptr, modelName, 256);
            if (strstr(modelName, partialModelName)) {
                int modelIndex = 0;
                apex_mem.Read<int>(entity + OFFSET_MODEL_INDEX, modelIndex);
                modelIndexCache[partialModelName] = modelIndex;
                return modelIndex;
            }
        }
    }
    modelIndexCache[partialModelName] = -1;
    return -1;
}

void ItemManager::ResetModelCache() {
    modelIndexCache.clear();
}
