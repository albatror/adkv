#include "offsets_dynamic.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <vector>

DynamicOffsets offsets = {0};

void initialize_offsets() {
    offsets.EntityList = 0x644bf38;
    offsets.LocalEnt = 0x282c198;
    offsets.LocalEntityHandle = 0x2017c68;
    offsets.NameList = 0x8e46440;
    offsets.GlobalVars = 0x1f7a5b0;
    offsets.LevelName = 0x1f7aa34;
    offsets.ClientState = offsets.LevelName - 0x1c0;
    offsets.SignonState = offsets.ClientState + 0xac;
    offsets.HostMap = 0x01e93dd0 + 0x58;
    offsets.Team = 0x0334;
    offsets.Health = 0x0324;
    offsets.MaxHealth = 0x0468;
    offsets.Shield = 0x01a0;
    offsets.MaxShield = 0x1A4;
    offsets.Armortype = 0x4814;
    offsets.Name = 0x0479;
    offsets.SignName = 0x0470;
    offsets.AbsVelocity = 0x0170;
    offsets.VisibleTime = 0x1a64;
    offsets.LastAimedAtTime = 0x1a5c;
    offsets.Zooming = 0x1cb1;
    offsets.ViewOffset = 0x00e8;
    offsets.ActiveWeapon = 0x1958 + 0x0058;
    offsets.ObserverList = 0x644df58 + 0x20C8;
    offsets.ObserverArray = 0x954;
    offsets.InDuckState = 0x2abc;
    offsets.InDuck = 0x03f39378;
    offsets.TraversalProgress = 0x2bcc;
    offsets.TraversalStartTime = 0x2bd4;
    offsets.TraversalReleaseTime = 0x2bdc;
    offsets.InJump = 0x3f39270;
    offsets.InToggleDuck = 0x03f391a8;
    offsets.WeaponName = 0x15f0;
    offsets.OffWeapon = 0x19d4;
    offsets.WallRunStartTime = 0x370c;
    offsets.WallRunClearTime = 0x3710;
    offsets.Flags = 0x00c8;
    offsets.InAttack = 0x03f389e8;
    offsets.InZoom = 0x03f39300;
    offsets.InForward = 0x03f393b8;
    offsets.InBackward = 0x03f393e0;
    offsets.LifeState = 0x0690;
    offsets.BleedOutState = 0x27c0;
    offsets.Origin = 0x017c;
    offsets.Bones = 0x0db8 + 0x48;
    offsets.StudioHdr = 0x1000;
    offsets.AimPunch = 0x2438;
    offsets.CameraPos = 0x1fc4;
    offsets.ViewAngles = 0x2614 - 0x14;
    offsets.BreathAngles = offsets.ViewAngles - 0x10;
    offsets.ObserverMode = 0x35e4;
    offsets.ObservingTarget = 0x35f0;
    offsets.InUse = 0x03f392f0;
    offsets.Matrix = 0x11a350;
    offsets.Render = 0x3f360f0;
    offsets.Weapon = 0x19c4;
    offsets.BulletSpeed = 0x2860;
    offsets.BulletScale = offsets.BulletSpeed + 0x8;
    offsets.ZoomFov = 0x1650 + 0xb8;
    offsets.Ammo = 0x15e0;
    offsets.ItemId = 0x15e4;
    offsets.ModelName = 0x0030;
    offsets.Yaw = 0x231c - 0x8;
    offsets.GlowEnable = 0x0299 - 0x1;
    offsets.GlowThroughWalls = 0x26c - 0x21;
    offsets.TimeBase = 0x2158;
    offsets.HighlightSettings = 0x6b95c40;
    offsets.InputSystem = 0x2034400;
    offsets.SkydiveState = 0x4878;
    offsets.GrappleActive = 0x2da8;
    offsets.Grapple = 0x2d20;
    offsets.GrappleAttached = 0x48;
    offsets.m_xp = 0x3824;
    offsets.Grade = 0x0348;
    offsets.CrosshairLastTargetTime = offsets.VisibleTime + 0x19f8;
}

