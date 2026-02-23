#include "wmi_disrupt.h"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iphlpapi.h>
#include <winternl.h>
#include <sddl.h>
#include <accctrl.h>
#include <aclapi.h>
#include <tlhelp32.h>
#include <ctime>

#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "Advapi32.lib")

HWIDData g_hwid;

std::string RandomString(size_t length, bool hex) {
    const char* chars = hex ? "0123456789abcdef" : "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string str;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, (hex ? 15 : 61));
    for (size_t i = 0; i < length; ++i) {
        str += chars[distribution(generator)];
    }
    return str;
}

std::string GenerateGUID() {
    return RandomString(8, true) + "-" + RandomString(4, true) + "-" + RandomString(4, true) + "-" + RandomString(4, true) + "-" + RandomString(12, true);
}

std::string GetRegistryString(HKEY hKey, const char* subKey, const char* valueName) {
    char buffer[512];
    DWORD bufferSize = sizeof(buffer);
    if (RegGetValueA(hKey, subKey, valueName, RRF_RT_REG_SZ, NULL, buffer, &bufferSize) == ERROR_SUCCESS) {
        return std::string(buffer);
    }
    return "Unknown";
}

void SetRegistryString(HKEY hKey, const char* subKey, const char* valueName, const std::string& value) {
    HKEY hOpenedKey;
    if (RegCreateKeyExA(hKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hOpenedKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hOpenedKey, valueName, 0, REG_SZ, (const BYTE*)value.c_str(), (DWORD)(value.length() + 1));
        RegCloseKey(hOpenedKey);
    }
}

void SetRegistryDWORD(HKEY hKey, const char* subKey, const char* valueName, DWORD value) {
    HKEY hOpenedKey;
    if (RegCreateKeyExA(hKey, subKey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hOpenedKey, NULL) == ERROR_SUCCESS) {
        RegSetValueExA(hOpenedKey, valueName, 0, REG_DWORD, (const BYTE*)&value, sizeof(DWORD));
        RegCloseKey(hOpenedKey);
    }
}

