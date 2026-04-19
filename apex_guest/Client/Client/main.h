#pragma once

#include <windows.h>
#include <time.h>
#include <fstream>
#include <iostream>
#include <locale>

#include "math.h"
#include "overlay.h"

extern float ads_fov;
extern float ads_smooth;
extern float hip_fov;
extern float hip_smooth;
extern bool aassist;
extern float aassist_dist;
extern char real_gpu_uuid[32];
extern char fake_gpu_uuid[32];
extern bool gpu_spoof_active;