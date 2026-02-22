#pragma once
#include <Windows.h>

bool DisruptWMI();
void GetRealRegistryIDs(char* mguid, char* hwid);
void GetRealDiskSerial(char* serial);
void GetRealSMBIOSSerial(char* serial);
void ApplyRegistrySpoofs(const char* spoof_mguid, const char* spoof_hwid);
