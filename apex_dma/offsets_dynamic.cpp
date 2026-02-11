#include "offsets_dynamic.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <sstream>
#include <regex>
#include <vector>
#include <ctime>
#include <iomanip>

DynamicOffsets offsets = {0};

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
            try {
                data[key] = std::stoull(value, nullptr, 16);
            } catch (...) {}
        }
    }
    return data;
}

bool load_offsets_from_ini(const char* offsets_file, const char* convars_file, const char* buttons_file) {
    auto off_data = read_ini(offsets_file);
    auto con_data = read_ini(convars_file);
    auto but_data = read_ini(buttons_file);

    if (off_data.empty()) return false;

    // Mapping based on r5dumper output
    if (off_data.count("[.Miscellaneous]cl_entitylist")) offsets.EntityList = off_data["[.Miscellaneous]cl_entitylist"];
    if (off_data.count("[.Miscellaneous]LocalPlayer")) offsets.LocalEnt = off_data["[.Miscellaneous]LocalPlayer"];
    if (off_data.count("[.Miscellaneous]LocalEntityHandle")) offsets.LocalEntityHandle = off_data["[.Miscellaneous]LocalEntityHandle"];
    if (off_data.count("[.Miscellaneous]NameList")) offsets.NameList = off_data["[.Miscellaneous]NameList"];
    if (off_data.count("[.Miscellaneous]GlobalVars")) offsets.GlobalVars = off_data["[.Miscellaneous]GlobalVars"];
    if (off_data.count("[.Miscellaneous]LevelName")) offsets.LevelName = off_data["[.Miscellaneous]LevelName"];
    if (off_data.count("[.Miscellaneous]ViewRender")) offsets.Render = off_data["[.Miscellaneous]ViewRender"];
    if (off_data.count("[.Miscellaneous]ViewMatrix")) offsets.Matrix = off_data["[.Miscellaneous]ViewMatrix"];
    if (off_data.count("[.Miscellaneous]CPlayer!camera_origin")) offsets.CameraPos = off_data["[.Miscellaneous]CPlayer!camera_origin"];
    if (off_data.count("[.Miscellaneous]CBaseAnimating!m_pStudioHdr")) offsets.StudioHdr = off_data["[.Miscellaneous]CBaseAnimating!m_pStudioHdr"];
    if (off_data.count("[.Miscellaneous]InputSystem")) offsets.InputSystem = off_data["[.Miscellaneous]InputSystem"];
    if (off_data.count("[.Miscellaneous]HighlightSettings")) offsets.HighlightSettings = off_data["[.Miscellaneous]HighlightSettings"];

    if (off_data.count("[RecvTable.DT_BaseEntity]m_iTeamNum")) offsets.Team = off_data["[RecvTable.DT_BaseEntity]m_iTeamNum"];
    if (off_data.count("[RecvTable.DT_Player]m_iHealth")) offsets.Health = off_data["[RecvTable.DT_Player]m_iHealth"];
    if (off_data.count("[RecvTable.DT_Player]m_iMaxHealth")) offsets.MaxHealth = off_data["[RecvTable.DT_Player]m_iMaxHealth"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_shieldHealth")) offsets.Shield = off_data["[RecvTable.DT_BaseEntity]m_shieldHealth"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_shieldHealthMax")) offsets.MaxShield = off_data["[RecvTable.DT_BaseEntity]m_shieldHealthMax"];
    if (off_data.count("[RecvTable.DT_Player]m_armorType")) offsets.Armortype = off_data["[RecvTable.DT_Player]m_armorType"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_iName")) offsets.Name = off_data["[RecvTable.DT_BaseEntity]m_iName"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_iSignifierName")) offsets.SignName = off_data["[RecvTable.DT_BaseEntity]m_iSignifierName"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_vecAbsVelocity")) offsets.AbsVelocity = off_data["[DataMap.DT_BaseEntity]m_vecAbsVelocity"];
    if (off_data.count("[.Miscellaneous]CPlayer!lastVisibleTime")) offsets.VisibleTime = off_data["[.Miscellaneous]CPlayer!lastVisibleTime"];
    if (off_data.count("[RecvTable.DT_Player]m_bZooming")) offsets.Zooming = off_data["[RecvTable.DT_Player]m_bZooming"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_currentFrame.viewOffset")) offsets.ViewOffset = off_data["[DataMap.DT_BaseEntity]m_currentFrame.viewOffset"];

    if (off_data.count("[RecvTable.DT_Player]m_duckState")) offsets.InDuckState = off_data["[RecvTable.DT_Player]m_duckState"];
    if (off_data.count("[DataMap.DT_Player]m_traversalProgress")) offsets.TraversalProgress = off_data["[DataMap.DT_Player]m_traversalProgress"];
    if (off_data.count("[DataMap.DT_Player]m_traversalStartTime")) offsets.TraversalStartTime = off_data["[DataMap.DT_Player]m_traversalStartTime"];

    if (off_data.count("[RecvTable.DT_Player]m_lifeState")) offsets.LifeState = off_data["[RecvTable.DT_Player]m_lifeState"];
    if (off_data.count("[RecvTable.DT_Player]m_bleedoutState")) offsets.BleedOutState = off_data["[RecvTable.DT_Player]m_bleedoutState"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_vecAbsOrigin")) offsets.Origin = off_data["[DataMap.DT_BaseEntity]m_vecAbsOrigin"];
    if (off_data.count("[RecvTable.DT_BaseAnimating]m_nForceBone")) offsets.Bones = off_data["[RecvTable.DT_BaseAnimating]m_nForceBone"] + 0x48;
    if (off_data.count("[DataMap.DT_Player]m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle")) offsets.AimPunch = off_data["[DataMap.DT_Player]m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle"];

    // ViewAngles is tricky, often derived
    // if (off_data.count("[.Miscellaneous]ViewAngles")) offsets.ViewAngles = off_data["[.Miscellaneous]ViewAngles"];

    if (but_data.count("[Buttons]in_duck")) offsets.InDuck = but_data["[Buttons]in_duck"];
    if (but_data.count("[Buttons]in_jump")) offsets.InJump = but_data["[Buttons]in_jump"];
    if (but_data.count("[Buttons]in_attack")) offsets.InAttack = but_data["[Buttons]in_attack"];
    if (but_data.count("[Buttons]in_zoom")) offsets.InZoom = but_data["[Buttons]in_zoom"];
    if (but_data.count("[Buttons]in_forward")) offsets.InForward = but_data["[Buttons]in_forward"];
    if (but_data.count("[Buttons]in_backward")) offsets.InBackward = but_data["[Buttons]in_backward"];
    if (but_data.count("[Buttons]in_use")) offsets.InUse = but_data["[Buttons]in_use"];
    if (but_data.count("[Buttons]in_toggle_duck")) offsets.InToggleDuck = but_data["[Buttons]in_toggle_duck"];

    if (con_data.count("[ConVars]host_map")) offsets.HostMap = con_data["[ConVars]host_map"] + 0x58;

    printf("Offsets updated from INI files.\n");
    return true;
}

void update_offsets_files(const char* offsets_file, const char* convars_file, const char* buttons_file) {
    std::map<std::string, uint64_t> new_offsets;
    auto load_to_map = [&](const char* filename) {
        std::ifstream file(filename);
        if (!file.is_open()) return;
        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#' || line[0] == ';') continue;
            std::stringstream ss(line);
            std::string key, val_str;
            if (ss >> key >> val_str) {
                std::string norm_key = key;
                if (norm_key.size() > 2 && norm_key[0] == '[' && norm_key[1] == '.') {
                    norm_key.erase(1, 1);
                }
                try {
                    new_offsets[norm_key] = std::stoull(val_str, nullptr, 16);
                } catch (...) {}
            }
        }
    };
    load_to_map(offsets_file);
    load_to_map(convars_file);
    load_to_map(buttons_file);

    if (new_offsets.empty()) {
        printf("No offsets found in extracted INI files to update.\n");
        return;
    }

    auto t = std::time(nullptr);
    struct tm tm;
    localtime_r(&t, &tm);
    char date_buf[64];
    std::strftime(date_buf, sizeof(date_buf), "%Y/%m/%d", &tm);
    std::string current_date = date_buf;

    // 1. Update offsets.ini
    {
        std::ifstream ini_in("offsets.ini");
        if (ini_in.is_open()) {
            std::vector<std::string> lines;
            std::string line, current_section;
            while (std::getline(ini_in, line)) {
                std::string trimmed = line;
                trimmed.erase(0, trimmed.find_first_not_of(" \t"));
                trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
                if (!trimmed.empty() && trimmed[0] == '[' && trimmed.back() == ']') {
                    current_section = trimmed;
                } else if (!current_section.empty() && line.find('=') != std::string::npos) {
                    size_t eq_pos = line.find('=');
                    std::string key = line.substr(0, eq_pos);
                    // trim key
                    size_t first = key.find_first_not_of(" \t");
                    if (first != std::string::npos) {
                        size_t last = key.find_last_not_of(" \t");
                        key = key.substr(first, (last - first + 1));
                    }
                    std::string full_key = current_section + key;
                    if (new_offsets.count(full_key)) {
                        std::stringstream ss;
                        ss << key << "=0x" << std::hex << new_offsets[full_key];
                        line = ss.str();
                    }
                }
                lines.push_back(line);
            }
            ini_in.close();
            std::ofstream ini_out("offsets.ini");
            for (const auto& l : lines) ini_out << l << "\n";
            printf("Updated offsets.ini\n");
        }
    }

    // 2. Update offsets.h
    {
        std::ifstream h_in("offsets.h");
        if (h_in.is_open()) {
            std::vector<std::string> lines;
            std::string line;
            while (std::getline(h_in, line)) {
                if (line.find("#define") != std::string::npos && line.find("//") != std::string::npos) {
                    size_t comment_pos = line.find("//");
                    std::string comment = line.substr(comment_pos);
                    size_t b1 = comment.find('[');
                    size_t b2 = comment.find(']');
                    if (b1 != std::string::npos && b2 != std::string::npos) {
                        std::string section = comment.substr(b1, b2 - b1 + 1);
                        std::string rest = comment.substr(b2 + 1);
                        if (!rest.empty() && rest[0] == '.') rest.erase(0, 1);
                        std::string key;
                        std::stringstream ss_rest(rest);
                        if (ss_rest >> key) {
                            size_t end_key = key.find_first_of(" \t+*-");
                            if (end_key != std::string::npos) key = key.substr(0, end_key);

                            std::string full_key = section + key;
                            if (new_offsets.count(full_key)) {
                                std::regex hex_regex("0x[0-9a-fA-F]+");
                                std::stringstream new_val;
                                new_val << "0x" << std::hex << new_offsets[full_key];
                                line = std::regex_replace(line, hex_regex, new_val.str(), std::regex_constants::format_first_only);

                                std::regex date_regex("\\d{4}/\\d{2}/\\d{2}");
                                line = std::regex_replace(line, date_regex, current_date);
                            }
                        }
                    }
                }
                lines.push_back(line);
            }
            h_in.close();
            std::ofstream h_out("offsets.h");
            for (const auto& l : lines) h_out << l << "\n";
            printf("Updated offsets.h\n");
        }
    }
}
