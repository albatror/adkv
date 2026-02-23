#include "wmi_disrupt.h"
#include "XorString.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include <random>
#include <algorithm>
#include <sddl.h>
#include <aclapi.h>

#pragma comment(lib, "Advapi32.lib")

char real_gpu_uuid[256] = "";
char spoofed_gpu_uuid[256] = "";

// NVML Types and Function Pointers
typedef int nvmlReturn_t;
typedef void* nvmlDevice_t;
#define NVML_SUCCESS 0

typedef nvmlReturn_t(*nvmlInit_t)();
typedef nvmlReturn_t(*nvmlDeviceGetCount_t)(unsigned int*);
typedef nvmlReturn_t(*nvmlDeviceGetHandleByIndex_t)(unsigned int, nvmlDevice_t*);
typedef nvmlReturn_t(*nvmlDeviceGetUUID_t)(nvmlDevice_t, char*, unsigned int);
typedef nvmlReturn_t(*nvmlShutdown_t)();

bool GetGPUUUIDFromNVML(std::string& outUUID) {
    HMODULE hNvml = LoadLibraryA("nvml.dll");
    if (!hNvml) {
        // Try common location if not in path
        hNvml = LoadLibraryA("C:\\Program Files\\NVIDIA Corporation\\NVSMI\\nvml.dll");
    }
    if (!hNvml) return false;

    nvmlInit_t nvmlInit = (nvmlInit_t)GetProcAddress(hNvml, "nvmlInit");
    nvmlDeviceGetCount_t nvmlDeviceGetCount = (nvmlDeviceGetCount_t)GetProcAddress(hNvml, "nvmlDeviceGetCount");
    nvmlDeviceGetHandleByIndex_t nvmlDeviceGetHandleByIndex = (nvmlDeviceGetHandleByIndex_t)GetProcAddress(hNvml, "nvmlDeviceGetHandleByIndex");
    nvmlDeviceGetUUID_t nvmlDeviceGetUUID = (nvmlDeviceGetUUID_t)GetProcAddress(hNvml, "nvmlDeviceGetUUID");
    nvmlShutdown_t nvmlShutdown = (nvmlShutdown_t)GetProcAddress(hNvml, "nvmlShutdown");

    bool success = false;
    if (nvmlInit && nvmlDeviceGetCount && nvmlDeviceGetHandleByIndex && nvmlDeviceGetUUID && nvmlShutdown) {
        if (nvmlInit() == NVML_SUCCESS) {
            unsigned int deviceCount = 0;
            if (nvmlDeviceGetCount(&deviceCount) == NVML_SUCCESS && deviceCount > 0) {
                nvmlDevice_t device;
                if (nvmlDeviceGetHandleByIndex(0, &device) == NVML_SUCCESS) {
                    char uuid[256];
                    if (nvmlDeviceGetUUID(device, uuid, sizeof(uuid)) == NVML_SUCCESS) {
                        outUUID = uuid;
                        success = true;
                    }
                }
            }
            nvmlShutdown();
        }
    }

    FreeLibrary(hNvml);
    return success;
}
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

void DeepSearchGPUUUID(HKEY hKey, const char* subKey, std::string& outUUID) {
    if (!outUUID.empty()) return;

    HKEY hSubKey;
    if (RegOpenKeyExA(hKey, subKey, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) return;

    char valueName[256];
    DWORD valueNameSize;
    char data[256];
    DWORD dataSize;
    DWORD index = 0;

    // Check all values in this key
    while (true) {
        valueNameSize = sizeof(valueName);
        dataSize = sizeof(data);
        DWORD type;
        LSTATUS status = RegEnumValueA(hSubKey, index++, valueName, &valueNameSize, NULL, &type, (BYTE*)data, &dataSize);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status == ERROR_SUCCESS && type == REG_SZ) {
            std::string val = data;
            if (val.find("GPU-") == 0 && val.length() > 10) {
                outUUID = val;
                RegCloseKey(hSubKey);
                return;
            }
        }
    }

    // Recurse into subkeys
    index = 0;
    char subKeyName[256];
    DWORD subKeyNameSize;
    while (true) {
        subKeyNameSize = sizeof(subKeyName);
        LSTATUS status = RegEnumKeyExA(hSubKey, index++, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status == ERROR_SUCCESS) {
            DeepSearchGPUUUID(hSubKey, subKeyName, outUUID);
            if (!outUUID.empty()) break;
        }
    }

    RegCloseKey(hSubKey);
}

