#pragma once
#include <string>
#include <vector>
#include <unordered_map>

#define COMMON_GLOW_ID                  67 //[] Common loot
#define RARE_GLOW_ID                    55 //[] Rare loot
#define EPIC_GLOW_ID                    48 //[] Epic loot
#define MYTHIC_GLOW_ID                  43 //[] Mythic loot
#define LEGENDARY_GLOW_ID               16 //[] Legendary loot
#define WEAPON_GLOW_ID                  9 //[] Weapon loot
#define AMMO_GLOW_ID                    59 //[] Ammo loot

enum class ItemCategory {
    OTHER,
    COMMON,
    RARE,
    EPIC,
    LEGENDARY,
    MYTHIC,
    WEAPON,
    AMMO
};

struct ItemDef {
    std::string name;
    int glowId;
    std::string model;
    ItemCategory category;
};

class ItemManager {
public:
    static ItemManager& getInstance() {
        static ItemManager instance;
        return instance;
    }

    void LoadItems(const std::string& path);
    void LoadWeapons(const std::string& path);
    bool GetItemInfo(const std::string& modelName, std::string& name, ItemCategory& category);
    bool GetItemInfoByID(int id, std::string& name, ItemCategory& category);
    int GetModelIndex(const std::string& search_str);
    void ResetModelCache();

private:
    ItemManager();
    ItemCategory DetermineCategory(const std::string& name, int glowId);
    std::vector<ItemDef> itemDefs;
    std::unordered_map<std::string, std::pair<std::string, ItemCategory>> cache;
    std::unordered_map<std::string, int> modelIndexCache;
};
