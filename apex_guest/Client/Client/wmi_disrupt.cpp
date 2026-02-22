#include "wmi_disrupt.h"
#include <iostream>
#include <vector>
#include <sddl.h>
#include <winternl.h>
#include <string>
#include <random>

#pragma comment(lib, "advapi32.lib")

std::wstring GenerateRandomUUIDW() {
    static const wchar_t hex_chars[] = L"0123456789abcdef";
    std::wstring uuid = L"";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    for (int i = 0; i < 8; ++i) uuid += hex_chars[dis(gen)];
    uuid += L"-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += L"-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += L"-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += L"-";
    for (int i = 0; i < 12; ++i) uuid += hex_chars[dis(gen)];

    return uuid;
}

bool SpoofMachineGuid() {
    std::wstring new_guid = GenerateRandomUUIDW();
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Cryptography", 0, KEY_SET_VALUE, &hKey) == ERROR_SUCCESS) {
        if (RegSetValueExW(hKey, L"MachineGuid", 0, REG_SZ, (LPBYTE)new_guid.c_str(), (new_guid.length() + 1) * sizeof(wchar_t)) == ERROR_SUCCESS) {
            printf("[+] Spoofed MachineGuid: %ws\n", new_guid.c_str());
            RegCloseKey(hKey);
            return true;
        }
        RegCloseKey(hKey);
    }
    printf("[-] Failed to spoof MachineGuid. Ensure running as Administrator.\n");
    return false;
}

bool EnablePrivilege(LPCWSTR lpPrivilege) {
    HANDLE hToken;
    LUID luid;
    TOKEN_PRIVILEGES tp;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;

    if (!LookupPrivilegeValueW(NULL, lpPrivilege, &luid)) {
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

// NT internal definitions
#define SystemHandleInformation 16

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

typedef NTSTATUS (NTAPI *pNtQuerySystemInformation)(
    SYSTEM_INFORMATION_CLASS SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
);

typedef NTSTATUS (NTAPI *pNtQueryObject)(
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
);

bool DisruptWMI() {
    printf("Attempting to disrupt WMI...\n");

    // Enable SeDebugPrivilege to allow opening system processes
    if (!EnablePrivilege(SE_DEBUG_NAME)) {
        printf("[-] Failed to enable SeDebugPrivilege. WMI disruption might fail.\n");
    } else {
        printf("[+] SeDebugPrivilege enabled.\n");
    }

    // 1. Identify WMI Service PID
    DWORD wmi_pid = 0;
    HKEY hKey;
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, L"SOFTWARE\\Microsoft\\Wbem\\Transports\\Decoupled\\Server", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(DWORD);
        RegQueryValueExW(hKey, L"ProcessIdentifier", NULL, NULL, (LPBYTE)&wmi_pid, &size);
        RegCloseKey(hKey);
    }

    if (wmi_pid == 0) {
        printf("[-] Failed to find WMI service PID in registry.\n");
        return false;
    }

    printf("[+] WMI service PID: %u\n", wmi_pid);

    // 2. Get ALPC Port Handle
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    auto NtQuerySystemInformation = (pNtQuerySystemInformation)GetProcAddress(ntdll, "NtQuerySystemInformation");
    auto NtQueryObject = (pNtQueryObject)GetProcAddress(ntdll, "NtQueryObject");

    if (!NtQuerySystemInformation || !NtQueryObject) return false;

    ULONG size = 0x10000;
    PSYSTEM_HANDLE_INFORMATION handle_info = (PSYSTEM_HANDLE_INFORMATION)malloc(size);
    while (NtQuerySystemInformation((SYSTEM_INFORMATION_CLASS)SystemHandleInformation, handle_info, size, &size) == 0xC0000004) { // STATUS_INFO_LENGTH_MISMATCH
        handle_info = (PSYSTEM_HANDLE_INFORMATION)realloc(handle_info, size);
    }

    HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, wmi_pid);
    if (!hProcess) {
        printf("[-] Failed to open WMI process for handle duplication. LastError: %u\n", GetLastError());
        free(handle_info);
        return false;
    }

    bool success = false;
    for (ULONG i = 0; i < handle_info->NumberOfHandles; i++) {
        if (handle_info->Handles[i].UniqueProcessId != wmi_pid) continue;

        HANDLE hDuplicated = NULL;
        if (DuplicateHandle(hProcess, (HANDLE)handle_info->Handles[i].HandleValue, GetCurrentProcess(), &hDuplicated, 0, FALSE, DUPLICATE_SAME_ACCESS)) {

            // Check object type
            BYTE type_buffer[1024];
            if (NtQueryObject(hDuplicated, (OBJECT_INFORMATION_CLASS)2, type_buffer, sizeof(type_buffer), NULL) >= 0) { // ObjectTypeInformation
                PUNICODE_STRING type_name = (PUNICODE_STRING)type_buffer;
                if (type_name->Buffer && wcsstr(type_name->Buffer, L"ALPC Port")) {
                    // Found an ALPC Port!

                    // 3. Apply Deny All security descriptor
                    PSECURITY_DESCRIPTOR sd = NULL;
                    if (ConvertStringSecurityDescriptorToSecurityDescriptorW(L"D:(A;;GA;;;WD)", SDDL_REVISION_1, &sd, NULL)) {
                        if (SetKernelObjectSecurity(hDuplicated, DACL_SECURITY_INFORMATION, sd)) {
                            printf("[+] Successfully restricted ALPC Port handle 0x%x in WMI process.\n", handle_info->Handles[i].HandleValue);
                            success = true;
                        }
                        LocalFree(sd);
                    }
                }
            }
            CloseHandle(hDuplicated);
        }
    }

    CloseHandle(hProcess);
    free(handle_info);

    if (success) {
        printf("[+] WMI disruption applied. WMIC queries should now fail.\n");
    } else {
        printf("[-] Failed to apply WMI disruption.\n");
    }

    return success;
}