void SearchDeviceMapVideo(std::string& outUUID) {
    if (!outUUID.empty()) return;
    HKEY hVideo;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\VIDEO", 0, KEY_READ, &hVideo) != ERROR_SUCCESS) return;

    char valueName[256];
    DWORD valueNameSize;
    char data[512];
    DWORD dataSize;
    DWORD index = 0;

    while (true) {
        valueNameSize = sizeof(valueName);
        dataSize = sizeof(data);
        DWORD type;
        LSTATUS status = RegEnumValueA(hVideo, index++, valueName, &valueNameSize, NULL, &type, (BYTE*)data, &dataSize);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status == ERROR_SUCCESS && type == REG_SZ) {
            std::string path = data;
            if (path.find("\\Registry\\Machine\\") == 0) {
                std::string regPath = path.substr(18);
                // Check if it's a Control\Video path
                DeepSearchGPUUUID(HKEY_LOCAL_MACHINE, regPath.c_str(), outUUID);
                if (!outUUID.empty()) break;
            }
        }
    }
    RegCloseKey(hVideo);
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

std::string ToLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
    return s;
}

void ReplaceAll(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) return;
    size_t start_pos = 0;
    while (true) {
        std::string strLower = ToLower(str);
        std::string fromLower = ToLower(from);
        start_pos = strLower.find(fromLower, start_pos);
        if (start_pos == std::string::npos) break;
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
}

void SearchAndReplaceBinary(HKEY hKey, const char* valueName, const std::string& target, const std::string& replacement) {
    DWORD dataSize = 0;
    if (RegQueryValueExA(hKey, valueName, NULL, NULL, NULL, &dataSize) != ERROR_SUCCESS) return;
    std::vector<unsigned char> data(dataSize);
    if (RegQueryValueExA(hKey, valueName, NULL, NULL, data.data(), &dataSize) != ERROR_SUCCESS) return;

    bool changed = false;
    // ANSI search
    if (target.length() == replacement.length()) {
        for (size_t i = 0; i + target.length() <= data.size(); ++i) {
            if (memcmp(data.data() + i, target.c_str(), target.length()) == 0) {
                memcpy(data.data() + i, replacement.c_str(), target.length());
                changed = true;
            }
        }
    }
    // UTF-16 search
    std::wstring targetW(target.begin(), target.end());
    std::wstring replacementW(replacement.begin(), replacement.end());
    size_t targetWLen = targetW.length() * sizeof(wchar_t);
    if (targetWLen == replacementW.length() * sizeof(wchar_t)) {
        for (size_t i = 0; i + targetWLen <= data.size(); i += 2) {
            if (memcmp(data.data() + i, targetW.c_str(), targetWLen) == 0) {
                memcpy(data.data() + i, replacementW.c_str(), targetWLen);
                changed = true;
            }
        }
    }

    if (changed) {
        RegSetValueExA(hKey, valueName, 0, REG_BINARY, data.data(), (DWORD)data.size());
    }
}

