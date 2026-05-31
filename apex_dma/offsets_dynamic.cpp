#include "offsets_dynamic.h"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>

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
    if (off_data.count("[.Miscellaneous]observerList")) offsets.ObserverList = off_data["[.Miscellaneous]observerList"];
    if (off_data.count("[.Miscellaneous]observer_index")) offsets.ObserverArray = off_data["[.Miscellaneous]observer_index"];

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
    if (off_data.count("[RecvTable.DT_Player]m_mantleBoostState")) offsets.MantleBoostState = off_data["[RecvTable.DT_Player]m_mantleBoostState"];
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

bool merge_ini_files(const char* offsets_file, const char* convars_file, const char* buttons_file, const char* output_file) {
    std::ofstream out(output_file, std::ios::trunc);
    if (!out.is_open()) {
        printf("merge_ini_files: cannot open output %s\n", output_file);
        return false;
    }
    const char* sources[] = { offsets_file, convars_file, buttons_file };
    for (const char* src : sources) {
        std::ifstream in(src);
        if (!in.is_open()) {
            printf("merge_ini_files: cannot open %s\n", src);
            continue;
        }
        out << in.rdbuf() << "\n";
    }
    printf("merge_ini_files: combined offsets written to %s\n", output_file);
    return true;
}

bool update_offsets_h(const char* offsets_h_path, const char* offsets_file, const char* convars_file, const char* buttons_file) {
    auto off_data = read_ini(offsets_file);
    auto con_data = read_ini(convars_file);
    auto but_data = read_ini(buttons_file);

    if (off_data.empty()) {
        printf("update_offsets_h: offset data empty, aborting\n");
        return false;
    }

    std::ifstream f(offsets_h_path);
    if (!f.is_open()) {
        printf("update_offsets_h: cannot open %s\n", offsets_h_path);
        return false;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(f, line))
        lines.push_back(line);
    f.close();

    time_t now_t = time(nullptr);
    struct tm* tm_info = localtime(&now_t);
    char date_buf[16];
    strftime(date_buf, sizeof(date_buf), "%Y/%m/%d", tm_info);

    int updated_count = 0;
    for (auto& ln : lines) {
        if (ln.size() < 8 || ln.substr(0, 7) != "#define") continue;

        // Locate the value start (skip "#define NAME ")
        size_t name_start = ln.find(' ', 7);
        if (name_start == std::string::npos) continue;
        name_start = ln.find_first_not_of(' ', name_start + 1);
        if (name_start == std::string::npos) continue;

        size_t val_pos = ln.find(' ', name_start);
        if (val_pos == std::string::npos) continue;
        val_pos = ln.find_first_not_of(' ', val_pos + 1);
        if (val_pos == std::string::npos) continue;

        // Value must begin with 0x (skip derived defines like OFFSET_A + 0x8)
        if (val_pos + 1 >= ln.size() || ln[val_pos] != '0' || ln[val_pos + 1] != 'x') continue;

        size_t hex_pos = val_pos;

        // Locate comment
        size_t comment_pos = ln.find("//");
        if (comment_pos == std::string::npos || comment_pos < hex_pos) continue;

        std::string comment = ln.substr(comment_pos + 2);
        size_t cs = comment.find_first_not_of(" \t");
        if (cs != std::string::npos) comment = comment.substr(cs);

        // Parse [Section].key or [Section]key from comment
        if (comment.empty() || comment[0] != '[') continue;
        size_t sec_end = comment.find(']');
        if (sec_end == std::string::npos) continue;
        std::string section = comment.substr(1, sec_end - 1);

        size_t key_start = sec_end + 1;
        if (key_start < comment.size() && comment[key_start] == '.') key_start++;
        if (key_start >= comment.size()) continue;

        size_t key_end = comment.find_first_of(" \t", key_start);
        std::string key_name = comment.substr(key_start, key_end == std::string::npos ? std::string::npos : key_end - key_start);
        if (key_name.empty()) continue;

        // Build lookup key and find new value
        std::string lookup_key;
        uint64_t new_val = 0;
        bool found = false;

        if (section == "ConVars") {
            lookup_key = "[ConVars]" + key_name;
            auto it = con_data.find(lookup_key);
            if (it != con_data.end()) { new_val = it->second; found = true; }
        } else if (section == "Buttons") {
            // offsets.h uses in_X, dump uses +X
            std::string btn = key_name;
            if (btn.size() > 3 && btn.substr(0, 3) == "in_") btn = "+" + btn.substr(3);
            lookup_key = "[Buttons]" + btn;
            auto it = but_data.find(lookup_key);
            if (it != but_data.end()) { new_val = it->second; found = true; }
        } else if (section == "Miscellaneous") {
            lookup_key = "[.Miscellaneous]" + key_name;
            auto it = off_data.find(lookup_key);
            if (it != off_data.end()) { new_val = it->second; found = true; }
        } else {
            lookup_key = "[" + section + "]" + key_name;
            auto it = off_data.find(lookup_key);
            if (it != off_data.end()) { new_val = it->second; found = true; }
        }

        if (!found) continue;

        // Find end of the hex literal (past "0x" + hex digits)
        size_t hex_end = hex_pos + 2;
        while (hex_end < ln.size() && isxdigit((unsigned char)ln[hex_end])) hex_end++;

        // Middle = everything between end-of-hex and the "//" (preserves + 0x58 arithmetic)
        std::string middle = ln.substr(hex_end, comment_pos - hex_end);

        // Build new hex string
        char new_hex[32];
        snprintf(new_hex, sizeof(new_hex), "0x%llx", (unsigned long long)new_val);

        // Update date in comment
        std::string new_comment = comment;
        size_t updated_pos = new_comment.find("updated ");
        if (updated_pos != std::string::npos) {
            size_t date_start = updated_pos + 8;
            size_t date_end = new_comment.find_first_of(" \t", date_start);
            new_comment = new_comment.substr(0, date_start) + date_buf +
                          (date_end != std::string::npos ? new_comment.substr(date_end) : "");
        }

        ln = ln.substr(0, hex_pos) + new_hex + middle + "//" + new_comment;
        updated_count++;
    }

    if (updated_count == 0) {
        printf("update_offsets_h: no matching offsets found\n");
        return false;
    }

    std::ofstream out(offsets_h_path);
    if (!out.is_open()) {
        printf("update_offsets_h: cannot write %s\n", offsets_h_path);
        return false;
    }
    for (const auto& l : lines)
        out << l << "\n";

    printf("update_offsets_h: updated %d offsets in %s\n", updated_count, offsets_h_path);
    return true;
}
