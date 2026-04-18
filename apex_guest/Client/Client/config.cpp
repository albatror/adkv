// config.cpp

#include "config.h"
#include "overlay.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <cctype>
#include <iterator>

extern int aim;
extern bool esp;
extern bool item_glow;
extern bool player_glow;
extern bool aim_no_recoil;
extern bool lock_target;
extern float max_dist;
extern float aim_dist;

extern int bone;
extern float glowr;
extern float glowg;
extern float glowb;
extern float glowcolor[3];
extern float glowrviz;
extern float glowgviz;
extern float glowbviz;
extern float glowcolorviz[3];
extern unsigned char insidevalue;
extern unsigned char outlinesize;
extern float glowrknocked;
extern float glowgknocked;
extern float glowbknocked;
extern float glowcolorknocked[3];
extern bool firing_range;
extern bool onevone;
extern bool triggerbot;
extern bool superglide;
extern bool bhop;
extern bool walljump;
extern float triggerbot_fov;
extern float vel_multiplier;
extern bool vel_manual;
extern visuals v;

// Helper to trim strings
std::string trim(const std::string& s) {
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start)) {
        start++;
    }
    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
}

void SaveConfig(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) return;

    file << "aim " << aim << "\n";
    file << "esp " << std::boolalpha << esp << "\n";
    file << "item_glow " << std::boolalpha << item_glow << "\n";
    file << "player_glow " << std::boolalpha << player_glow << "\n";
    file << "aim_no_recoil " << std::boolalpha << aim_no_recoil << "\n";
    file << "lock_target " << std::boolalpha << lock_target << "\n";
    file << "max_dist " << max_dist << "\n";
    file << "aim_dist " << aim_dist << "\n";
    file << "ads_fov " << ads_fov << "\n";
    file << "ads_smooth " << ads_smooth << "\n";
    file << "hip_fov " << hip_fov << "\n";
    file << "hip_smooth " << hip_smooth << "\n";
    file << "vel_multiplier " << vel_multiplier << "\n";
    file << "vel_manual " << std::boolalpha << vel_manual << "\n";
    file << "bone " << bone << "\n";
    file << "healthbar " << std::boolalpha << v.healthbar << "\n";
    file << "shieldbar " << std::boolalpha << v.shieldbar << "\n";
    file << "weapon " << std::boolalpha << v.weapon << "\n";
    file << "distance " << std::boolalpha << v.distance << "\n";
    file << "platform " << std::boolalpha << v.platform << "\n";
    file << "line " << std::boolalpha << v.line << "\n";
    file << "skeleton " << std::boolalpha << v.skeleton << "\n";
    file << "skeleton_thickness " << v.skeleton_thickness << "\n";
    file << "spectator_notifier " << std::boolalpha << v.spectator_notifier << "\n";
    file << "info_window " << std::boolalpha << v.info_window << "\n";
    file << "info_window_logo " << std::boolalpha << v.info_window_logo << "\n";
    file << "target_indicator " << std::boolalpha << v.target_indicator << "\n";
    file << "target_indicator_fov " << v.target_indicator_fov << "\n";
    file << "glowr " << glowr << "\n";
    file << "glowg " << glowg << "\n";
    file << "glowb " << glowb << "\n";
    file << "glowcolor " << glowcolor[0] << " " << glowcolor[1] << " " << glowcolor[2] << "\n";
    file << "glowrviz " << glowrviz << "\n";
    file << "glowgviz " << glowgviz << "\n";
    file << "glowbviz " << glowbviz << "\n";
    file << "glowcolorviz " << glowcolorviz[0] << " " << glowcolorviz[1] << " " << glowcolorviz[2] << "\n";
    file << "glowrknocked " << glowrknocked << "\n";
    file << "glowgknocked " << glowgknocked << "\n";
    file << "glowbknocked " << glowbknocked << "\n";
    file << "glowcolorknocked " << glowcolorknocked[0] << " " << glowcolorknocked[1] << " " << glowcolorknocked[2] << "\n";
    file << "insidevalue " << (int)insidevalue << "\n";
    file << "outlinesize " << (int)outlinesize << "\n";
    file << "firing_range " << std::boolalpha << firing_range << "\n";
    file << "onevone " << std::boolalpha << onevone << "\n";
    file << "triggerbot " << std::boolalpha << triggerbot << "\n";
    file << "superglide " << std::boolalpha << superglide << "\n";
    file << "bhop " << std::boolalpha << bhop << "\n";
    file << "walljump " << std::boolalpha << walljump << "\n";
    file << "triggerbot_fov " << triggerbot_fov << "\n";
    file << "triggerbot_fov_circle " << std::boolalpha << v.triggerbot_fov_circle << "\n";
    file << "ads_fov_circle " << std::boolalpha << v.ads_fov_circle << "\n";
    file << "hip_fov_circle " << std::boolalpha << v.hip_fov_circle << "\n";

    file.close();
}