void GetRealRegistryIDs() {
    g_hwid.machine_guid_real = GetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", "MachineGuid");
    g_hwid.hw_profile_guid_real = GetRegistryString(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001", "HwProfileGuid");
    g_hwid.machine_id_real = GetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\SQMClient", "MachineId");
    g_hwid.computer_hardware_id_real = GetRegistryString(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", "ComputerHardwareId");
    g_hwid.product_id_real = GetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductID");

    g_hwid.mac_real = GetRealMAC();

    // BIOS
    g_hwid.bios_serial_real = GetRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardSerialNumber");
    if (g_hwid.bios_serial_real == "Unknown")
        g_hwid.bios_serial_real = GetRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "SystemSerialNumber");

    // GPU
    g_hwid.nv_uuid_real = "Unknown";
    HKEY hVideoKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", 0, KEY_READ, &hVideoKey) == ERROR_SUCCESS) {
        char subKeyName[255];
        DWORD subKeyNameSize;
        for (DWORD i = 0; subKeyNameSize = sizeof(subKeyName), RegEnumKeyExA(hVideoKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            std::string subKeyPath = "SYSTEM\\CurrentControlSet\\Control\\Video\\" + std::string(subKeyName);
            HKEY hGuidKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &hGuidKey) == ERROR_SUCCESS) {
                char adapterSubKey[255];
                DWORD adapterSubKeySize;
                for (DWORD j = 0; adapterSubKeySize = sizeof(adapterSubKey), RegEnumKeyExA(hGuidKey, j, adapterSubKey, &adapterSubKeySize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++j) {
                    std::string finalPath = subKeyPath + "\\" + std::string(adapterSubKey);
                    std::string val = GetRegistryString(HKEY_LOCAL_MACHINE, finalPath.c_str(), "GPU-UUID");
                    if (val != "Unknown" && val.find("GPU-") != std::string::npos) {
                        g_hwid.nv_uuid_real = val;
                        break;
                    }
                    val = GetRegistryString(HKEY_LOCAL_MACHINE, finalPath.c_str(), "GPUDeviceGuid");
                    if (val != "Unknown" && val.length() > 10) {
                        g_hwid.nv_uuid_real = "GPU-" + val.substr(val.find('{') != std::string::npos ? 1 : 0, 36);
                        break;
                    }
                }
                RegCloseKey(hGuidKey);
            }
            if (g_hwid.nv_uuid_real != "Unknown") break;
        }
        RegCloseKey(hVideoKey);
    }

    // Disk
    g_hwid.disk_serial_real = "Unknown";
    g_hwid.disk_id_real = "Unknown";
    HKEY hScsiKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi", 0, KEY_READ, &hScsiKey) == ERROR_SUCCESS) {
        char portName[255];
        DWORD portNameSize;
        for (DWORD i = 0; portNameSize = sizeof(portName), RegEnumKeyExA(hScsiKey, i, portName, &portNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            std::string portPath = "HARDWARE\\DEVICEMAP\\Scsi\\" + std::string(portName);
            HKEY hPortKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, portPath.c_str(), 0, KEY_READ, &hPortKey) == ERROR_SUCCESS) {
                char busName[255];
                DWORD busNameSize;
                for (DWORD j = 0; busNameSize = sizeof(busName), RegEnumKeyExA(hPortKey, j, busName, &busNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++j) {
                    std::string busPath = portPath + "\\" + std::string(busName);
                    HKEY hBusKey;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, busPath.c_str(), 0, KEY_READ, &hBusKey) == ERROR_SUCCESS) {
                        char targetName[255];
                        DWORD targetNameSize;
                        for (DWORD k = 0; targetNameSize = sizeof(targetName), RegEnumKeyExA(hBusKey, k, targetName, &targetNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++k) {
                            std::string targetPath = busPath + "\\" + std::string(targetName);
                            HKEY hTargetKey;
                            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, targetPath.c_str(), 0, KEY_READ, &hTargetKey) == ERROR_SUCCESS) {
                                char lunName[255];
                                DWORD lunNameSize;
                                for (DWORD l = 0; lunNameSize = sizeof(lunName), RegEnumKeyExA(hTargetKey, l, lunName, &lunNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++l) {
                                    std::string lunPath = targetPath + "\\" + std::string(lunName);
                                    std::string sn = GetRegistryString(HKEY_LOCAL_MACHINE, lunPath.c_str(), "SerialNumber");
                                    if (sn != "Unknown" && g_hwid.disk_serial_real == "Unknown") {
                                        g_hwid.disk_serial_real = sn;
                                        g_hwid.disk_id_real = GetRegistryString(HKEY_LOCAL_MACHINE, lunPath.c_str(), "Identifier");
                                    }
                                }
                                RegCloseKey(hTargetKey);
                            }
                        }
                        RegCloseKey(hBusKey);
                    }
                }
                RegCloseKey(hPortKey);
            }
        }
        RegCloseKey(hScsiKey);
    }
    if (g_hwid.disk_id_real == "Unknown") {
         g_hwid.disk_id_real = GetRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter\\0\\DiskController\\0\\DiskPeripheral\\0", "Identifier");
    }
}

std::string GetRealMAC() {
    PIP_ADAPTER_INFO AdapterInfo;
    DWORD dwBufLen = sizeof(IP_ADAPTER_INFO);
    char mac_addr[18];

    AdapterInfo = (IP_ADAPTER_INFO*)malloc(sizeof(IP_ADAPTER_INFO));
    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == ERROR_BUFFER_OVERFLOW) {
        free(AdapterInfo);
        AdapterInfo = (IP_ADAPTER_INFO*)malloc(dwBufLen);
    }

    if (GetAdaptersInfo(AdapterInfo, &dwBufLen) == NO_ERROR) {
        PIP_ADAPTER_INFO pAdapterInfo = AdapterInfo;
        while (pAdapterInfo) {
            if (pAdapterInfo->Type == MIB_IF_TYPE_ETHERNET || pAdapterInfo->Type == IF_TYPE_IEEE80211) {
                sprintf_s(mac_addr, "%02X:%02X:%02X:%02X:%02X:%02X",
                    pAdapterInfo->Address[0], pAdapterInfo->Address[1], pAdapterInfo->Address[2],
                    pAdapterInfo->Address[3], pAdapterInfo->Address[4], pAdapterInfo->Address[5]);
                free(AdapterInfo);
                return std::string(mac_addr);
            }
            pAdapterInfo = pAdapterInfo->Next;
        }
    }
    if (AdapterInfo) free(AdapterInfo);
    return "00:00:00:00:00:00";
}

