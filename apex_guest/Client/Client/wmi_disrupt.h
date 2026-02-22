#pragma once
#include <Windows.h>

bool DisruptWMI();
bool SpoofMachineGuid();
void GetRealRegistryIDs(char* mguid, char* hwid);
void ApplyRegistrySpoofs(const char* spoof_mguid, const char* spoof_hwid);