static std::map<std::string, uint64_t> read_ini(const char* filename) {
    std::map<std::string, uint64_t> data;
    std::ifstream file(filename);
    if (!file.is_open()) return data;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;
        std::stringstream ss(line);
        std::string key, value;
        if (ss >> key >> value) {
            try { data[key] = std::stoull(value, nullptr, 16); } catch (...) {}
        }
    }
    return data;
}

static std::string normalize_key(std::string key) {
    std::string normalized = key;
    normalized.erase(std::remove(normalized.begin(), normalized.end(), '.'), normalized.end());
    if (normalized.find("[Miscellaneous]") == 0) {
        normalized = "[.Miscellaneous]" + normalized.substr(15);
    }
    return normalized;
}

static uint64_t fuzzy_lookup(const std::map<std::string, uint64_t>& data, std::string key) {
    if (data.count(key)) return data.at(key);
    std::string n_key = normalize_key(key);
    if (data.count(n_key)) return data.at(n_key);
    size_t bracket_pos = n_key.find(']');
    if (bracket_pos != std::string::npos) {
        std::string section = n_key.substr(0, bracket_pos + 1);
        std::string field = n_key.substr(bracket_pos + 1);
        if (field.find("m_") == 0) {
            std::string alt_field = section + field.substr(2);
            if (data.count(alt_field)) return data.at(alt_field);
        } else {
            std::string alt_field = section + "m_" + field;
            if (data.count(alt_field)) return data.at(alt_field);
        }
    }
    return 0;
}

