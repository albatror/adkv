#pragma once
#include "memory.h"
#include <string>

void SpoofHardware();
void SaveHardwareInfos(const std::string& filename, const std::string& mac, const std::string& gpu_uuid);
bool LoadHardwareInfos(const std::string& filename, std::string& mac, std::string& gpu_uuid);
void MacToBytes(const std::string& mac_str, uint8_t* bytes);
std::string MacToString(const uint8_t* bytes);
void UUIDToBytes(const char* uuid, uint8_t* bytes, bool little_endian);
