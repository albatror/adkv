#include "wmi_disrupt.h"
#include <windows.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <tlhelp32.h>
#include <aclapi.h>
#include <sddl.h>
#include <winternl.h>

#pragma comment(lib, "advapi32.lib")

// NT internal definitions for handle enumeration
typedef enum _SYSTEM_INFORMATION_CLASS_EX {
    SystemHandleInformation = 16
} SYSTEM_INFORMATION_CLASS_EX;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;
    UCHAR HandleAttributes;
    USHORT HandleValue;
    PVOID Object;
    ULONG GrantedAccess;
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1];
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

extern "C" NTSTATUS NTAPI NtQuerySystemInformation(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

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

            // Iterate through subkeys like 0000, 0001
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

    // Also check GridLicensing
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global\\GridLicensing", "ClientUUID", fake_uuid);
}

bool IdentifyAndSpoofGPU() {
    std::ifstream infile("original_gpu.txt");
    if (infile.is_open()) return true;

    // Search for any existing GPU-UUID to save as original
    const char* class_path = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\0000";
    std::string real_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, class_path, "GPU-UUID");

    if (real_uuid.empty()) {
        // Fallback search
        real_uuid = GetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global\\GridLicensing", "ClientUUID");
    }

    if (!real_uuid.empty()) {
        std::ofstream outfile("original_gpu.txt");
        outfile << real_uuid;
        outfile.close();
    }
    return true;
}

bool SetPrivilege(LPCSTR lpszPrivilege, BOOL bEnablePrivilege) {
    TOKEN_PRIVILEGES tp;
    LUID luid;
    HANDLE hToken;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) return FALSE;
    if (!LookupPrivilegeValueA(NULL, lpszPrivilege, &luid)) { CloseHandle(hToken); return FALSE; }
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = (bEnablePrivilege) ? SE_PRIVILEGE_ENABLED : 0;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) { CloseHandle(hToken); return FALSE; }
    CloseHandle(hToken);
    return TRUE;
}

bool DisruptWMI() {
    if (!SetPrivilege(SE_DEBUG_NAME, TRUE)) return false;

    // Get WMI Service PID from registry
    HKEY hKey;
    DWORD wmiPid = 0;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Wbem\\Transports\\Decoupled\\Server", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD dwSize = sizeof(DWORD);
        RegQueryValueExA(hKey, "ProcessIdentifier", NULL, NULL, (LPBYTE)&wmiPid, &dwSize);
        RegCloseKey(hKey);
    }

    if (wmiPid == 0) {
        // Fallback: find wmiprvse.exe
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(PROCESSENTRY32);
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        if (Process32First(snapshot, &entry)) {
            while (Process32Next(snapshot, &entry)) {
                if (stricmp(entry.szExeFile, "wmiprvse.exe") == 0) {
                    wmiPid = entry.th32ProcessID;
                    break;
                }
            }
        }
        CloseHandle(snapshot);
    }

    if (wmiPid == 0) return false;

    HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, wmiPid);
    if (!hProcess) return false;

    ULONG handleInfoSize = 0x10000;
    PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(handleInfoSize);
    while (NtQuerySystemInformation(SystemHandleInformation, handleInfo, handleInfoSize, NULL) == 0xC0000004) {
        handleInfoSize *= 2;
        handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, handleInfoSize);
    }

    const char* sddl = "D:(D;;GA;;;WD)";
    PSECURITY_DESCRIPTOR sd = NULL;
    ConvertStringSecurityDescriptorToSecurityDescriptorA(sddl, SDDL_REVISION_1, &sd, NULL);

    for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
        if (handleInfo->Handles[i].UniqueProcessId == wmiPid) {
            HANDLE hDup = NULL;
            if (DuplicateHandle(hProcess, (HANDLE)handleInfo->Handles[i].HandleValue, GetCurrentProcess(), &hDup, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
                // If it's an ALPC port, we block it. We can't easily check type without more calls,
                // so we apply the SD to all likely candidates or just everything for disruption.
                SetKernelObjectSecurity(hDup, DACL_SECURITY_INFORMATION, sd);
                CloseHandle(hDup);
            }
        }
    }

    free(handleInfo);
    LocalFree(sd);
    CloseHandle(hProcess);
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
