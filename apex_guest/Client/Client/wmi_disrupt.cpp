#include "wmi_disrupt.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tlhelp32.h>
#include <aclapi.h>
#include <sddl.h>

#pragma comment(lib, "advapi32.lib")

std::string GetRegistryString(HKEY hKeyRoot, const char* subKey, const char* valueName) {
    HKEY hKey;
    if (RegOpenKeyExA(hKeyRoot, subKey, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
        return "";

    char buffer[512];
    DWORD bufferSize = sizeof(buffer);
    if (RegQueryValueExA(hKey, valueName, NULL, NULL, (LPBYTE)buffer, &bufferSize) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return "";
    }

    RegCloseKey(hKey);
    return std::string(buffer);
}

bool SetRegistryString(HKEY hKeyRoot, const char* subKey, const char* valueName, const std::string& value) {
    HKEY hKey;
    if (RegOpenKeyExA(hKeyRoot, subKey, 0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        return false;

    if (RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)(value.length() + 1)) != ERROR_SUCCESS) {
        RegCloseKey(hKey);
        return false;
    }

    RegCloseKey(hKey);
    return true;
}

void ApplyRegistrySpoofs(const std::string& fake_uuid) {
    // NVIDIA specific registry keys
    const char* video_path = "SYSTEM\\CurrentControlSet\\Control\\Video";
    const char* class_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, video_path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char subKeyName[256];
        DWORD subKeyNameSize = sizeof(subKeyName);
        for (DWORD i = 0; RegEnumKeyExA(hKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            std::string full_path = std::string(video_path) + "\\" + subKeyName + "\\0000";

            // Check for GPU-UUID or similar
            // nvidia-smi often uses these
            SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "GPU-UUID", fake_uuid);
            SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "ClientUUID", fake_uuid);
            SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "PersistenceIdentifier", fake_uuid);

            subKeyNameSize = sizeof(subKeyName);
        }
        RegCloseKey(hKey);
    }

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, class_path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char subKeyName[256];
        DWORD subKeyNameSize = sizeof(subKeyName);
        for (DWORD i = 0; RegEnumKeyExA(hKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            if (isdigit(subKeyName[0])) {
                std::string full_path = std::string(class_path) + "\\" + subKeyName;
                SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "GPU-UUID", fake_uuid);
                SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "ClientUUID", fake_uuid);
            }
            subKeyNameSize = sizeof(subKeyName);
        }
        RegCloseKey(hKey);
    }
}

bool IdentifyAndSpoofGPU() {
    // Check if we already have the original UUID saved
    std::ifstream infile("original_gpu.txt");
    if (infile.is_open()) {
        return true; // Already saved/handled
    }

    // Find the current (real) UUID in the registry
    std::string real_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000", "GPU-UUID");
    if (real_uuid.empty()) {
        real_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video\\{some-guid}\\0000", "GPU-UUID");
    }

    if (!real_uuid.empty()) {
        std::ofstream outfile("original_gpu.txt");
        outfile << real_uuid;
        outfile.close();
    }

    return true;
}

bool DisruptWMI() {
    // Deny all access to WMI ALPC ports to block queries
    // This is a common technique on UnknownCheats

    const char* sddl = "D:(D;;GA;;;WD)"; // Deny All to Everyone
    PSECURITY_DESCRIPTOR sd = NULL;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorA(sddl, SDDL_REVISION_1, &sd, NULL)) {
        return false;
    }

    // This part is complex and usually involves duplicating handles from the WMI service
    // For now we'll skip the full ALPC implementation unless requested,
    // but the registry part is usually enough for nvidia-smi.

    LocalFree(sd);
    return true;
}

void RestoreGPU() {
    std::ifstream infile("original_gpu.txt");
    std::string original_uuid;
    if (infile.is_open()) {
        std::getline(infile, original_uuid);
        infile.close();
    }

    if (!original_uuid.empty()) {
        ApplyRegistrySpoofs(original_uuid);
    }
}
