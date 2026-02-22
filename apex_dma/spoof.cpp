#include "spoof.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>
#include <chrono>
#include <string>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <cctype>

extern Memory apex_mem;

uint8_t g_real_mac[6] = { 0 };
HardwareIdentifiers g_spoofed_infos;

std::string GenerateRandomUUID(bool uppercase) {
    static const char hex_chars_upper[] = "0123456789ABCDEF";
    static const char hex_chars_lower[] = "0123456789abcdef";
    const char* hex_chars = uppercase ? hex_chars_upper : hex_chars_lower;

    std::string uuid = "";
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

uint8_t HexToByte(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return 0;
}

void UUIDToBytes(const char* uuid, uint8_t* bytes, bool little_endian) {
    if (!uuid || !bytes) return;
    uint8_t temp[16];
    int temp_idx = 0;
    for (int i = 0; i < 36 && temp_idx < 16; ++i) {
        if (uuid[i] == '-') continue;
        temp[temp_idx++] = (HexToByte(uuid[i]) << 4) | HexToByte(uuid[i+1]);
        i += 1;
    }

    if (little_endian) {
        bytes[0] = temp[3]; bytes[1] = temp[2]; bytes[2] = temp[1]; bytes[3] = temp[0];
        bytes[4] = temp[5]; bytes[5] = temp[4];
        bytes[6] = temp[7]; bytes[7] = temp[6];
        for (int i = 8; i < 16; ++i) bytes[i] = temp[i];
    } else {
        for (int i = 0; i < 16; ++i) bytes[i] = temp[i];
    }
}

void ToUnicode(const char* src, uint8_t* dst, int len) {
    if (!src || !dst) return;
    for (int i = 0; i < len; ++i) {
        dst[i * 2] = src[i];
        dst[i * 2 + 1] = 0;
    }
}

void SaveHardwareInfos(const std::string& filename, const HardwareIdentifiers& infos) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "MAC: " << infos.mac << "\n";
        file << "GPU: " << infos.gpu_uuid << "\n";
        file << "MachineGuid: " << infos.machine_guid << "\n";
        file << "HwProfileGuid: " << infos.hw_profile_guid << "\n";
        file << "DiskSerial: " << infos.disk_serial << "\n";
        file << "SMBIOSSerial: " << infos.smbios_serial << "\n";
        file.close();
    }
}

bool LoadHardwareInfos(const std::string& filename, HardwareIdentifiers& infos) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string line;
    infos.mac = "";
    infos.gpu_uuid = "";
    infos.machine_guid = "";
    infos.hw_profile_guid = "";
    while (std::getline(file, line)) {
        if (line.find("MAC: ") == 0 && line.size() > 5) infos.mac = line.substr(5);
        else if (line.find("GPU: ") == 0 && line.size() > 5) infos.gpu_uuid = line.substr(5);
        else if (line.find("MachineGuid: ") == 0 && line.size() > 13) infos.machine_guid = line.substr(13);
        else if (line.find("HwProfileGuid: ") == 0 && line.size() > 15) infos.hw_profile_guid = line.substr(15);
        else if (line.find("DiskSerial: ") == 0 && line.size() > 12) infos.disk_serial = line.substr(12);
        else if (line.find("SMBIOSSerial: ") == 0 && line.size() > 14) infos.smbios_serial = line.substr(14);
    }
    return !infos.mac.empty();
}

std::string GenerateRandomGUID() {
    return "{" + GenerateRandomUUID(false) + "}";
}

void MacToBytes(const std::string& mac_str, uint8_t* bytes) {
    if (mac_str.empty() || !bytes) return;
    int values[6];
    if (6 == sscanf(mac_str.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5])) {
        for (int i = 0; i < 6; ++i) bytes[i] = (uint8_t)values[i];
    }
}

std::string MacToString(const uint8_t* bytes) {
    if (!bytes) return "";
    char buf[20];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
    return std::string(buf);
}

