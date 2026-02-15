#include "Config.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <cctype>

Config config;

static std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) start++;
    auto end = s.end();
    if (std::distance(start, end) <= 0) return "";
    do { end--; } while (std::distance(start, end) > 0 && std::isspace(*end)) ;
    return std::string(start, end + 1);
}

bool Config::load(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open config file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#' || line[0] == ';' || line[0] == '[') continue;

        auto delimiterPos = line.find('=');
        if (delimiterPos == std::string::npos) continue;

        std::string key = trim(line.substr(0, delimiterPos));
        std::string value = trim(line.substr(delimiterPos + 1));

        try {
            if (key == "firing_range") firing_range = (value == "true" || value == "1");
            else if (key == "onevone") onevone = (value == "true" || value == "1");
            else if (key == "max_dist") max_dist = std::stof(value);
            else if (key == "aim") aim = std::stoi(value);
            else if (key == "aiming") aiming = (value == "true" || value == "1");
            else if (key == "max_fov") max_fov = std::stof(value);
            else if (key == "smooth") smooth = std::stof(value);
            else if (key == "bone") bone = std::stoi(value);
            else if (key == "bone_auto") bone_auto = (value == "true" || value == "1");
            else if (key == "aim_no_recoil") aim_no_recoil = (value == "true" || value == "1");
            else if (key == "player_glow") player_glow = (value == "true" || value == "1");
            else if (key == "inside_value") inside_value = std::stoi(value);
            else if (key == "outline_size") outline_size = std::stoi(value);
            else if (key == "outside_value") outside_value = std::stoi(value);
            else if (key == "glow_r") glow_r = std::stof(value);
            else if (key == "glow_g") glow_g = std::stof(value);
            else if (key == "glow_b") glow_b = std::stof(value);
            else if (key == "glow_r_viz") glow_r_viz = std::stof(value);
            else if (key == "glow_g_viz") glow_g_viz = std::stof(value);
            else if (key == "glow_b_viz") glow_b_viz = std::stof(value);
            else if (key == "glow_r_knocked") glow_r_knocked = std::stof(value);
            else if (key == "glow_g_knocked") glow_g_knocked = std::stof(value);
            else if (key == "glow_b_knocked") glow_b_knocked = std::stof(value);
            else if (key == "knocked_context_id") knocked_context_id = std::stoi(value);
            else if (key == "knocked_setting_index") knocked_setting_index = std::stoi(value);
            else if (key == "visible_context_id") visible_context_id = std::stoi(value);
            else if (key == "visible_setting_index") visible_setting_index = std::stoi(value);
            else if (key == "not_visible_context_id") not_visible_context_id = std::stoi(value);
            else if (key == "not_visible_setting_index") not_visible_setting_index = std::stoi(value);
            else if (key == "screen_width") screen_width = std::stoi(value);
            else if (key == "screen_height") screen_height = std::stoi(value);
            else if (key == "super_key") super_key = std::stoi(value);
            else if (key == "flickbot") flickbot = (value == "true" || value == "1");
            else if (key == "triggerbot") triggerbot = (value == "true" || value == "1");
            else if (key == "rapidfire") rapidfire = (value == "true" || value == "1");
            else if (key == "superglide") superglide = (value == "true" || value == "1");
            else if (key == "bhop") bhop = (value == "true" || value == "1");
            else if (key == "walljump") walljump = (value == "true" || value == "1");
            else if (key == "autotapstrafe") autotapstrafe = (value == "true" || value == "1");
            else if (key == "glowarmor") glowarmor = (value == "true" || value == "1");
        } catch (const std::exception& e) {
            std::cerr << "Error parsing config key " << key << ": " << e.what() << std::endl;
        }
    }
    return true;
}