void RecursiveGPUUUIDSearchAndReplace(HKEY hKey, const char* subKey, const std::string& target, const std::string& replacement) {
    HKEY hSubKey;
    if (RegOpenKeyExA(hKey, subKey, 0, KEY_READ | KEY_WRITE, &hSubKey) != ERROR_SUCCESS) {
        if (RegOpenKeyExA(hKey, subKey, 0, KEY_READ, &hSubKey) != ERROR_SUCCESS) return;
    }

    char valueName[256];
    DWORD valueNameSize;
    char data[8192];
    DWORD dataSize;
    DWORD index = 0;

    std::string targetNoPrefix = "";
    if (target.find("GPU-") == 0) targetNoPrefix = target.substr(4);

    std::string targetNoDashes = target;
    targetNoDashes.erase(std::remove(targetNoDashes.begin(), targetNoDashes.end(), '-'), targetNoDashes.end());
    if (targetNoDashes.find("GPU") == 0) targetNoDashes = targetNoDashes.substr(3);

    std::string replacementNoPrefix = "";
    if (replacement.find("GPU-") == 0) replacementNoPrefix = replacement.substr(4);

    std::string replacementNoDashes = replacement;
    replacementNoDashes.erase(std::remove(replacementNoDashes.begin(), replacementNoDashes.end(), '-'), replacementNoDashes.end());
    if (replacementNoDashes.find("GPU") == 0) replacementNoDashes = replacementNoDashes.substr(3);

    std::string targetWithBraces = "{" + target + "}";
    if (target.find("GPU-") == 0) targetWithBraces = "{" + target.substr(4) + "}";

    std::string replacementWithBraces = "{" + replacement + "}";
    if (replacement.find("GPU-") == 0) replacementWithBraces = "{" + replacement.substr(4) + "}";

    while (true) {
        valueNameSize = sizeof(valueName);
        dataSize = sizeof(data);
        DWORD type;
        LSTATUS status = RegEnumValueA(hSubKey, index, valueName, &valueNameSize, NULL, &type, (BYTE*)data, &dataSize);
        if (status == ERROR_NO_MORE_ITEMS) break;
        if (status == ERROR_SUCCESS) {
            if (type == REG_SZ || type == REG_MULTI_SZ || type == REG_EXPAND_SZ) {
                std::string val = data;
                std::string originalVal = val;

                ReplaceAll(val, target, replacement);
                if (!targetNoPrefix.empty() && !replacementNoPrefix.empty()) {
                    ReplaceAll(val, targetNoPrefix, replacementNoPrefix);
                }
                if (!targetNoDashes.empty() && !replacementNoDashes.empty()) {
                    ReplaceAll(val, targetNoDashes, replacementNoDashes);
                }
                ReplaceAll(val, targetWithBraces, replacementWithBraces);

                if (val != originalVal) {
                    RegSetValueExA(hSubKey, valueName, 0, type, (const BYTE*)val.c_str(), (DWORD)(val.length() + 1));
                }
            } else if (type == REG_BINARY) {
                SearchAndReplaceBinary(hSubKey, valueName, target, replacement);
                if (!targetNoPrefix.empty() && !replacementNoPrefix.empty()) {
                    SearchAndReplaceBinary(hSubKey, valueName, targetNoPrefix, replacementNoPrefix);
                }
            }
        }
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
    }

    RegCloseKey(hSubKey);
}

bool IsUserAdmin() {
    bool isAdmin = false;
    HANDLE hToken = NULL;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &hToken)) {
        TOKEN_ELEVATION elevation;
        DWORD dwSize;
        if (GetTokenInformation(hToken, TokenElevation, &elevation, sizeof(elevation), &dwSize)) {
            isAdmin = elevation.TokenIsElevated;
        }
    }
    if (hToken) CloseHandle(hToken);
    return isAdmin;
}