void SpoofHardware() {
    printf("------------------------------------------\n");
    printf("[SPOOFER] Starting HWID Spoofing System...\n");

    std::string real_mac_str = MacToString(g_real_mac);
    printf("[SPOOFER] Real MAC from memory: %s\n", real_mac_str.c_str());

    HardwareIdentifiers real_infos;
    bool has_real_file = LoadHardwareInfos("Real-infos.txt", real_infos);
    if (has_real_file) printf("[SPOOFER] Loaded REAL IDs from file: MAC=%s, GPU=%s\n", real_infos.mac.c_str(), real_infos.gpu_uuid.c_str());

    bool has_spoofed_file = LoadHardwareInfos("spoofed-infos.txt", g_spoofed_infos);
    if (has_spoofed_file) printf("[SPOOFER] Loaded SPOOFED IDs from file: MAC=%s, GPU=%s\n", g_spoofed_infos.mac.c_str(), g_spoofed_infos.gpu_uuid.c_str());

    printf("[SPOOFER] Retrieving max physical address...\n");
    uint64_t max_addr = apex_mem.GetMaxPhysicalAddress();
    if (max_addr == 0 || max_addr < 0x100000000) {
        printf("[!] Warning: Could not retrieve valid max physical address from Memflow. Using fallback: 0x%lx\n", MAX_PHYADDR);
        max_addr = MAX_PHYADDR;
    } else {
        printf("[+] Max physical address retrieved: 0x%lx\n", max_addr);
    }

    const size_t chunk_size = 1024 * 1024 * 16;
    std::vector<uint8_t> buffer;
    try {
        buffer.resize(chunk_size);
        printf("[SPOOFER] Allocated scan buffer (16MB)\n");
    } catch (const std::exception& e) {
        printf("[!] Error: Failed to allocate scan buffer: %s\n", e.what());
        return;
    }

    bool real_gpu_found = false;
    std::string found_gpu_uuid = "";

    printf("[SPOOFER] Scanning for REAL identifiers in guest memory...\n");

    // Pass 1: Identify REAL GPU UUID if not known
    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size - 2048) {
        size_t to_read = (addr + chunk_size > max_addr) ? (size_t)(max_addr - addr) : chunk_size;
        if (to_read < 128) break;

        if (apex_mem.ReadPhysical(addr, buffer.data(), to_read)) {
            for (size_t i = 0; i < to_read - 128; ++i) {
                if (memcmp(&buffer[i], "GPU-", 4) == 0) {
                    char current_uuid[37];
                    memcpy(current_uuid, &buffer[i + 4], 36);
                    current_uuid[36] = '\0';

                    bool is_upper = false, is_lower = false, match = true;
                    for (int j = 0; j < 36; ++j) {
                        if (j == 8 || j == 13 || j == 18 || j == 23) {
                            if (current_uuid[j] != '-') { match = false; break; }
                        } else {
                            char c = current_uuid[j];
                            if (c >= '0' && c <= '9') continue;
                            if (c >= 'A' && c <= 'F') is_upper = true;
                            else if (c >= 'a' && c <= 'f') is_lower = true;
                            else { match = false; break; }
                        }
                    }

                    if (match && !(is_upper && is_lower)) {
                        std::string uuid_str = "GPU-" + std::string(current_uuid);

                        // Check if this matches our spoofed UUID
                        if (has_spoofed_file && uuid_str == g_spoofed_infos.gpu_uuid) {
                            printf("[!] Detected ALREADY patched GPU UUID: %s\n", uuid_str.c_str());
                            printf("[!] Skipping physical memory scan to avoid re-patching.\n");
                            printf("------------------------------------------\n");
                            return;
                        }

                        if (!real_gpu_found) {
                            found_gpu_uuid = uuid_str;
                            real_gpu_found = true;
                            printf("[+] Found Real GPU UUID: %s\n", found_gpu_uuid.c_str());
                        }
                    }
                }
            }
        }
        if (real_gpu_found) break;
    }

    if (!real_gpu_found) {
        printf("[-] Could not find any NVIDIA GPU UUID in physical memory. It might be already patched or not NVIDIA.\n");
        printf("------------------------------------------\n");
        return;
    }

    if (!has_real_file) {
        real_infos.mac = real_mac_str;
        real_infos.gpu_uuid = found_gpu_uuid;
        // MachineGuid and HwProfileGuid will be filled by guest later or manually if needed
        SaveHardwareInfos("Real-infos.txt", real_infos);
        printf("[+] Saved real identifiers to Real-infos.txt\n");
    }

    if (has_spoofed_file) {
        printf("[SPOOFER] Using stored spoofed identifiers.\n");
    } else {
        g_spoofed_infos.gpu_uuid = "GPU-" + GenerateRandomUUID(true);

        uint8_t rand_mac[6];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (int i = 0; i < 6; ++i) rand_mac[i] = dis(gen);
        rand_mac[0] &= 0xFE; // Unicast
        g_spoofed_infos.mac = MacToString(rand_mac);

        g_spoofed_infos.machine_guid = GenerateRandomUUID(false); // Registry usually doesn't have {}
        g_spoofed_infos.hw_profile_guid = GenerateRandomGUID();
        g_spoofed_infos.disk_serial = GenerateRandomUUID(true).substr(0, 20); // 20 chars
        g_spoofed_infos.smbios_serial = GenerateRandomUUID(true).substr(0, 12);

        SaveHardwareInfos("spoofed-infos.txt", g_spoofed_infos);
        printf("[+] Generated new spoofed identifiers and saved to spoofed-infos.txt\n");
    }

    // Use guest-reported IDs if physical scan didn't find them but guest did
    if (real_infos.disk_serial.empty()) {
        // We might want to wait for guest or use a default if we are doing auto-spoof
    }

    printf("\n[SPOOFER] IDENTIFIERS SUMMARY:\n");
    printf("    REAL MAC:   %s\n", real_mac_str.c_str());
    printf("    SPOOF MAC: %s\n", g_spoofed_infos.mac.c_str());
    printf("    REAL GPU:   %s\n", found_gpu_uuid.c_str());
    printf("    SPOOF GPU: %s\n", g_spoofed_infos.gpu_uuid.c_str());
    printf("    SPOOF MGUID: %s\n", g_spoofed_infos.machine_guid.c_str());
    printf("    SPOOF HWID:  %s\n", g_spoofed_infos.hw_profile_guid.c_str());
    printf("    SPOOF DISK: %s\n", g_spoofed_infos.disk_serial.c_str());
    printf("    SPOOF SMB:  %s\n\n", g_spoofed_infos.smbios_serial.c_str());

    if (found_gpu_uuid.size() < 40 || g_spoofed_infos.gpu_uuid.size() < 40) {
        printf("[-] Error: GPU UUID strings are too short. Aborting replacement pass.\n");
        return;
    }

    std::string real_uuid_no_prefix = found_gpu_uuid.substr(4);
    std::string new_uuid_no_prefix = g_spoofed_infos.gpu_uuid.substr(4);

    std::string real_uuid_str_u = real_uuid_no_prefix;
    std::transform(real_uuid_str_u.begin(), real_uuid_str_u.end(), real_uuid_str_u.begin(), [](unsigned char c){ return std::toupper(c); });
    std::string real_uuid_str_l = real_uuid_str_u;
    std::transform(real_uuid_str_l.begin(), real_uuid_str_l.end(), real_uuid_str_l.begin(), [](unsigned char c){ return std::tolower(c); });

    std::string new_uuid_str_u = new_uuid_no_prefix;
    std::transform(new_uuid_str_u.begin(), new_uuid_str_u.end(), new_uuid_str_u.begin(), [](unsigned char c){ return std::toupper(c); });
    std::string new_uuid_str_l = new_uuid_str_u;
    std::transform(new_uuid_str_l.begin(), new_uuid_str_l.end(), new_uuid_str_l.begin(), [](unsigned char c){ return std::tolower(c); });

    uint8_t real_gpu_bytes_be[16], real_gpu_bytes_le[16];
    UUIDToBytes(real_uuid_str_u.c_str(), real_gpu_bytes_be, false);
    UUIDToBytes(real_uuid_str_u.c_str(), real_gpu_bytes_le, true);

    uint8_t new_gpu_bytes_be[16], new_gpu_bytes_le[16];
    UUIDToBytes(new_uuid_str_u.c_str(), new_gpu_bytes_be, false);
    UUIDToBytes(new_uuid_str_u.c_str(), new_gpu_bytes_le, true);

    uint8_t real_mac_bytes[6] = {0}, new_mac_bytes[6] = {0};
    MacToBytes(real_mac_str, real_mac_bytes);
    MacToBytes(g_spoofed_infos.mac, new_mac_bytes);

    int spoof_count = 0;
    int mac_count = 0;
    int disk_count = 0;

    printf("[SPOOFER] Starting physical memory replacement pass...\n");
    auto start_time = std::chrono::high_resolution_clock::now();

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size - 2048) {
        size_t to_read = (addr + chunk_size > max_addr) ? (size_t)(max_addr - addr) : chunk_size;
        if (to_read < 128) break;

        if (apex_mem.ReadPhysical(addr, buffer.data(), to_read)) {
            for (size_t i = 0; i < to_read - 128; ++i) {
                // GPU String replacement
                if (memcmp(&buffer[i], real_uuid_str_u.c_str(), 36) == 0) {
                    apex_mem.WritePhysical(addr + i, new_uuid_str_u.c_str(), 36);
                    spoof_count++;
                } else if (memcmp(&buffer[i], real_uuid_str_l.c_str(), 36) == 0) {
                    apex_mem.WritePhysical(addr + i, new_uuid_str_l.c_str(), 36);
                    spoof_count++;
                }
                // GPU Bytes replacement
                else if (memcmp(&buffer[i], real_gpu_bytes_be, 16) == 0) {
                    apex_mem.WritePhysical(addr + i, new_gpu_bytes_be, 16);
                    spoof_count++;
                } else if (memcmp(&buffer[i], real_gpu_bytes_le, 16) == 0) {
                    apex_mem.WritePhysical(addr + i, new_gpu_bytes_le, 16);
                    spoof_count++;
                }
                // MAC Address Spoofing
                if (real_mac_bytes[0] != 0 && memcmp(&buffer[i], real_mac_bytes, 6) == 0) {
                    apex_mem.WritePhysical(addr + i, new_mac_bytes, 6);
                    mac_count++;
                }
                // Disk Serial Spoofing (ASCII)
                if (!real_infos.disk_serial.empty() && memcmp(&buffer[i], real_infos.disk_serial.c_str(), real_infos.disk_serial.size()) == 0) {
                    apex_mem.WritePhysical(addr + i, g_spoofed_infos.disk_serial.c_str(), g_spoofed_infos.disk_serial.size());
                    disk_count++;
                }
            }
        }
    }

    // SMBIOS Patching (Targeted scan in BIOS region)
    printf("[SPOOFER] Scanning for SMBIOS tables in 0xF0000-0xFFFFF...\n");
    uint8_t smbios_buf[0x10000];
    if (apex_mem.ReadPhysical(0xF0000, smbios_buf, 0x10000)) {
        for (int i = 0; i < 0x10000 - 32; i += 16) {
            if (memcmp(&smbios_buf[i], "_SM_", 4) == 0 || memcmp(&smbios_buf[i], "_SM3_", 5) == 0) {
                printf("[+] Found SMBIOS Entry Point at 0x%X\n", 0xF0000 + i);
                // Here we could parse the table, but a simple replacement of the real serial string in the whole BIOS region is often enough
            }
        }

        if (!real_infos.smbios_serial.empty()) {
             for (int i = 0; i < 0x10000 - real_infos.smbios_serial.size(); ++i) {
                 if (memcmp(&smbios_buf[i], real_infos.smbios_serial.c_str(), real_infos.smbios_serial.size()) == 0) {
                     apex_mem.WritePhysical(0xF0000 + i, g_spoofed_infos.smbios_serial.c_str(), g_spoofed_infos.smbios_serial.size());
                     printf("[+] Patched SMBIOS serial occurrence in BIOS region.\n");
                 }
             }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end_time - start_time).count();
    printf("[SPOOFER] Physical replacement pass completed in %.2f seconds.\n", duration);
    printf("[SPOOFER] Replaced %d GPU occurrences, %d MAC occurrences, and %d Disk occurrences.\n", spoof_count, mac_count, disk_count);
    printf("------------------------------------------\n");
}
