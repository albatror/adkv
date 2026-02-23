#pragma once
#include <string>

bool IdentifyAndSpoofGPU();
void ApplyRegistrySpoofs(const std::string& fake_uuid);
bool DisruptWMI();
void RestoreGPU();
