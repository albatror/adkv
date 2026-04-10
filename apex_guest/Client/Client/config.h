// config.h
#pragma once
#include <string>

void LoadConfig(const std::string& filename);
void SaveConfig(const std::string& filename);

extern float ads_fov;
extern float ads_smooth;
extern float hip_fov;
extern float hip_smooth;
