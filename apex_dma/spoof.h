#pragma once
#include "memory.h"
#include <string>

bool spoof_gpu_uuid(std::string &real_uuid, std::string &fake_uuid);
bool physical_spoof(const std::string& target_uuid, std::string& fake_uuid);
