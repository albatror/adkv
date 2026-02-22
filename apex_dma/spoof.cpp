#include "spoof.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>

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
    int byte_idx = 0;
    uint8_t temp[16];
    int temp_idx = 0;
    for (int i = 0; i < 36 && temp_idx < 16; ++i) {
        if (uuid[i] == '-') continue;
        temp[temp_idx++] = (HexToByte(uuid[i]) << 4) | HexToByte(uuid[i+1]);
        i += 1;
    }

    if (little_endian) {
        // Data1 (4 bytes) LE
        bytes[0] = temp[3]; bytes[1] = temp[2]; bytes[2] = temp[1]; bytes[3] = temp[0];
        // Data2 (2 bytes) LE
        bytes[4] = temp[5]; bytes[5] = temp[4];
        // Data3 (2 bytes) LE
        bytes[6] = temp[7]; bytes[7] = temp[6];
        // Data4 (8 bytes) BE (standard)
        for (int i = 8; i < 16; ++i) bytes[i] = temp[i];
    } else {
        for (int i = 0; i < 16; ++i) bytes[i] = temp[i];
    }
}

void SpoofHardware() {
    printf("------------------------------------------\n");
    printf("Starting HWID Spoofing...\n");

    if (g_real_mac[0] != 0 || g_real_mac[1] != 0) {
        printf("[+] Real MAC from client: %02X:%02X:%02X:%02X:%02X:%02X\n",
            g_real_mac[0], g_real_mac[1], g_real_mac[2], g_real_mac[3], g_real_mac[4], g_real_mac[5]);
    }

    uint64_t max_addr = apex_mem.GetMaxPhysicalAddress();
    if (max_addr == 0) max_addr = MAX_PHYADDR;

    printf("Scanning guest physical memory (up to %.2f GB)...\n", (double)max_addr / (1024.0 * 1024.0 * 1024.0));

    const size_t chunk_size = 1024 * 1024 * 8; // 8MB chunks
    std::vector<uint8_t> buffer(chunk_size);

    std::string new_gpu_uuid_upper = GenerateRandomUUID(true);
    std::string new_gpu_uuid_lower = new_gpu_uuid_upper;
    std::transform(new_gpu_uuid_lower.begin(), new_gpu_uuid_lower.end(), new_gpu_uuid_lower.begin(), ::tolower);

    std::string new_gpu_uuid_no_dashes = new_gpu_uuid_upper;
    new_gpu_uuid_no_dashes.erase(std::remove(new_gpu_uuid_no_dashes.begin(), new_gpu_uuid_no_dashes.end(), '-'), new_gpu_uuid_no_dashes.end());

    uint8_t new_gpu_bytes_be[16];
    UUIDToBytes(new_gpu_uuid_upper.c_str(), new_gpu_bytes_be, false);
    uint8_t new_gpu_bytes_le[16];
    UUIDToBytes(new_gpu_uuid_upper.c_str(), new_gpu_bytes_le, true);

    bool gpu_found = false;
    char real_uuid_str[37] = {0};
    char real_uuid_no_dashes[33] = {0};
    uint8_t real_gpu_bytes_be[16] = {0};
    uint8_t real_gpu_bytes_le[16] = {0};

    uint8_t new_mac[6];
    std::random_device rd_mac;
    std::mt19937 gen_mac(rd_mac());
    std::uniform_int_distribution<> dis_mac(0, 255);
    for (int i = 0; i < 6; ++i) new_mac[i] = dis_mac(gen_mac);
    new_mac[0] &= 0xFE; // Unicast

    int spoof_count = 0;
    int mac_count = 0;

    // Scan physical memory
    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size - 1024) {
        if (apex_mem.ReadPhysical(addr, buffer.data(), chunk_size)) {
            for (size_t i = 0; i < chunk_size - 64; ++i) {
                // 1. NVIDIA GPU UUID Spoofing (String format with "GPU-")
                if (memcmp(&buffer[i], "GPU-", 4) == 0) {
                    char current_uuid[37];
                    memcpy(current_uuid, &buffer[i + 4], 36);
                    current_uuid[36] = '\0';

                    bool is_upper = false;
                    bool is_lower = false;
                    bool match = true;
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
                        if (!gpu_found) {
                            printf("\n[+] Found Real NVIDIA GPU UUID: %s\n", current_uuid);
                            printf("[+] Generated Spoofed UUID:   %s\n\n", new_gpu_uuid_upper.c_str());
                            gpu_found = true;
                            strncpy(real_uuid_str, current_uuid, 36);
                            UUIDToBytes(real_uuid_str, real_gpu_bytes_be, false);
                            UUIDToBytes(real_uuid_str, real_gpu_bytes_le, true);

                            // Prepare no-dashes version
                            int k = 0;
                            for(int j=0; j<36; j++) if(real_uuid_str[j] != '-') real_uuid_no_dashes[k++] = real_uuid_str[j];
                        }

                        const char* replacement = is_lower ? new_gpu_uuid_lower.c_str() : new_gpu_uuid_upper.c_str();
                        if (apex_mem.WritePhysical(addr + i + 4, replacement, 36)) {
                            spoof_count++;
                        }
                    }
                }

                // 2. NVIDIA GPU UUID Spoofing (Raw Bytes BE and LE)
                if (gpu_found) {
                    if (memcmp(&buffer[i], real_gpu_bytes_be, 16) == 0) {
                         if (i >= 4 && memcmp(&buffer[i-4], "GPU-", 4) == 0) {
                            // Handled
                        } else {
                            if (apex_mem.WritePhysical(addr + i, new_gpu_bytes_be, 16)) spoof_count++;
                        }
                    } else if (memcmp(&buffer[i], real_gpu_bytes_le, 16) == 0) {
                        if (apex_mem.WritePhysical(addr + i, new_gpu_bytes_le, 16)) spoof_count++;
                    }
                }

                // 3. NVIDIA GPU UUID Spoofing (No dashes string)
                if (gpu_found && memcmp(&buffer[i], real_uuid_no_dashes, 32) == 0) {
                     if (apex_mem.WritePhysical(addr + i, new_gpu_uuid_no_dashes.c_str(), 32)) {
                        spoof_count++;
                    }
                }

                // 4. MAC Address Spoofing
                if (g_real_mac[0] != 0 && memcmp(&buffer[i], g_real_mac, 6) == 0) {
                    if (apex_mem.WritePhysical(addr + i, new_mac, 6)) {
                        mac_count++;
                    }
                }
            }
        }

        if (addr % (1024LL * 1024 * 1024 * 4) == 0 && addr > 0) {
            printf("Progress: %lu GB scanned...\n", addr / (1024 * 1024 * 1024));
        }
    }

    if (gpu_found) {
        printf("\n[+] HWID Spoofing completed. Replaced %d occurrences of GPU UUID (Strings and Raw Bytes).\n", spoof_count);
    } else {
        printf("\n[-] NVIDIA GPU UUID not found in physical memory.\n");
    }

    if (mac_count > 0) {
        printf("[+] Replaced %d occurrences of MAC address.\n", mac_count);
    }
    printf("------------------------------------------\n");
}