std::string GenerateSpoofMAC() {
    const char* ouis[] = { "00:15:5D", "00:1A:A0", "00:1C:C0", "00:01:E3", "00:07:32", "00:0B:82", "00:E0:4C" };
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 6);
    std::string mac = ouis[dis(gen)];
    char suffix[10];
    std::uniform_int_distribution<> disHex(0, 255);
    sprintf_s(suffix, ":%02X:%02X:%02X", disHex(gen), disHex(gen), disHex(gen));
    mac += suffix;
    return mac;
}

bool IsAlreadyPatched() {
    DWORD dwVal = 0;
    DWORD dwSize = sizeof(dwVal);
    if (RegGetValueA(HKEY_CURRENT_USER, "Software\\ApexGuest", "Spoofed", RRF_RT_REG_DWORD, NULL, &dwVal, &dwSize) == ERROR_SUCCESS) {
        return dwVal == 1;
    }
    return false;
}

void SaveSpoofedIDsToHKCU() {
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\ApexGuest", 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        auto save = [&](const char* name, const std::string& val) {
            RegSetValueExA(hKey, name, 0, REG_SZ, (const BYTE*)val.c_str(), (DWORD)(val.length() + 1));
        };
        save("machine_guid_spoof", g_hwid.machine_guid_spoof);
        save("hw_profile_guid_spoof", g_hwid.hw_profile_guid_spoof);
        save("machine_id_spoof", g_hwid.machine_id_spoof);
        save("computer_hardware_id_spoof", g_hwid.computer_hardware_id_spoof);
        save("product_id_spoof", g_hwid.product_id_spoof);
        save("mac_spoof", g_hwid.mac_spoof);
        save("bios_serial_spoof", g_hwid.bios_serial_spoof);
        save("nv_uuid_spoof", g_hwid.nv_uuid_spoof);
        save("disk_id_spoof", g_hwid.disk_id_spoof);
        save("disk_serial_spoof", g_hwid.disk_serial_spoof);
        RegCloseKey(hKey);
    }
}

void LoadCurrentIDsAsSpoofed() {
    auto load = [&](const char* name) -> std::string {
        char buffer[512];
        DWORD bufferSize = sizeof(buffer);
        if (RegGetValueA(HKEY_CURRENT_USER, "Software\\ApexGuest", name, RRF_RT_REG_SZ, NULL, buffer, &bufferSize) == ERROR_SUCCESS) {
            return std::string(buffer);
        }
        return "N/A";
    };
    g_hwid.machine_guid_spoof = load("machine_guid_spoof");
    g_hwid.hw_profile_guid_spoof = load("hw_profile_guid_spoof");
    g_hwid.machine_id_spoof = load("machine_id_spoof");
    g_hwid.computer_hardware_id_spoof = load("computer_hardware_id_spoof");
    g_hwid.product_id_spoof = load("product_id_spoof");
    g_hwid.mac_spoof = load("mac_spoof");
    g_hwid.bios_serial_spoof = load("bios_serial_spoof");
    g_hwid.nv_uuid_spoof = load("nv_uuid_spoof");
    g_hwid.disk_id_spoof = load("disk_id_spoof");
    g_hwid.disk_serial_spoof = load("disk_serial_spoof");
}

