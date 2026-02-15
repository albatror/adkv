#ifndef CONFIG_H
#define CONFIG_H

#include <string>

struct Config {
    // Main
    bool firing_range = false;
    bool onevone = false;
    float max_dist = 8000.0f;

    // Aimbot
    int aim = 2;
    bool aiming = true;
    float max_fov = 10.0f;
    float smooth = 12.0f;
    int bone = 2;
    bool bone_auto = true;
    bool aim_no_recoil = true;

    // Glow
    bool player_glow = true;
    int inside_value = 6;
    int outline_size = 32;
    int outside_value = 125;

    // Glow Colors
    float glow_r = 1.0f;
    float glow_g = 0.0f;
    float glow_b = 0.0f;
    float glow_r_viz = 0.0f;
    float glow_g_viz = 1.0f;
    float glow_b_viz = 0.0f;
    float glow_r_knocked = 0.0f;
    float glow_g_knocked = 0.0f;
    float glow_b_knocked = 1.0f;

    // Glow Settings
    int knocked_context_id = 5;
    int knocked_setting_index = 80;
    int visible_context_id = 6;
    int visible_setting_index = 81;
    int not_visible_context_id = 7;
    int not_visible_setting_index = 82;

    // Screen
    int screen_width = 2560;
    int screen_height = 1440;

    // Misc
    int super_key = 0;

    // Extra toggles
    bool flickbot = false;
    bool triggerbot = false;
    bool rapidfire = false;
    bool superglide = false;
    bool bhop = false;
    bool walljump = false;
    bool autotapstrafe = false;
    bool glowarmor = false;

    bool load(const std::string& filename);
};

extern Config config;

#endif