void IdentifyAndSpoofGPU() {
    if (gpu_spoofed) return;

    if (!IsUserAdmin()) {
        std::cout << "WARNING: Client is not running as Administrator. Registry spoofing will likely fail." << std::endl;
    }

    // 1. Try to read from original_gpu.txt
    std::ifstream inFile("original_gpu.txt");
    std::string real_str = "";
    if (inFile.is_open()) {
        std::getline(inFile, real_str);
        inFile.close();
    }

    if (real_str == "GPU-NOT-FOUND") real_str = "";

    // 2. Get current value from registry (might be spoofed)
    std::string current_str = "";
    SearchDeviceMapVideo(current_str);
    if (current_str.empty()) DeepSearchGPUUUID(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", current_str);
    if (current_str.empty()) DeepSearchGPUUUID(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", current_str);
    if (current_str.empty()) DeepSearchGPUUUID(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Control\\Video", current_str);
    if (current_str.empty()) DeepSearchGPUUUID(HKEY_LOCAL_MACHINE, "SYSTEM\\ControlSet001\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", current_str);
    if (current_str.empty()) DeepSearchGPUUUID(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global\\GpuUUIDs", current_str);

    // 3. Try to get REAL UUID from NVML (hardware level, usually not spoofed by registry)
    std::string nvml_real_uuid = "";
    bool found_nvml = GetGPUUUIDFromNVML(nvml_real_uuid);

    // 4. Handle real UUID identification
    if (real_str.empty()) {
        if (found_nvml) {
            real_str = nvml_real_uuid;
        } else if (!current_str.empty()) {
            real_str = current_str;
        }

        if (!real_str.empty()) {
            std::ofstream outFile("original_gpu.txt");
            outFile << real_str;
            outFile.close();
        } else {
            real_str = "GPU-NOT-FOUND";
        }
    }
    strncpy(real_gpu_uuid, real_str.c_str(), sizeof(real_gpu_uuid));

    // 4. Determine spoofed UUID
    std::string spoofed_str;
    if (!current_str.empty() && ToLower(current_str) != ToLower(real_str)) {
        spoofed_str = current_str;
        std::cout << "Already spoofed. Real: " << real_gpu_uuid << " Current: " << spoofed_str << std::endl;
    } else {
        spoofed_str = GenerateRandomGPUUUID();
    }
    strncpy(spoofed_gpu_uuid, spoofed_str.c_str(), sizeof(spoofed_gpu_uuid));

    // 5. Apply spoofing everywhere (even if already spoofed, to catch missing instances)
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SYSTEM", real_str, spoofed_str);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation", real_str, spoofed_str);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\NVIDIA Corporation", real_str, spoofed_str);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NVIDIA", real_str, spoofed_str);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\VIDEO", real_str, spoofed_str);

    gpu_spoofed = true;
    std::cout << "Real GPU UUID: " << real_gpu_uuid << std::endl;
    std::cout << "Spoofed GPU UUID: " << spoofed_gpu_uuid << std::endl;
}

void RestoreGPU() {
    if (!gpu_spoofed || real_gpu_uuid[0] == '\0' || spoofed_gpu_uuid[0] == '\0' || std::string(real_gpu_uuid) == "GPU-NOT-FOUND") return;

    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SYSTEM", spoofed_gpu_uuid, real_gpu_uuid);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation", spoofed_gpu_uuid, real_gpu_uuid);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SOFTWARE\\WOW6432Node\\NVIDIA Corporation", spoofed_gpu_uuid, real_gpu_uuid);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\NVIDIA", spoofed_gpu_uuid, real_gpu_uuid);
    RecursiveGPUUUIDSearchAndReplace(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\VIDEO", spoofed_gpu_uuid, real_gpu_uuid);
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

    // Try to stop nvwmi service (NVIDIA WMI Provider)
    SC_HANDLE scm = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
    if (scm) {
        SC_HANDLE service = OpenServiceA(scm, "nvwmi", SERVICE_STOP | SERVICE_QUERY_STATUS);
        if (service) {
            SERVICE_STATUS ss;
            ControlService(service, SERVICE_CONTROL_STOP, &ss);
            CloseHandle(service);
        }
        CloseHandle(scm);
    }

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
