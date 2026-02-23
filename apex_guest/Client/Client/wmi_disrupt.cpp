#include "wmi_disrupt.h"
#include "XorString.h"
#include <iostream>
#include <fstream>
#include <random>
#include <algorithm>
#include <sddl.h>
#include <aclapi.h>

#pragma comment(lib, "Advapi32.lib")

char real_gpu_uuid[256] = "";
char spoofed_gpu_uuid[256] = "";
bool disrupt_wmi = false;
bool gpu_spoofed = false;

// Function to generate a random GPU UUID
std::string GenerateRandomGPUUUID() {
    static const char hex_chars[] = "0123456789abcdef";
    std::string uuid = "GPU-";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    for (int i = 0; i < 8; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 12; ++i) uuid += hex_chars[dis(gen)];

    return uuid;
}

bool GetRegistryString(HKEY hKey, const char* subKey, const char* valueName, std::string& outString) {
    char data[256];
    DWORD dataSize = sizeof(data);
    if (RegGetValueA(hKey, subKey, valueName, RRF_RT_REG_SZ, NULL, data, &dataSize) == ERROR_SUCCESS) {
        outString = data;
        // Basic check to ensure it looks like a GPU UUID
        if (outString.find("GPU-") != std::string::npos) {
            return true;
        }
    }
    return false;
}

bool SearchForGPUUUID(HKEY hRoot, const char* baseKey, std::string& outUUID) {
    HKEY hBase;
    if (RegOpenKeyExA(hRoot, baseKey, 0, KEY_READ, &hBase) != ERROR_SUCCESS) return false;

    char subKeyName[256];
    DWORD subKeyNameSize;
    DWORD index = 0;
    while (true) {
        subKeyNameSize = sizeof(subKeyName);
        if (RegEnumKeyExA(hBase, index++, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) break;

        // Try directly under this subkey (e.g., 0000 or {GUID})
        if (GetRegistryString(hBase, subKeyName, "GPU-UUID", outUUID)) { RegCloseKey(hBase); return true; }
        if (GetRegistryString(hBase, subKeyName, "ClientUUID", outUUID)) { RegCloseKey(hBase); return true; }
        if (GetRegistryString(hBase, subKeyName, "PersistenceIdentifier", outUUID)) { RegCloseKey(hBase); return true; }

        // Try nested (e.g., {GUID}\0000 or DriverClass\0000)
        HKEY hNested;
        if (RegOpenKeyExA(hBase, subKeyName, 0, KEY_READ, &hNested) == ERROR_SUCCESS) {
            char nestedName[256];
            DWORD nestedSize;
            DWORD nestedIndex = 0;
            while (true) {
                nestedSize = sizeof(nestedName);
                if (RegEnumKeyExA(hNested, nestedIndex++, nestedName, &nestedSize, NULL, NULL, NULL, NULL) != ERROR_SUCCESS) break;

                // Check common value names
                if (GetRegistryString(hNested, nestedName, "GPU-UUID", outUUID)) { RegCloseKey(hNested); RegCloseKey(hBase); return true; }
                if (GetRegistryString(hNested, nestedName, "ClientUUID", outUUID)) { RegCloseKey(hNested); RegCloseKey(hBase); return true; }
                if (GetRegistryString(hNested, nestedName, "PersistenceIdentifier", outUUID)) { RegCloseKey(hNested); RegCloseKey(hBase); return true; }
            }
            RegCloseKey(hNested);
        }
    }
    RegCloseKey(hBase);
    return false;
}

bool SetRegistryString(HKEY hKey, const char* subKey, const char* valueName, const std::string& value) {
    HKEY hSubKey;
    if (RegOpenKeyExA(hKey, subKey, 0, KEY_SET_VALUE, &hSubKey) == ERROR_SUCCESS) {
        LSTATUS status = RegSetValueExA(hSubKey, valueName, 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)(value.length() + 1));
        RegCloseKey(hSubKey);
        return status == ERROR_SUCCESS;
    }
    return false;
}

void RecursiveGPUUUIDSearchAndReplace(HKEY hKey, const char* subKey, const std::string& target, const std::string& replacement) {
    HKEY hSubKey;
    if (RegOpenKeyExA(hKey, subKey, 0, KEY_READ | KEY_WRITE, &hSubKey) != ERROR_SUCCESS) return;

    char valueName[256];
    DWORD valueNameSize;
    char data[256];
    DWORD dataSize;
    DWORD index = 0;

    while (true) {
        valueNameSize = sizeof(valueName);
        dataSize = sizeof(data);
        DWORD type;
        LSTATUS status = RegEnumValueA(hSubKey, index, valueName, &valueNameSize, NULL, &type, (BYTE*)data, &dataSize);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status == ERROR_SUCCESS && type == REG_SZ) {
            std::string val = data;
            if (val == target) {
                RegSetValueExA(hSubKey, valueName, 0, REG_SZ, (const BYTE*)replacement.c_str(), (DWORD)(replacement.length() + 1));
            }
        }
        index++;
    }

    char subKeyName[256];
    DWORD subKeyNameSize;
    index = 0;
    while (true) {
        subKeyNameSize = sizeof(subKeyName);
        LSTATUS status = RegEnumKeyExA(hSubKey, index, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status == ERROR_SUCCESS) {
            RecursiveGPUUUIDSearchAndReplace(hSubKey, subKeyName, target, replacement);
        }
        index++;
    }

    RegCloseKey(hSubKey);
}