bool load_offsets_from_ini(const char* offsets_file, const char* convars_file, const char* buttons_file) {
    auto off_data = read_ini(offsets_file);
    auto con_data = read_ini(convars_file);
    auto but_data = read_ini(buttons_file);
    if (off_data.empty()) return false;
    offsets.EntityList = fuzzy_lookup(off_data, "[.Miscellaneous]cl_entitylist");
    offsets.LocalEnt = fuzzy_lookup(off_data, "[.Miscellaneous]LocalPlayer");
    offsets.LocalEntityHandle = fuzzy_lookup(off_data, "[.Miscellaneous]LocalEntityHandle");
    offsets.NameList = fuzzy_lookup(off_data, "[.Miscellaneous]NameList");
    offsets.GlobalVars = fuzzy_lookup(off_data, "[.Miscellaneous]GlobalVars");
    offsets.LevelName = fuzzy_lookup(off_data, "[.Miscellaneous]LevelName");
    offsets.Render = fuzzy_lookup(off_data, "[.Miscellaneous]ViewRender");
    offsets.Matrix = fuzzy_lookup(off_data, "[.Miscellaneous]ViewMatrix");
    offsets.CameraPos = fuzzy_lookup(off_data, "[.Miscellaneous]CPlayer!camera_origin");
    offsets.StudioHdr = fuzzy_lookup(off_data, "[.Miscellaneous]CBaseAnimating!m_pStudioHdr");
    offsets.InputSystem = fuzzy_lookup(off_data, "[.Miscellaneous]InputSystem");
    offsets.HighlightSettings = fuzzy_lookup(off_data, "[.Miscellaneous]HighlightSettings");
    offsets.Team = fuzzy_lookup(off_data, "[RecvTable.DT_BaseEntity]m_iTeamNum");
    offsets.Health = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_iHealth");
    offsets.MaxHealth = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_iMaxHealth");
    offsets.Shield = fuzzy_lookup(off_data, "[RecvTable.DT_BaseEntity]m_shieldHealth");
    offsets.MaxShield = fuzzy_lookup(off_data, "[RecvTable.DT_BaseEntity]m_shieldHealthMax");
    offsets.Armortype = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_armorType");
    offsets.Name = fuzzy_lookup(off_data, "[RecvTable.DT_BaseEntity]m_iName");
    offsets.SignName = fuzzy_lookup(off_data, "[RecvTable.DT_BaseEntity]m_iSignifierName");
    offsets.AbsVelocity = fuzzy_lookup(off_data, "[DataMap.DT_BaseEntity]m_vecAbsVelocity");
    uint64_t lastVis = fuzzy_lookup(off_data, "[.Miscellaneous]lastVisibleTime_-2");
    if (lastVis) offsets.VisibleTime = lastVis + 2;
    else offsets.VisibleTime = fuzzy_lookup(off_data, "[.Miscellaneous]CPlayer!lastVisibleTime");
    offsets.TimeBase = fuzzy_lookup(off_data, "[DataMap.C_Player]m_currentFramePlayer.timeBase");
    if (!offsets.TimeBase) offsets.TimeBase = fuzzy_lookup(off_data, "[DataMap.DT_Player]m_currentFramePlayer.timeBase");
    offsets.Zooming = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_bZooming");
    offsets.ViewOffset = fuzzy_lookup(off_data, "[DataMap.DT_BaseEntity]m_currentFrame.viewOffset");
    offsets.InDuckState = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_duckState");
    offsets.TraversalProgress = fuzzy_lookup(off_data, "[DataMap.DT_Player]m_traversalProgress");
    offsets.TraversalStartTime = fuzzy_lookup(off_data, "[DataMap.DT_Player]m_traversalStartTime");
    offsets.LifeState = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_lifeState");
    offsets.BleedOutState = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_bleedoutState");
    offsets.Origin = fuzzy_lookup(off_data, "[DataMap.DT_BaseEntity]m_vecAbsOrigin");
    offsets.Bones = fuzzy_lookup(off_data, "[RecvTable.DT_BaseAnimating]m_nForceBone") + 0x48;
    offsets.AimPunch = fuzzy_lookup(off_data, "[DataMap.DT_Player]m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle");
    offsets.ViewAngles = fuzzy_lookup(off_data, "[.Miscellaneous]ViewAngles");
    if (offsets.ViewAngles) offsets.BreathAngles = offsets.ViewAngles - 0x10;
    offsets.ObserverMode = fuzzy_lookup(off_data, "[RecvTable.DT_LocalPlayerExclusive]m_iObserverMode");
    offsets.ObservingTarget = fuzzy_lookup(off_data, "[RecvTable.DT_LocalPlayerExclusive]m_hObserverTarget");
    offsets.Weapon = fuzzy_lookup(off_data, "[RecvTable.DT_BaseCombatCharacter]m_latestPrimaryWeapons");
    offsets.BulletSpeed = fuzzy_lookup(off_data, "[.Miscellaneous]CWeaponX!m_flProjectileSpeed");
    if (offsets.BulletSpeed) offsets.BulletScale = offsets.BulletSpeed + 0x8;
    uint64_t weaponData = fuzzy_lookup(off_data, "[RecvTable.DT_WeaponX]m_playerData");
    if (weaponData) offsets.ZoomFov = weaponData + 0xb8;
    offsets.Ammo = fuzzy_lookup(off_data, "[RecvTable.DT_PropSurvival]m_ammoInClip");
    offsets.ItemId = fuzzy_lookup(off_data, "[RecvTable.DT_PropSurvival]m_customScriptInt");
    offsets.ModelName = fuzzy_lookup(off_data, "[DataMap.DT_BaseEntity]m_ModelName");
    uint64_t ammoPool = fuzzy_lookup(off_data, "[DataMap.DT_Player]m_currentFramePlayer.m_ammoPoolCount");
    if (ammoPool) offsets.Yaw = ammoPool - 0x8;
    uint64_t highlight = fuzzy_lookup(off_data, "[RecvTable.DT_HighlightSettings]m_highlightGenericContexts");
    if (highlight) {
        offsets.GlowEnable = highlight - 0x1;
        offsets.GlowThroughWalls = highlight - 0x21;
    }
    offsets.SkydiveState = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_skydiveState");
    offsets.GrappleActive = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_grappleActive");
    offsets.Grapple = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_grapple");
    offsets.GrappleAttached = fuzzy_lookup(off_data, "[RecvTable.DT_GrappleData]m_grappleAttached");
    offsets.m_xp = fuzzy_lookup(off_data, "[RecvTable.DT_Player]m_xp");
    offsets.Grade = fuzzy_lookup(off_data, "[RecvTable.DT_BaseEntity]m_grade");
    offsets.CrosshairLastTargetTime = fuzzy_lookup(off_data, "[.Miscellaneous]CWeaponX!lastCrosshairTargetTime");
    if (!offsets.CrosshairLastTargetTime) offsets.CrosshairLastTargetTime = offsets.VisibleTime + 0x19f8;
    offsets.InDuck = fuzzy_lookup(but_data, "[Buttons]+duck");
    if (!offsets.InDuck) offsets.InDuck = fuzzy_lookup(but_data, "[Buttons]in_duck");
    offsets.InJump = fuzzy_lookup(but_data, "[Buttons]+jump");
    if (!offsets.InJump) offsets.InJump = fuzzy_lookup(but_data, "[Buttons]in_jump");
    offsets.InAttack = fuzzy_lookup(but_data, "[Buttons]+attack");
    if (!offsets.InAttack) offsets.InAttack = fuzzy_lookup(but_data, "[Buttons]in_attack");
    offsets.InZoom = fuzzy_lookup(but_data, "[Buttons]+zoom");
    if (!offsets.InZoom) offsets.InZoom = fuzzy_lookup(but_data, "[Buttons]in_zoom");
    offsets.InForward = fuzzy_lookup(but_data, "[Buttons]+forward");
    if (!offsets.InForward) offsets.InForward = fuzzy_lookup(but_data, "[Buttons]in_forward");
    offsets.InBackward = fuzzy_lookup(but_data, "[Buttons]+backward");
    if (!offsets.InBackward) offsets.InBackward = fuzzy_lookup(but_data, "[Buttons]in_backward");
    offsets.InUse = fuzzy_lookup(but_data, "[Buttons]+use");
    if (!offsets.InUse) offsets.InUse = fuzzy_lookup(but_data, "[Buttons]in_use");
    offsets.InToggleDuck = fuzzy_lookup(but_data, "[Buttons]+toggle_duck");
    if (!offsets.InToggleDuck) offsets.InToggleDuck = fuzzy_lookup(but_data, "[Buttons]in_toggle_duck");
    offsets.HostMap = fuzzy_lookup(con_data, "[ConVars]host_map") + 0x58;
    printf("Offsets updated from INI files.\n");
    return true;
}

