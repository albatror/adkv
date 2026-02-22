#pragma once
#include "memory.h"
#include <string>

struct HardwareIdentifiers {
    std::string mac;
    std::string gpu_uuid;
    std::string machine_guid;
    std::string hw_profile_guid;
    std::string disk_serial;
    std::string smbios_serial;
};

extern HardwareIdentifiers g_spoofed_infos;

void SpoofHardware();
void SaveHardwareInfos(const std::string& filename, const HardwareIdentifiers& infos);
bool LoadHardwareInfos(const std::string& filename, HardwareIdentifiers& infos);
void MacToBytes(const std::string& mac_str, uint8_t* bytes);
std::string MacToString(const uint8_t* bytes);
void UUIDToBytes(const char* uuid, uint8_t* bytes, bool little_endian);
