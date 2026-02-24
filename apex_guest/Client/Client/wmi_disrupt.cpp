#include "wmi_disrupt.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tlhelp32.h>
#include <aclapi.h>
#include <sddl.h>
#include <algorithm>

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
    const char* video_path = "SYSTEM\\CurrentControlSet\\Control\\Video";
    const char* class_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, video_path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char subKeyName[256];
        DWORD subKeyNameSize = sizeof(subKeyName);
        for (DWORD i = 0; RegEnumKeyExA(hKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            std::string base_path = std::string(video_path) + "\\" + subKeyName;

            HKEY hSubKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, base_path.c_str(), 0, KEY_READ, &hSubKey) == ERROR_SUCCESS) {
                char devKeyName[256];
                DWORD devKeyNameSize = sizeof(devKeyName);
                for (DWORD j = 0; RegEnumKeyExA(hSubKey, j, devKeyName, &devKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++j) {
                    std::string full_path = base_path + "\\" + devKeyName;
                    SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "GPU-UUID", fake_uuid);
                    SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "ClientUUID", fake_uuid);
                    SetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "PersistenceIdentifier", fake_uuid);
                    devKeyNameSize = sizeof(devKeyName);
                }
                RegCloseKey(hSubKey);
            }
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

    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global\\GridLicensing", "ClientUUID", fake_uuid);
}

void SearchAndReplaceRegistry(HKEY hKeyRoot, const std::string& subKey, const std::string& real_uuid, const std::string& fake_uuid) {
    HKEY hKey;
    if (RegOpenKeyExA(hKeyRoot, subKey.c_str(), 0, KEY_READ | KEY_WRITE, &hKey) != ERROR_SUCCESS)
        return;

    char valueName[256];
    DWORD valueNameSize = sizeof(valueName);
    char valueData[512];
    DWORD valueDataSize = sizeof(valueData);
    DWORD type;

    for (DWORD i = 0; RegEnumValueA(hKey, i, valueName, &valueNameSize, NULL, &type, (LPBYTE)valueData, &valueDataSize) == ERROR_SUCCESS; ++i) {
        if (type == REG_SZ) {
            std::string data(valueData);
            if (data.find(real_uuid) != std::string::npos) {
                std::string newData = data;
                size_t pos = 0;
                while ((pos = newData.find(real_uuid, pos)) != std::string::npos) {
                    newData.replace(pos, real_uuid.length(), fake_uuid);
                    pos += fake_uuid.length();
                }
                RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE*)newData.c_str(), (DWORD)(newData.length() + 1));
            }
        }
        valueNameSize = sizeof(valueName);
        valueDataSize = sizeof(valueData);
    }

    char subKeyName[256];
    DWORD subKeyNameSize = sizeof(subKeyName);
    for (DWORD i = 0; RegEnumKeyExA(hKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
        SearchAndReplaceRegistry(hKeyRoot, subKey + "\\" + subKeyName, real_uuid, fake_uuid);
        subKeyNameSize = sizeof(subKeyName);
    }

    RegCloseKey(hKey);
}

void VerifyRegistrySpoofs(const std::string& real_uuid) {
    std::cout << "\n--- Registry Verification ---" << std::endl;
    const char* class_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}";

    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, class_path, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char subKeyName[256];
        DWORD subKeyNameSize = sizeof(subKeyName);
        for (DWORD i = 0; RegEnumKeyExA(hKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            if (isdigit(subKeyName[0])) {
                std::string full_path = std::string(class_path) + "\\" + subKeyName;
                std::string current_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, full_path.c_str(), "GPU-UUID");
                if (!current_uuid.empty()) {
                    std::cout << "Class\\...\\" << subKeyName << "\\GPU-UUID: " << current_uuid << std::endl;
                }
            }
            subKeyNameSize = sizeof(subKeyName);
        }
        RegCloseKey(hKey);
    }

    std::string grid_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global\\GridLicensing", "ClientUUID");
    if (!grid_uuid.empty()) {
        std::cout << "GridLicensing\\ClientUUID: " << grid_uuid << std::endl;
    } else {
        std::cout << "GridLicensing\\ClientUUID: NOT FOUND" << std::endl;
    }

    if (!real_uuid.empty()) {
        std::cout << "Searching registry for real UUID occurrences..." << std::endl;
        SearchAndReplaceRegistry(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", real_uuid, "SPOOFED");
        SearchAndReplaceRegistry(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", real_uuid, "SPOOFED");
    }
}

void PrintNvidiaSmiInfo() {
    std::cout << "\n--- NVIDIA-SMI (Guest) Info ---" << std::endl;
    system("nvidia-smi -L");
    system("nvidia-smi --query-gpu=uuid,name --format=csv");
    std::cout << "-------------------------------" << std::endl;
}

bool IdentifyAndSpoofGPU(const char* host_real_uuid) {
    std::ifstream infile("original_gpu.txt");
    if (infile.is_open()) return true;

    std::string real_uuid;
    if (host_real_uuid && strlen(host_real_uuid) > 0) {
        real_uuid = host_real_uuid;
    } else {
        const char* class_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";
        real_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, class_path, "GPU-UUID");

        if (real_uuid.empty()) {
            real_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global\\GridLicensing", "ClientUUID");
        }
    }

    if (!real_uuid.empty()) {
        std::ofstream outfile("original_gpu.txt");
        outfile << real_uuid;
        outfile.close();
    }
    return true;
}

void RestoreGPU() {
    std::ifstream infile("original_gpu.txt");
    std::string original_uuid;
    if (infile.is_open()) {
        std::getline(infile, original_uuid);
        infile.close();
    }
    if (!original_uuid.empty()) ApplyRegistrySpoofs(original_uuid);
}