void LoadConfig(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string key;
        ss >> key;

        if (key == "aim") ss >> aim;
        else if (key == "esp") ss >> std::boolalpha >> esp;
        else if (key == "item_glow") ss >> std::boolalpha >> item_glow;
        else if (key == "player_glow") ss >> std::boolalpha >> player_glow;
        else if (key == "aim_no_recoil") ss >> std::boolalpha >> aim_no_recoil;
        else if (key == "lock_target") ss >> std::boolalpha >> lock_target;
        else if (key == "max_dist") ss >> max_dist;
        else if (key == "aim_dist") ss >> aim_dist;
        else if (key == "ads_fov") ss >> ads_fov;
        else if (key == "ads_smooth") ss >> ads_smooth;
        else if (key == "hip_fov") ss >> hip_fov;
        else if (key == "hip_smooth") ss >> hip_smooth;
        else if (key == "vel_multiplier") ss >> vel_multiplier;
        else if (key == "vel_manual") ss >> std::boolalpha >> vel_manual;
        else if (key == "bone") ss >> bone;
        else if (key == "healthbar") ss >> std::boolalpha >> v.healthbar;
        else if (key == "shieldbar") ss >> std::boolalpha >> v.shieldbar;
        else if (key == "weapon") ss >> std::boolalpha >> v.weapon;
        else if (key == "distance") ss >> std::boolalpha >> v.distance;
        else if (key == "platform") ss >> std::boolalpha >> v.platform;
        else if (key == "line") ss >> std::boolalpha >> v.line;
        else if (key == "skeleton") ss >> std::boolalpha >> v.skeleton;
        else if (key == "skeleton_thickness") ss >> v.skeleton_thickness;
        else if (key == "spectator_notifier") ss >> std::boolalpha >> v.spectator_notifier;
        else if (key == "info_window") ss >> std::boolalpha >> v.info_window;
        else if (key == "info_window_logo") ss >> std::boolalpha >> v.info_window_logo;
        else if (key == "target_indicator") ss >> std::boolalpha >> v.target_indicator;
        else if (key == "target_indicator_fov") ss >> v.target_indicator_fov;
        else if (key == "glowr") ss >> glowr;
        else if (key == "glowg") ss >> glowg;
        else if (key == "glowb") ss >> glowb;
        else if (key == "glowcolor") ss >> glowcolor[0] >> glowcolor[1] >> glowcolor[2];
        else if (key == "glowrviz") ss >> glowrviz;
        else if (key == "glowgviz") ss >> glowgviz;
        else if (key == "glowbviz") ss >> glowbviz;
        else if (key == "glowcolorviz") ss >> glowcolorviz[0] >> glowcolorviz[1] >> glowcolorviz[2];
        else if (key == "glowrknocked") ss >> glowrknocked;
        else if (key == "glowgknocked") ss >> glowgknocked;
        else if (key == "glowbknocked") ss >> glowbknocked;
        else if (key == "glowcolorknocked") ss >> glowcolorknocked[0] >> glowcolorknocked[1] >> glowcolorknocked[2];
        else if (key == "insidevalue") { int val; ss >> val; insidevalue = (unsigned char)val; }
        else if (key == "outlinesize") { int val; ss >> val; outlinesize = (unsigned char)val; }
        else if (key == "firing_range") ss >> std::boolalpha >> firing_range;
        else if (key == "onevone") ss >> std::boolalpha >> onevone;
        else if (key == "triggerbot") ss >> std::boolalpha >> triggerbot;
        else if (key == "superglide") ss >> std::boolalpha >> superglide;
        else if (key == "bhop") ss >> std::boolalpha >> bhop;
        else if (key == "walljump") ss >> std::boolalpha >> walljump;
        else if (key == "triggerbot_fov") ss >> triggerbot_fov;
        else if (key == "triggerbot_fov_circle") ss >> std::boolalpha >> v.triggerbot_fov_circle;
        else if (key == "ads_fov_circle") ss >> std::boolalpha >> v.ads_fov_circle;
        else if (key == "hip_fov_circle") ss >> std::boolalpha >> v.hip_fov_circle;
    }

    file.close();
}
