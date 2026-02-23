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
extern bool player_glow;
extern bool aim_no_recoil;
extern bool lock_target;
extern float max_dist;
extern float default_smooth;
extern float default_fov;
extern int bone;
extern float glowr;
extern float glowg;
extern float glowb;
extern float glowcolor[3];
extern float glowrviz;
extern float glowgviz;
extern float glowbviz;
extern float glowcolorviz[3];
extern float glowrknocked;
extern float glowgknocked;
extern float glowbknocked;
extern float glowcolorknocked[3];
extern bool firing_range;
extern bool onevone;
extern float DDS;
extern float min_max_fov;
extern float max_max_fov;
extern float min_smooth;
extern float max_smooth;
extern float min_cfsize;
extern float max_cfsize;
extern bool flickbot;
extern bool triggerbot;
extern bool superglide;
extern bool bhop;
extern bool walljump;
extern float flickbot_fov;
extern float flickbot_smooth;
extern float triggerbot_fov;
extern bool fov;
extern float cfsize;
extern bool disrupt_wmi;
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
    file << "player_glow " << std::boolalpha << player_glow << "\n";
    file << "aim_no_recoil " << std::boolalpha << aim_no_recoil << "\n";
    file << "lock_target " << std::boolalpha << lock_target << "\n";
    file << "max_dist " << max_dist << "\n";
    file << "default_smooth " << default_smooth << "\n";
    file << "default_fov " << default_fov << "\n";
    file << "bone " << bone << "\n";
    file << "healthbar " << std::boolalpha << v.healthbar << "\n";
    file << "shieldbar " << std::boolalpha << v.shieldbar << "\n";
    file << "distance " << std::boolalpha << v.distance << "\n";
    file << "line " << std::boolalpha << v.line << "\n";
    file << "skeleton " << std::boolalpha << v.skeleton << "\n";
    file << "spectator_notifier " << std::boolalpha << v.spectator_notifier << "\n";
    file << "info_window " << std::boolalpha << v.info_window << "\n";
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
    file << "firing_range " << std::boolalpha << firing_range << "\n";
    file << "onevone " << std::boolalpha << onevone << "\n";
    file << "DDS " << DDS << "\n";
    file << "min_max_fov " << min_max_fov << "\n";
    file << "max_max_fov " << max_max_fov << "\n";
    file << "min_smooth " << min_smooth << "\n";
    file << "max_smooth " << max_smooth << "\n";
    file << "min_cfsize " << min_cfsize << "\n";
    file << "max_cfsize " << max_cfsize << "\n";
    file << "flickbot " << std::boolalpha << flickbot << "\n";
    file << "triggerbot " << std::boolalpha << triggerbot << "\n";
    file << "superglide " << std::boolalpha << superglide << "\n";
    file << "bhop " << std::boolalpha << bhop << "\n";
    file << "walljump " << std::boolalpha << walljump << "\n";
    file << "flickbot_fov " << flickbot_fov << "\n";
    file << "flickbot_smooth " << flickbot_smooth << "\n";
    file << "triggerbot_fov " << triggerbot_fov << "\n";
    file << "flickbot_fov_circle " << std::boolalpha << v.flickbot_fov_circle << "\n";
    file << "triggerbot_fov_circle " << std::boolalpha << v.triggerbot_fov_circle << "\n";
    file << "fov " << std::boolalpha << fov << "\n";
    file << "cfsize " << cfsize << "\n";
    file << "disrupt_wmi " << std::boolalpha << disrupt_wmi << "\n";

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
        else if (key == "player_glow") ss >> std::boolalpha >> player_glow;
        else if (key == "aim_no_recoil") ss >> std::boolalpha >> aim_no_recoil;
        else if (key == "lock_target") ss >> std::boolalpha >> lock_target;
        else if (key == "max_dist") ss >> max_dist;
        else if (key == "default_smooth") ss >> default_smooth;
        else if (key == "default_fov") ss >> default_fov;
        else if (key == "bone") ss >> bone;
        else if (key == "healthbar") ss >> std::boolalpha >> v.healthbar;
        else if (key == "shieldbar") ss >> std::boolalpha >> v.shieldbar;
        else if (key == "distance") ss >> std::boolalpha >> v.distance;
        else if (key == "line") ss >> std::boolalpha >> v.line;
        else if (key == "skeleton") ss >> std::boolalpha >> v.skeleton;
        else if (key == "spectator_notifier") ss >> std::boolalpha >> v.spectator_notifier;
        else if (key == "info_window") ss >> std::boolalpha >> v.info_window;
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
        else if (key == "firing_range") ss >> std::boolalpha >> firing_range;
        else if (key == "onevone") ss >> std::boolalpha >> onevone;
        else if (key == "DDS") ss >> DDS;
        else if (key == "min_max_fov") ss >> min_max_fov;
        else if (key == "max_max_fov") ss >> max_max_fov;
        else if (key == "min_smooth") ss >> min_smooth;
        else if (key == "max_smooth") ss >> max_smooth;
        else if (key == "min_cfsize") ss >> min_cfsize;
        else if (key == "max_cfsize") ss >> max_cfsize;
        else if (key == "flickbot") ss >> std::boolalpha >> flickbot;
        else if (key == "triggerbot") ss >> std::boolalpha >> triggerbot;
        else if (key == "superglide") ss >> std::boolalpha >> superglide;
        else if (key == "bhop") ss >> std::boolalpha >> bhop;
        else if (key == "walljump") ss >> std::boolalpha >> walljump;
        else if (key == "flickbot_fov") ss >> flickbot_fov;
        else if (key == "flickbot_smooth") ss >> flickbot_smooth;
        else if (key == "triggerbot_fov") ss >> triggerbot_fov;
        else if (key == "flickbot_fov_circle") ss >> std::boolalpha >> v.flickbot_fov_circle;
        else if (key == "triggerbot_fov_circle") ss >> std::boolalpha >> v.triggerbot_fov_circle;
        else if (key == "fov") ss >> std::boolalpha >> fov;
        else if (key == "cfsize") ss >> cfsize;
        else if (key == "disrupt_wmi") ss >> std::boolalpha >> disrupt_wmi;
    }

    file.close();
}
