#pragma once
#include <string>
#include <vector>
#include <Windows.h>

struct HWIDData {
    std::string mac_real;
    std::string mac_spoof;
    std::string machine_guid_real;
    std::string machine_guid_spoof;
    std::string hw_profile_guid_real;
    std::string hw_profile_guid_spoof;
    std::string machine_id_real;
    std::string machine_id_spoof;
    std::string computer_hardware_id_real;
    std::string computer_hardware_id_spoof;
    std::string product_id_real;
    std::string product_id_spoof;
    std::string install_date_real;
    std::string install_date_spoof;
    std::string bios_serial_real;
    std::string bios_serial_spoof;
    std::string nv_uuid_real;
    std::string nv_uuid_spoof;
    std::string disk_id_real;
    std::string disk_id_spoof;
    std::string disk_serial_real;
    std::string disk_serial_spoof;
};

extern HWIDData g_hwid;

bool IsAlreadyPatched();
void GetRealRegistryIDs();
void ApplyRegistrySpoofs(void* unused1, void* unused2);
void LoadCurrentIDsAsSpoofed();
void DisruptWMI();
std::string GetRealMAC();
std::string GenerateSpoofMAC();
std::string RandomString(size_t length, bool hex = false);
std::string GenerateGUID();