void ApplyRegistrySpoofs(void* unused1, void* unused2) {
    std::cout << "Starting Registry Masking..." << std::endl;

    g_hwid.machine_guid_spoof = GenerateGUID();
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Cryptography", "MachineGuid", g_hwid.machine_guid_spoof);
    std::cout << "[+] Spoofed MachineGuid : " << g_hwid.machine_guid_spoof << std::endl;

    g_hwid.hw_profile_guid_spoof = "{" + GenerateGUID() + "}";
    SetRegistryString(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\IDConfigDB\\Hardware Profiles\\0001", "HwProfileGuid", g_hwid.hw_profile_guid_spoof);
    std::cout << "[+] Spoofed HwProfileGuid: " << g_hwid.hw_profile_guid_spoof << std::endl;

    g_hwid.machine_id_spoof = "{" + GenerateGUID() + "}";
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\SQMClient", "MachineId", g_hwid.machine_id_spoof);
    std::cout << "[+] Spoofed SQM MachineId: " << g_hwid.machine_id_spoof << std::endl;

    g_hwid.computer_hardware_id_spoof = "{" + GenerateGUID() + "}";
    SetRegistryString(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\SystemInformation", "ComputerHardwareId", g_hwid.computer_hardware_id_spoof);
    std::cout << "[+] Spoofed ComputerHardwareId: " << g_hwid.computer_hardware_id_spoof << std::endl;

    g_hwid.product_id_spoof = RandomString(5, true) + "-" + RandomString(5, true) + "-" + RandomString(5, true) + "-" + RandomString(5, true);
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "ProductID", g_hwid.product_id_spoof);
    SetRegistryDWORD(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", "InstallDate", (DWORD)std::time(NULL) - 1000000);
    std::cout << "[+] Spoofed Windows ProductID and InstallDate" << std::endl;

    // MAC Spoofing
    g_hwid.mac_spoof = GenerateSpoofMAC();
    std::string mac_no_colons = g_hwid.mac_spoof;
    mac_no_colons.erase(std::remove(mac_no_colons.begin(), mac_no_colons.end(), ':'), mac_no_colons.end());

    HKEY hNetKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}", 0, KEY_READ, &hNetKey) == ERROR_SUCCESS) {
        char subKeyName[255];
        DWORD subKeyNameSize;
        for (DWORD i = 0; subKeyNameSize = sizeof(subKeyName), RegEnumKeyExA(hNetKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            if (strlen(subKeyName) == 4) { // 0000, 0001, etc.
                std::string subKeyPath = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}\\" + std::string(subKeyName);
                SetRegistryString(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), "NetworkAddress", mac_no_colons);
            }
        }
        RegCloseKey(hNetKey);
    }
    std::cout << "[+] Spoofed MAC: " << g_hwid.mac_spoof << std::endl;

    // BIOS / Board Serials
    g_hwid.bios_serial_spoof = RandomString(12, false);
    SetRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "BaseBoardSerialNumber", g_hwid.bios_serial_spoof);
    SetRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\BIOS", "SystemSerialNumber", g_hwid.bios_serial_spoof);
    std::cout << "[+] Spoofed BIOS/Board Serials in Registry" << std::endl;

    // NVIDIA
    g_hwid.nv_uuid_spoof = "GPU-" + GenerateGUID();
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global", "ClientUUID", g_hwid.nv_uuid_spoof);
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global", "PersistenceIdentifier", g_hwid.nv_uuid_spoof);
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global", "GPU-UUID", g_hwid.nv_uuid_spoof);
    SetRegistryString(HKEY_LOCAL_MACHINE, "SOFTWARE\\NVIDIA Corporation\\Global", "NVIDIA-UUID", g_hwid.nv_uuid_spoof);

    HKEY hVideoKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Video", 0, KEY_READ, &hVideoKey) == ERROR_SUCCESS) {
        char subKeyName[255];
        DWORD subKeyNameSize;
        for (DWORD i = 0; subKeyNameSize = sizeof(subKeyName), RegEnumKeyExA(hVideoKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            std::string subKeyPath = "SYSTEM\\CurrentControlSet\\Control\\Video\\" + std::string(subKeyName);
            // Iterate all subkeys of the GUID (0000, 0001, etc)
            HKEY hGuidKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), 0, KEY_READ, &hGuidKey) == ERROR_SUCCESS) {
                char adapterSubKey[255];
                DWORD adapterSubKeySize;
                for (DWORD j = 0; adapterSubKeySize = sizeof(adapterSubKey), RegEnumKeyExA(hGuidKey, j, adapterSubKey, &adapterSubKeySize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++j) {
                    std::string finalPath = subKeyPath + "\\" + std::string(adapterSubKey);
                    SetRegistryString(HKEY_LOCAL_MACHINE, finalPath.c_str(), "GPU-UUID", g_hwid.nv_uuid_spoof);
                    SetRegistryString(HKEY_LOCAL_MACHINE, finalPath.c_str(), "NVIDIA-UUID", g_hwid.nv_uuid_spoof);
                    SetRegistryString(HKEY_LOCAL_MACHINE, finalPath.c_str(), "GPUDeviceGuid", "{" + g_hwid.nv_uuid_spoof.substr(4) + "}");
                }
                RegCloseKey(hGuidKey);
            }
        }
        RegCloseKey(hVideoKey);
    }

    // Iterate display adapter class keys
    HKEY hClassKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}", 0, KEY_READ, &hClassKey) == ERROR_SUCCESS) {
        char subKeyName[255];
        DWORD subKeyNameSize;
        for (DWORD i = 0; subKeyNameSize = sizeof(subKeyName), RegEnumKeyExA(hClassKey, i, subKeyName, &subKeyNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            if (strlen(subKeyName) == 4) {
                std::string subKeyPath = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e968-e325-11ce-bfc1-08002be10318}\\" + std::string(subKeyName);
                SetRegistryString(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), "GPU-UUID", g_hwid.nv_uuid_spoof);
                SetRegistryString(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), "NVIDIA-UUID", g_hwid.nv_uuid_spoof);
                SetRegistryString(HKEY_LOCAL_MACHINE, subKeyPath.c_str(), "GPUDeviceGuid", "{" + g_hwid.nv_uuid_spoof.substr(4) + "}");
            }
        }
        RegCloseKey(hClassKey);
    }
    std::cout << "[+] Spoofed NVIDIA ClientUUID and persistenceIdentifier" << std::endl;

    // Disk
    g_hwid.disk_id_spoof = RandomString(8, true) + "-" + RandomString(8, true) + "-A";
    g_hwid.disk_serial_spoof = RandomString(20, false);
    SetRegistryString(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\MultifunctionAdapter\\0\\DiskController\\0\\DiskPeripheral\\0", "Identifier", g_hwid.disk_id_spoof);

    // Scsi Serials iteration
    HKEY hScsiRootKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\Scsi", 0, KEY_READ, &hScsiRootKey) == ERROR_SUCCESS) {
        char portName[255];
        DWORD portNameSize;
        for (DWORD i = 0; portNameSize = sizeof(portName), RegEnumKeyExA(hScsiRootKey, i, portName, &portNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) {
            std::string portPath = "HARDWARE\\DEVICEMAP\\Scsi\\" + std::string(portName);
            HKEY hPortKey;
            if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, portPath.c_str(), 0, KEY_READ, &hPortKey) == ERROR_SUCCESS) {
                char busName[255];
                DWORD busNameSize;
                for (DWORD j = 0; busNameSize = sizeof(busName), RegEnumKeyExA(hPortKey, j, busName, &busNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++j) {
                    std::string busPath = portPath + "\\" + std::string(busName);
                    HKEY hBusKey;
                    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, busPath.c_str(), 0, KEY_READ, &hBusKey) == ERROR_SUCCESS) {
                        char targetName[255];
                        DWORD targetNameSize;
                        for (DWORD k = 0; targetNameSize = sizeof(targetName), RegEnumKeyExA(hBusKey, k, targetName, &targetNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++k) {
                             std::string targetPath = busPath + "\\" + std::string(targetName);
                             HKEY hTargetKey;
                             if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, targetPath.c_str(), 0, KEY_READ, &hTargetKey) == ERROR_SUCCESS) {
                                 char lunName[255];
                                 DWORD lunNameSize;
                                 for (DWORD l = 0; lunNameSize = sizeof(lunName), RegEnumKeyExA(hTargetKey, l, lunName, &lunNameSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++l) {
                                     std::string lunPath = targetPath + "\\" + std::string(lunName);
                                     SetRegistryString(HKEY_LOCAL_MACHINE, lunPath.c_str(), "SerialNumber", g_hwid.disk_serial_spoof);
                                 }
                                 RegCloseKey(hTargetKey);
                             }
                        }
                        RegCloseKey(hBusKey);
                    }
                }
                RegCloseKey(hPortKey);
            }
        }
        RegCloseKey(hScsiRootKey);
    }

    std::cout << "[+] Spoofed Disk Identifier and Serial" << std::endl;

    // Persist spoofed IDs to HKCU so we can show them in overlay after restart
    SaveSpoofedIDsToHKCU();

    // Mark as patched
    HKEY hKey;
    if (RegCreateKeyExA(HKEY_CURRENT_USER, "Software\\ApexGuest", 0, NULL, REG_OPTION_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
        DWORD val = 1;
        RegSetValueExA(hKey, "Spoofed", 0, REG_DWORD, (const BYTE*)&val, sizeof(val));
        RegCloseKey(hKey);
    }

    std::cout << "[+] Registry Spoofing completed successfully" << std::endl;
}

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