bool update_offsets_h(const char* h_file, const char* buttons_ini, const char* convars_ini, const char* offsets_ini) {
    auto off_data = read_ini(offsets_ini);
    auto con_data = read_ini(convars_ini);
    auto but_data = read_ini(buttons_ini);
    std::map<std::string, uint64_t> all_data;
    all_data.insert(off_data.begin(), off_data.end());
    all_data.insert(con_data.begin(), con_data.end());
    all_data.insert(but_data.begin(), but_data.end());
    std::ifstream file(h_file);
    if (!file.is_open()) return false;
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) lines.push_back(line);
    file.close();
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss_date;
    ss_date << std::put_time(std::localtime(&in_time_t), "%Y/%m/%d");
    std::string today = ss_date.str();
    std::ofstream out(h_file);
    if (!out.is_open()) return false;
    bool redef_zone = false;
    for (auto& l : lines) {
        if (l.find("// REDEFINITIONS") != std::string::npos) redef_zone = true;
        if (redef_zone) { out << l << "\n"; continue; }
        if (l.find("#define OFFSET_") == 0 || l.find("#define HIGHLIGHT_SETTINGS") == 0 || l.find("#define GLOW_") == 0 || l.find("#define GameVersion") == 0) {
            size_t comment_pos = l.find("//");
            if (comment_pos != std::string::npos) {
                size_t open_bracket = l.find("[", comment_pos);
                if (open_bracket != std::string::npos) {
                    size_t updated_pos = l.find("updated", open_bracket);
                    size_t space_pos = l.find(" ", open_bracket);
                    size_t key_end = space_pos;
                    if (updated_pos != std::string::npos && (key_end == std::string::npos || updated_pos < key_end)) key_end = updated_pos;
                    if (key_end == std::string::npos) key_end = l.length();
                    std::string full_key = l.substr(open_bracket, key_end - open_bracket);
                    size_t cb = full_key.find(']');
                    if (cb != std::string::npos && full_key.length() > cb + 1 && full_key[cb+1] == '.') full_key.erase(cb+1, 1);
                    uint64_t val = fuzzy_lookup(all_data, full_key);
                    std::string key = full_key;
                    std::string suffix = "";
                    if (val == 0) {
                        size_t last_math = full_key.find_last_of("+-");
                        if (last_math != std::string::npos && last_math > full_key.find(']')) {
                            key = full_key.substr(0, last_math);
                            suffix = full_key.substr(last_math);
                            key.erase(key.find_last_not_of(" \t") + 1);
                            val = fuzzy_lookup(all_data, key);
                        }
                    }
                    if (val != 0 || full_key.find("GameVersion") != std::string::npos) {
                        std::stringstream ss_line(l);
                        std::string define, name;
                        ss_line >> define >> name;
                        out << define << " " << name << " ";
                        if (full_key.find("GameVersion") != std::string::npos) out << "v3.0.3.24";
                        else {
                            size_t name_end = l.find(name) + name.length();
                            std::string val_part = l.substr(name_end, comment_pos - name_end);
                            if (val_part.find("OFFSET_") != std::string::npos || val_part.find("offsets.") != std::string::npos) {
                                val_part.erase(0, val_part.find_first_not_of(" \t"));
                                val_part.erase(val_part.find_last_not_of(" \t") + 1);
                                out << val_part;
                            } else {
                                char hex_val[32];
                                sprintf(hex_val, "0x%lx", val);
                                out << hex_val << " " << suffix;
                            }
                        }
                        std::string comment_content = l.substr(comment_pos + 2);
                        size_t up_pos = comment_content.find("updated");
                        if (up_pos != std::string::npos) comment_content = comment_content.substr(0, up_pos);
                        out << " //" << comment_content << "updated " << today << "\n";
                        continue;
                    }
                }
            }
        }
        out << l << "\n";
    }
    out.close();
    return true;
}

