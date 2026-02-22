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

extern Memory apex_mem;

uint8_t g_real_mac[6] = { 0 };

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
    for (int i = 0; i < len; ++i) {
        dst[i * 2] = src[i];
        dst[i * 2 + 1] = 0;
    }
}

void SaveHardwareInfos(const std::string& filename, const std::string& mac, const std::string& gpu_uuid) {
    std::ofstream file(filename);
    if (file.is_open()) {
        file << "MAC: " << mac << "\n";
        file << "GPU: " << gpu_uuid << "\n";
        file.close();
    }
}

bool LoadHardwareInfos(const std::string& filename, std::string& mac, std::string& gpu_uuid) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("MAC: ") == 0) mac = line.substr(5);
        else if (line.find("GPU: ") == 0) gpu_uuid = line.substr(5);
    }
    return !mac.empty() && !gpu_uuid.empty();
}

void MacToBytes(const std::string& mac_str, uint8_t* bytes) {
    int values[6];
    if (6 == sscanf(mac_str.c_str(), "%x:%x:%x:%x:%x:%x", &values[0], &values[1], &values[2], &values[3], &values[4], &values[5])) {
        for (int i = 0; i < 6; ++i) bytes[i] = (uint8_t)values[i];
    }
}

std::string MacToString(const uint8_t* bytes) {
    char buf[20];
    snprintf(buf, sizeof(buf), "%02X:%02X:%02X:%02X:%02X:%02X", bytes[0], bytes[1], bytes[2], bytes[3], bytes[4], bytes[5]);
    return std::string(buf);
}