#define SystemHandleInformation (SYSTEM_INFORMATION_CLASS)16

void DisruptWMI() {
    std::cout << "Attempting to disrupt WMI..." << std::endl;

    DWORD wmi_pid = 0;
    // Try multiple ways to find WMI process
    // Method 1: Registry (Decoupled Server)
    DWORD dwSize = sizeof(wmi_pid);
    RegGetValueA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Wbem\\Transports\\Decoupled\\Server", "ProcessIdentifier", RRF_RT_REG_DWORD, NULL, &wmi_pid, &dwSize);

    // Method 2: Find svchost hosting Winmgmt
    if (wmi_pid == 0) {
        SC_HANDLE hSCM = OpenSCManagerA(NULL, NULL, SC_MANAGER_CONNECT);
        if (hSCM) {
            SC_HANDLE hService = OpenServiceA(hSCM, "Winmgmt", SERVICE_QUERY_STATUS);
            if (hService) {
                SERVICE_STATUS_PROCESS ssp;
                DWORD bytesNeeded;
                if (QueryServiceStatusEx(hService, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(ssp), &bytesNeeded)) {
                    wmi_pid = ssp.dwProcessId;
                }
                CloseServiceHandle(hService);
            }
            CloseServiceHandle(hSCM);
        }
    }

    if (wmi_pid == 0) {
        std::cout << "[-] Failed to get WMI service PID." << std::endl;
        return;
    }
    std::cout << "[+] WMI service PID: " << wmi_pid << std::endl;

    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        LUID luid;
        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luid)) {
            TOKEN_PRIVILEGES tp;
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Luid = luid;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
        }
        CloseHandle(hToken);
    }

    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    auto NtQuerySystemInformationPtr = (NTSTATUS(NTAPI*)(SYSTEM_INFORMATION_CLASS, PVOID, ULONG, PULONG))GetProcAddress(ntdll, "NtQuerySystemInformation");

    ULONG bufferSize = 0x10000;
    PSYSTEM_HANDLE_INFORMATION handleInfo = (PSYSTEM_HANDLE_INFORMATION)malloc(bufferSize);
    while (NtQuerySystemInformationPtr(SystemHandleInformation, handleInfo, bufferSize, NULL) == 0xC0000004) {
        bufferSize *= 2;
        handleInfo = (PSYSTEM_HANDLE_INFORMATION)realloc(handleInfo, bufferSize);
    }

    HANDLE hProcess = OpenProcess(PROCESS_DUP_HANDLE, FALSE, wmi_pid);
    if (!hProcess) {
        free(handleInfo);
        return;
    }

    PSECURITY_DESCRIPTOR pSD = NULL;
    ULONG sdSize = 0;
    // D:(D;;GA;;;WD) -> Deny Everyone All Access
    if (ConvertStringSecurityDescriptorToSecurityDescriptorA("D:(D;;GA;;;WD)", SDDL_REVISION_1, &pSD, &sdSize)) {
        for (ULONG i = 0; i < handleInfo->NumberOfHandles; i++) {
            if (handleInfo->Handles[i].UniqueProcessId == wmi_pid) {
                HANDLE hDup = NULL;
                if (DuplicateHandle(hProcess, (HANDLE)handleInfo->Handles[i].HandleValue, GetCurrentProcess(), &hDup, 0, FALSE, DUPLICATE_SAME_ACCESS)) {
                    SetKernelObjectSecurity(hDup, DACL_SECURITY_INFORMATION, pSD);
                    CloseHandle(hDup);
                }
            }
        }
        LocalFree(pSD);
    }

    CloseHandle(hProcess);
    free(handleInfo);
    std::cout << "[+] WMI disruption applied. WMIC queries should now fail." << std::endl;
}