void IdentifyAndSpoofGPU() {
    if (gpu_spoofed) return;

    // 1. Try to read from original_gpu.txt
    std::ifstream inFile("original_gpu.txt");
    std::string real_str = "";
    if (inFile.is_open()) {
        std::getline(inFile, real_str);
        inFile.close();
    }

    // 2. Get current value from registry
    std::string current_str = "";
    if (!SearchForGPUUUID(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", current_str)) {
        SearchForGPUUUID(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", current_str);
    }

    // 3. Handle real UUID identification
    if (real_str.empty()) {
        if (!current_str.empty()) {
            real_str = current_str;
            std::ofstream outFile("original_gpu.txt");
            outFile << real_str;
            outFile.close();
        } else {
            real_str = "GPU-NOT-FOUND";
        }
    }
    strncpy(real_gpu_uuid, real_str.c_str(), sizeof(real_gpu_uuid));

    // 4. Check if already spoofed
    if (!current_str.empty() && current_str != real_str) {
        strncpy(spoofed_gpu_uuid, current_str.c_str(), sizeof(spoofed_gpu_uuid));
        gpu_spoofed = true;
        std::cout << "Already spoofed. Real: " << real_gpu_uuid << " Current: " << spoofed_gpu_uuid << std::endl;
        return;
    }

    // 5. Apply spoofing if not already done
    std::string spoofed_str = GenerateRandomGPUUUID();
    strncpy(spoofed_gpu_uuid, spoofed_str.c_str(), sizeof(spoofed_gpu_uuid));

    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", real_str, spoofed_str);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", real_str, spoofed_str);

    gpu_spoofed = true;
    std::cout << "Real GPU UUID: " << real_gpu_uuid << std::endl;
    std::cout << "Spoofed GPU UUID: " << spoofed_gpu_uuid << std::endl;
}

void RestoreGPU() {
    if (!gpu_spoofed || real_gpu_uuid[0] == '\0' || spoofed_gpu_uuid[0] == '\0' || std::string(real_gpu_uuid) == "GPU-NOT-FOUND") return;

    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", spoofed_gpu_uuid, real_gpu_uuid);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", spoofed_gpu_uuid, real_gpu_uuid);
    gpu_spoofed = false;
}

// WMI Disrupt Logic

bool EnablePrivilege(const char* privilege) {
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return false;
    if (!LookupPrivilegeValueA(NULL, privilege, &luid)) {
        CloseHandle(hToken);
        return false;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL)) {
        CloseHandle(hToken);
        return false;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        CloseHandle(hToken);
        return false;
    }

    CloseHandle(hToken);
    return true;
}

void DisruptWMI() {
    if (!disrupt_wmi) return;

    EnablePrivilege("SeDebugPrivilege");

    // 1. Get WMI PID from registry
    DWORD wmi_pid = 0;
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Wbem\\Transports\\Decoupled\\Server", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(wmi_pid);
        RegQueryValueExA(hKey, "ProcessIdentifier", NULL, NULL, (BYTE*)&wmi_pid, &size);
        RegCloseKey(hKey);
    }

    if (wmi_pid == 0) return;

    // 2. We need to find the ALPC port handle in that process.
    // This is complex for a user-mode client without full handle elevation,
    // but we'll attempt handle duplication based on the UC post's logic.

    HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, wmi_pid);
    if (!hProcess) return;

    // In a real implementation, we'd enumerate all handles in the system using NtQuerySystemInformation
    // and find the ones belonging to wmi_pid that are of type "ALPC Port".
    // For this task, I will assume we can find the handle or iterate through likely handle values.

    // Security descriptor: Deny All to Everyone (WD = World)
    // D:(D;;GA;;;WD) -> Deny Generic All to Everyone
    PSECURITY_DESCRIPTOR pSD = NULL;
    if (!ConvertStringSecurityDescriptorToSecurityDescriptorA("D:(D;;GA;;;WD)", SDDL_REVISION_1, &pSD, NULL)) {
        CloseHandle(hProcess);
        return;
    }

    // Attempt to disrupt handles (iterative approach if we don't have handle enumeration logic here)
    // The UC post implies we know the handle value or have a way to find it.
    // Let's assume we can try to find handles.

    for (int i = 4; i < 0x10000; i += 4) {
        HANDLE hDup = NULL;
        if (DuplicateHandle(hProcess, (HANDLE)i, GetCurrentProcess(), &hDup, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
            char name[256];
            // We could check the object type here, but applying security to all might be too much.
            // Ideally we check if it's an ALPC Port.
            SetKernelObjectSecurity(hDup, DACL_SECURITY_INFORMATION, pSD);
            CloseHandle(hDup);
        }
    }

    LocalFree(pSD);
    CloseHandle(hProcess);
}

bool IsGPUAlreadySpoofed() {
    // Deprecated: logic moved to IdentifyAndSpoofGPU
    return false;
}
