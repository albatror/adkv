// config.h
#pragma once

struct Config {

  // Bools
  bool esp = false;
  bool aim_enable = false;
  bool item_glow = false;
  bool player_glow = false;
  bool aim_no_recoil = false;
  bool thirdperson = false;
  bool chargerifle = false;

  // Floats
  float max_dist = 100.f;
  float smooth = 0.f;
  float max_fov = 0.f;

  // Ints
  int bone = 0;

  // Visuals struct
  struct {
    bool box = false;
    bool name = false;
    bool fov = false;
    bool line = false;
    bool distance = false;
    bool healthbar = false;
    bool shieldbar = false;
  } v;

  // Glow colors
  struct {
   float r = 0.f, g = 0.f, b = 0.f;
  } glowcolorviz;

  struct {
   float r = 0.f, g = 0.f, b = 0.f;
  } glowcolor;

  struct {
   float r = 0.f, g = 0.f, b = 0.f;
  } glowcolorknocked;

  // Other floats
  float cfsize = 0.f;
  float DDS = 0.f;

};

void LoadConfig(Config& config);
void SaveConfig(const Config& config);