void SpoofHardware() {
    printf("------------------------------------------\n");
    printf("Starting HWID Spoofing System...\n");

    std::string real_mac_str = "";
    if (g_real_mac[0] != 0 || g_real_mac[1] != 0) {
        real_mac_str = MacToString(g_real_mac);
    }

    std::string stored_real_mac, stored_real_gpu;
    bool has_real_file = LoadHardwareInfos("Real-infos.txt", stored_real_mac, stored_real_gpu);

    std::string stored_spoofed_mac, stored_spoofed_gpu;
    bool has_spoofed_file = LoadHardwareInfos("spoofed-infos.txt", stored_spoofed_mac, stored_spoofed_gpu);

    // If we have real MAC from client and no real file, save it
    if (!real_mac_str.empty() && !has_real_file) {
        // We still need the real GPU UUID to save the full real file.
        // We'll proceed with scanning.
    }

    uint64_t max_addr = apex_mem.GetMaxPhysicalAddress();
    if (max_addr == 0 || max_addr < 0x100000000) max_addr = MAX_PHYADDR;

    printf("Scanning guest physical memory...\n");

    const size_t chunk_size = 1024 * 1024 * 16;
    std::vector<uint8_t> buffer(chunk_size);

    bool real_gpu_found = false;
    std::string found_gpu_uuid = "";

    // First pass or combined: identify REAL GPU UUID if not known
    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size - 2048) {
        size_t to_read = (addr + chunk_size > max_addr) ? (size_t)(max_addr - addr) : chunk_size;
        if (to_read < 128) break;

        if (apex_mem.ReadPhysical(addr, buffer.data(), to_read)) {
            for (size_t i = 0; i < to_read - 128; ++i) {
                // Heuristic for GPU UUID
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
                        if (has_spoofed_file && uuid_str == stored_spoofed_gpu) {
                            printf("[!] Detected ALREADY SPOOFED GPU UUID: %s\n", uuid_str.c_str());
                            printf("[!] Skipping physical memory spoofing as it's already done.\n");
                            printf("------------------------------------------\n");
                            return;
                        }

                        if (!real_gpu_found) {
                            found_gpu_uuid = uuid_str;
                            real_gpu_found = true;
                            printf("[+] Found Real NVIDIA GPU UUID: %s\n", found_gpu_uuid.c_str());
                        }
                    }
                }
            }
        }
        if (real_gpu_found) break;
    }

    if (!real_gpu_found) {
        printf("[-] Could not find any NVIDIA GPU UUID in physical memory.\n");
        printf("------------------------------------------\n");
        return;
    }

    // Save Real Infos if not present
    if (!has_real_file) {
        SaveHardwareInfos("Real-infos.txt", real_mac_str, found_gpu_uuid);
        printf("[+] Saved real identifiers to Real-infos.txt\n");
    }

    // Generate or use stored Spoofed Infos
    std::string new_gpu_uuid_upper;
    std::string new_mac_str;

    if (has_spoofed_file) {
        new_gpu_uuid_upper = stored_spoofed_gpu;
        new_mac_str = stored_spoofed_mac;
        printf("[+] Using previously generated spoofed identifiers from spoofed-infos.txt\n");
    } else {
        new_gpu_uuid_upper = "GPU-" + GenerateRandomUUID(true);

        uint8_t rand_mac[6];
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);
        for (int i = 0; i < 6; ++i) rand_mac[i] = dis(gen);
        rand_mac[0] &= 0xFE; // Unicast
        new_mac_str = MacToString(rand_mac);

        SaveHardwareInfos("spoofed-infos.txt", new_mac_str, new_gpu_uuid_upper);
        printf("[+] Generated and saved new spoofed identifiers to spoofed-infos.txt\n");
    }

    printf("\n[SPOOFER] REAL MAC:   %s\n", real_mac_str.c_str());
    printf("[SPOOFER] SPOOF MAC: %s\n", new_mac_str.c_str());
    printf("[SPOOFER] REAL GPU:   %s\n", found_gpu_uuid.c_str());
    printf("[SPOOFER] SPOOF GPU: %s\n\n", new_gpu_uuid_upper.c_str());

    // Prepare bytes for replacement
    std::string real_uuid_no_prefix = found_gpu_uuid.substr(4);
    std::string new_uuid_no_prefix = new_gpu_uuid_upper.substr(4);

    std::string real_uuid_str_u = real_uuid_no_prefix;
    std::transform(real_uuid_str_u.begin(), real_uuid_str_u.end(), real_uuid_str_u.begin(), ::toupper);
    std::string real_uuid_str_l = real_uuid_str_u;
    std::transform(real_uuid_str_l.begin(), real_uuid_str_l.end(), real_uuid_str_l.begin(), ::tolower);

    std::string new_uuid_str_u = new_uuid_no_prefix;
    std::transform(new_uuid_str_u.begin(), new_uuid_str_u.end(), new_uuid_str_u.begin(), ::toupper);
    std::string new_uuid_str_l = new_uuid_str_u;
    std::transform(new_uuid_str_l.begin(), new_uuid_str_l.end(), new_uuid_str_l.begin(), ::tolower);

    uint8_t real_gpu_bytes_be[16], real_gpu_bytes_le[16];
    UUIDToBytes(real_uuid_str_u.c_str(), real_gpu_bytes_be, false);
    UUIDToBytes(real_uuid_str_u.c_str(), real_gpu_bytes_le, true);

    uint8_t new_gpu_bytes_be[16], new_gpu_bytes_le[16];
    UUIDToBytes(new_uuid_str_u.c_str(), new_gpu_bytes_be, false);
    UUIDToBytes(new_uuid_str_u.c_str(), new_gpu_bytes_le, true);

    uint8_t real_mac_bytes[6], new_mac_bytes[6];
    MacToBytes(real_mac_str, real_mac_bytes);
    MacToBytes(new_mac_str, new_mac_bytes);

    int spoof_count = 0;
    int mac_count = 0;

    auto start_time = std::chrono::high_resolution_clock::now();

    // Actual replacement pass
    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size - 2048) {
        size_t to_read = (addr + chunk_size > max_addr) ? (size_t)(max_addr - addr) : chunk_size;
        if (to_read < 128) break;

        if (apex_mem.ReadPhysical(addr, buffer.data(), to_read)) {
            bool chunk_modified = false;
            for (size_t i = 0; i < to_read - 128; ++i) {
                // GPU String replacement
                if (memcmp(&buffer[i], real_uuid_str_u.c_str(), 36) == 0) {
                    memcpy(&buffer[i], new_uuid_str_u.c_str(), 36);
                    apex_mem.WritePhysical(addr + i, new_uuid_str_u.c_str(), 36);
                    spoof_count++;
                } else if (memcmp(&buffer[i], real_uuid_str_l.c_str(), 36) == 0) {
                    memcpy(&buffer[i], new_uuid_str_l.c_str(), 36);
                    apex_mem.WritePhysical(addr + i, new_uuid_str_l.c_str(), 36);
                    spoof_count++;
                }
                // GPU Bytes replacement
                else if (memcmp(&buffer[i], real_gpu_bytes_be, 16) == 0) {
                    memcpy(&buffer[i], new_gpu_bytes_be, 16);
                    apex_mem.WritePhysical(addr + i, new_gpu_bytes_be, 16);
                    spoof_count++;
                } else if (memcmp(&buffer[i], real_gpu_bytes_le, 16) == 0) {
                    memcpy(&buffer[i], new_gpu_bytes_le, 16);
                    apex_mem.WritePhysical(addr + i, new_gpu_bytes_le, 16);
                    spoof_count++;
                }
                // MAC Address Spoofing
                if (real_mac_bytes[0] != 0 && memcmp(&buffer[i], real_mac_bytes, 6) == 0) {
                    memcpy(&buffer[i], new_mac_bytes, 6);
                    apex_mem.WritePhysical(addr + i, new_mac_bytes, 6);
                    mac_count++;
                }
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double duration = std::chrono::duration<double>(end_time - start_time).count();
    printf("[+] Spoofing completed in %.2f seconds.\n", duration);
    printf("[+] Replaced %d GPU occurrences and %d MAC occurrences.\n", spoof_count, mac_count);
    printf("------------------------------------------\n");
}
