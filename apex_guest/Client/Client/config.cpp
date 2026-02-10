// config.cpp

#include "config.h"
#include <fstream>
#include <string>

void LoadConfig(Config& config) {

  std::ifstream file("config.txt");

  if (file.is_open()) {

    std::string line;

    while (std::getline(file, line)) {

      if (line.find("esp") != std::string::npos) {
        file >> std::boolalpha >> config.esp;
      }
      else if (line.find("aim_enable") != std::string::npos) {
        file >> std::boolalpha >> config.aim_enable;
      }
      else if (line.find("item_glow") != std::string::npos) {
        file >> std::boolalpha >> config.item_glow;
      }
      else if (line.find("player_glow") != std::string::npos) {
        file >> std::boolalpha >> config.player_glow;
      }
      else if (line.find("aim_no_recoil") != std::string::npos) {
        file >> std::boolalpha >> config.aim_no_recoil;
      }
      else if (line.find("thirdperson") != std::string::npos) {
        file >> std::boolalpha >> config.thirdperson;
      }
      else if (line.find("chargerifle") != std::string::npos) {
        file >> std::boolalpha >> config.chargerifle;
      }
      else if (line.find("max_dist") != std::string::npos) {
        file >> config.max_dist;
      }
      else if (line.find("smooth") != std::string::npos) {
        file >> config.smooth;
      }
      else if (line.find("max_fov") != std::string::npos) {
        file >> config.max_fov;
      }
      else if (line.find("bone") != std::string::npos) {
        file >> config.bone;
      }
      else if (line.find("v.box") != std::string::npos) {
        file >> std::boolalpha >> config.v.box;
      }
      else if (line.find("v.name") != std::string::npos) {
        file >> std::boolalpha >> config.v.name;
      }
      else if (line.find("v.fov") != std::string::npos) {
        file >> std::boolalpha >> config.v.fov;
      }
      else if (line.find("v.line") != std::string::npos) {
        file >> std::boolalpha >> config.v.line;
      }
      else if (line.find("v.distance") != std::string::npos) {
        file >> std::boolalpha >> config.v.distance;
      }
      else if (line.find("v.healthbar") != std::string::npos) {
        file >> std::boolalpha >> config.v.healthbar;
      }
      else if (line.find("v.shieldbar") != std::string::npos) {
        file >> std::boolalpha >> config.v.shieldbar;
      }
      else if (line.find("glowcolorviz") != std::string::npos) {
        file >> config.glowcolorviz.r >> config.glowcolorviz.g >> config.glowcolorviz.b;
      }
      else if (line.find("glowcolor") != std::string::npos) {
        file >> config.glowcolor.r >> config.glowcolor.g >> config.glowcolor.b;
      }
      else if (line.find("glowcolorknocked") != std::string::npos) {
        file >> config.glowcolorknocked.r >> config.glowcolorknocked.g >> config.glowcolorknocked.b;
      }
      else if (line.find("cfsize") != std::string::npos) {
        file >> config.cfsize;
      }
      else if (line.find("DDS") != std::string::npos) {
        file >> config.DDS;
      }

    }

    file.close();

  }

}

void SaveConfig(const Config& config) {

  std::ofstream file("config.txt");

  if (file.is_open()) {

    file << "esp " << std::boolalpha << config.esp << "\n";
    file << "aim_enable " << std::boolalpha << config.aim_enable << "\n";
    file << "item_glow " << std::boolalpha << config.item_glow << "\n";
    file << "player_glow " << std::boolalpha << config.player_glow << "\n";
    file << "aim_no_recoil " << std::boolalpha << config.aim_no_recoil << "\n";
    file << "thirdperson " << std::boolalpha << config.thirdperson << "\n";
    file << "chargerifle " << std::boolalpha << config.chargerifle << "\n";
    file << "max_dist " << config.max_dist << "\n";
    file << "smooth " << config.smooth << "\n";
    file << "max_fov " << config.max_fov << "\n";
    file << "bone " << config.bone << "\n";
    file << "v.box " << std::boolalpha << config.v.box << "\n";
    file << "v.name " << std::boolalpha << config.v.name << "\n";
    file << "v.fov " << std::boolalpha << config.v.fov << "\n";
    file << "v.line " << std::boolalpha << config.v.line << "\n";
    file << "v.distance " << std::boolalpha << config.v.distance << "\n";
    file << "v.healthbar " << std::boolalpha << config.v.healthbar << "\n";
    file << "v.shieldbar " << std::boolalpha << config.v.shieldbar << "\n";
    file << "glowcolorviz " << config.glowcolorviz.r << " " << config.glowcolorviz.g << " " << config.glowcolorviz.b << "\n";
    file << "glowcolor " << config.glowcolor.r << " " << config.glowcolor.g << " " << config.glowcolor.b << "\n";
    file << "glowcolorknocked " << config.glowcolorknocked.r << " " << config.glowcolorknocked.g << " " << config.glowcolorknocked.b << "\n";
    file << "cfsize " << config.cfsize << "\n";
    file << "DDS " << config.DDS << "\n";

    file.close();

  }

}