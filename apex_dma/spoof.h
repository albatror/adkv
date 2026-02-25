#pragma once
#include <string>

extern std::string real_gpu_uuid;
extern std::string fake_gpu_uuid;
extern bool gpu_spoofed;

bool spoof_gpu_uuid();
bool spoof_gpu_uuid_v2(const std::string& target_real_uuid);