bool update_offsets_ini(const char* ini_file) {
    std::ofstream ini(ini_file);
    if (ini.is_open()) {
        ini << "[Miscellaneous]\n";
        ini << "cl_entitylist 0x" << std::hex << offsets.EntityList << "\n";
        ini << "LocalPlayer 0x" << std::hex << offsets.LocalEnt << "\n";
        ini << "GlobalVars 0x" << std::hex << offsets.GlobalVars << "\n";
        ini << "LevelName 0x" << std::hex << offsets.LevelName << "\n";
        ini << "ViewMatrix 0x" << std::hex << offsets.Matrix << "\n";
        ini << "ViewRender 0x" << std::hex << offsets.Render << "\n";
        ini << "\n[Buttons]\n";
        ini << "in_attack 0x" << std::hex << offsets.InAttack << "\n";
        ini << "in_jump 0x" << std::hex << offsets.InJump << "\n";
        ini << "in_duck 0x" << std::hex << offsets.InDuck << "\n";
        ini << "in_forward 0x" << std::hex << offsets.InForward << "\n";
        ini << "in_backward 0x" << std::hex << offsets.InBackward << "\n";
        ini << "in_use 0x" << std::hex << offsets.InUse << "\n";
        ini << "in_zoom 0x" << std::hex << offsets.InZoom << "\n";
        ini.close();
        return true;
    }
    return false;
}
