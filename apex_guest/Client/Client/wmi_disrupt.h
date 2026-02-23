#pragma once
#include <string>

bool IdentifyAndSpoofGPU();
void ApplyRegistrySpoofs(const std::string& fake_uuid);
void SearchAndReplaceRegistry(HKEY hKeyRoot, const std::string& subKey, const std::string& real_uuid, const std::string& fake_uuid);
void VerifyRegistrySpoofs(const std::string& real_uuid);
bool DisruptWMI();
void RestoreGPU();
