#include "wmi_disrupt.h"
#include <iostream>
#include <vector>
#include <winternl.h>
#include <sddl.h>
#include <accctrl.h>
#include <time.h>
#include <aclapi.h>

#pragma comment(lib, "advapi32.lib")

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

typedef enum _OBJECT_INFORMATION_CLASS {
    ObjectBasicInformation,
    ObjectNameInformation,
    ObjectTypeInformation,
    ObjectAllInformation,
    ObjectDataInformation
} OBJECT_INFORMATION_CLASS;

typedef struct _PUBLIC_OBJECT_TYPE_INFORMATION {
    UNICODE_STRING TypeName;
    ULONG Reserved[22];
} PUBLIC_OBJECT_TYPE_INFORMATION, *PPUBLIC_OBJECT_TYPE_INFORMATION;

typedef NTSTATUS(NTAPI* tNtQueryObject)(
    HANDLE Handle,
    OBJECT_INFORMATION_CLASS ObjectInformationClass,
    PVOID ObjectInformation,
    ULONG ObjectInformationLength,
    PULONG ReturnLength
    );

typedef NTSTATUS(NTAPI* tNtQuerySystemInformation)(
    ULONG SystemInformationClass,
    PVOID SystemInformation,
    ULONG SystemInformationLength,
    PULONG ReturnLength
    );

bool DisruptWMI() {
    DWORD wmi_pid = 0;
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Wbem\\Transports\\Decoupled\\Server", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(wmi_pid);
        RegQueryValueExA(hKey, "ProcessIdentifier", NULL, NULL, (LPBYTE)&wmi_pid, &size);
        RegCloseKey(hKey);
    }

    if (wmi_pid == 0) return false;

    HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, wmi_pid);
    if (!hProcess) return false;

    HMODULE hNtdll = GetModuleHandleA("ntdll.dll");
    auto NtQuerySystemInformation = (tNtQuerySystemInformation)GetProcAddress(hNtdll, "NtQuerySystemInformation");
    auto NtQueryObject = (tNtQueryObject)GetProcAddress(hNtdll, "NtQueryObject");

    ULONG size = 0x10000;
    PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(size);
    while (NtQuerySystemInformation(SystemHandleInformation, handleInfo, size, &size) == ((NTSTATUS)0xC0000004L)) {
        handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, size);
    }

    bool success = false;
    for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
        if (handleInfo->Handles[i].UniqueProcessId == wmi_pid) {
            HANDLE hDuplicated = NULL;
            if (DuplicateHandle(hProcess, (HANDLE)handleInfo->Handles[i].HandleValue, GetCurrentProcess(), &hDuplicated, 0, FALSE, DUPLICATE_SAME_ACCESS)) {

                // Check if it's an ALPC Port
                char typeBuffer[1024];
                ULONG typeSize;
                if (NtQueryObject(hDuplicated, ObjectTypeInformation, typeBuffer, sizeof(typeBuffer), &typeSize) >= 0) {
                    PPUBLIC_OBJECT_TYPE_INFORMATION typeInfo = (PPUBLIC_OBJECT_TYPE_INFORMATION)typeBuffer;
                    if (typeInfo->TypeName.Buffer && wcsstr(typeInfo->TypeName.Buffer, L"ALPC Port")) {
                        PSECURITY_DESCRIPTOR pSD = NULL;
                        // D:(D;;GA;;;WD) -> Deny all access to Everyone
                        if (ConvertStringSecurityDescriptorToSecurityDescriptorA("D:(D;;GA;;;WD)", SDDL_REVISION_1, &pSD, NULL)) {
                            if (SetKernelObjectSecurity(hDuplicated, DACL_SECURITY_INFORMATION, pSD)) {
                                success = true;
                            }
                            LocalFree(pSD);
                        }
                    }
                }
                CloseHandle(hDuplicated);
            }
        }
    }

    free(handleInfo);
    CloseHandle(hProcess);
    return success;
}

std::string RandomString(int len) {
    static const char alphanum[] = "0123456789abcdef";
    std::string s = "";
    for (int i = 0; i < len; ++i) {
        s += alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    return s;
}

std::string GenerateMachineGuid() {
    return RandomString(8) + "-" + RandomString(4) + "-" + RandomString(4) + "-" + RandomString(4) + "-" + RandomString(12);
}

void ApplyRegistrySpoofs() {
    static bool seeded = false;
    if (!seeded) {
        srand((unsigned int)time(NULL));
        seeded = true;
    }

    HKEY hKey;
    std::string newGuid = GenerateMachineGuid();
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
        RegSetValueExA(hKey, "MachineGuid", 0, REG_SZ, (const BYTE*)newGuid.c_str(), newGuid.length() + 1);
        RegCloseKey(hKey);
    }

    // NVIDIA Spoofing
    // Mask NVIDIA-specific ClientUUID, PersistenceIdentifier, and GPU-UUID entries
    const char* nvidiaKeys[] = {
        "SYSTEM\\CurrentControlSet\\Control\\Video",
        "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}"
    };

    for (const char* keyRoot : nvidiaKeys) {
        if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, keyRoot, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
            char subKeyName[256];
            DWORD subKeySize = sizeof(subKeyName);
            for (DWORD i = 0; RegEnumKeyExA(hKey, i, subKeyName, &subKeySize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; i++) {
                subKeySize = sizeof(subKeyName);
                HKEY hSubKey;
                if (RegOpenKeyExA(hKey, subKeyName, 0, KEY_ALL_ACCESS, &hSubKey) == ERROR_SUCCESS) {
                    std::string newUUID = "GPU-" + GenerateMachineGuid();
                    RegSetValueExA(hSubKey, "GPU-UUID", 0, REG_SZ, (const BYTE*)newUUID.c_str(), newUUID.length() + 1);

                    std::string newClientUUID = "{" + GenerateMachineGuid() + "}";
                    RegSetValueExA(hSubKey, "ClientUUID", 0, REG_SZ, (const BYTE*)newClientUUID.c_str(), newClientUUID.length() + 1);

                    RegCloseKey(hSubKey);
                }
            }
            RegCloseKey(hKey);
        }
    }
}

std::string GetRealMachineGuid() {
    char guid[256] = { 0 };
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        DWORD size = sizeof(guid);
        RegQueryValueExA(hKey, "MachineGuid", NULL, NULL, (LPBYTE)guid, &size);
        RegCloseKey(hKey);
    }
    return std::string(guid);
}
