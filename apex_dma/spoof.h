#pragma once
#include "memory.h"
#include <string>

bool spoof_gpu_uuid(const std::string& target_uuid, std::string &real_uuid, std::string &fake_uuid);
bool physical_spoof(const std::string& target_uuid, std::string& fake_uuid, bool verbose);
bool manual_driver_spoof(const std::string& target_uuid, uint64_t system_addr, uint64_t gpu_addr, uint32_t valid_offset, std::string &real_uuid, std::string &fake_uuid);
