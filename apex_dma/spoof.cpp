#include "spoof.h"
#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cstring>

extern Memory apex_mem;

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

void UUIDToBytes(const char* uuid, uint8_t* bytes) {
    int byte_idx = 0;
    for (int i = 0; i < 36 && byte_idx < 16; ++i) {
        if (uuid[i] == '-') continue;
        bytes[byte_idx++] = (HexToByte(uuid[i]) << 4) | HexToByte(uuid[i+1]);
        i += 1;
    }
}

void SpoofHardware() {
    printf("------------------------------------------\n");
    printf("Starting HWID Spoofing...\n");

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

    uint8_t new_gpu_bytes[16];
    UUIDToBytes(new_gpu_uuid_upper.c_str(), new_gpu_bytes);

    bool gpu_found = false;
    char real_uuid_str[37] = {0};
    char real_uuid_no_dashes[33] = {0};
    uint8_t real_gpu_bytes[16] = {0};
    int spoof_count = 0;

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
                            UUIDToBytes(real_uuid_str, real_gpu_bytes);

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

                // 2. NVIDIA GPU UUID Spoofing (Raw Bytes)
                if (gpu_found && memcmp(&buffer[i], real_gpu_bytes, 16) == 0) {
                    // Check if this wasn't part of a string we already replaced
                    // (heuristic: check if bytes before were "GPU-")
                    if (i >= 4 && memcmp(&buffer[i-4], "GPU-", 4) == 0) {
                        // Already handled by string replacement above (most likely)
                    } else {
                        if (apex_mem.WritePhysical(addr + i, new_gpu_bytes, 16)) {
                            spoof_count++;
                        }
                    }
                }

                // 3. NVIDIA GPU UUID Spoofing (No dashes string)
                if (gpu_found && memcmp(&buffer[i], real_uuid_no_dashes, 32) == 0) {
                     if (apex_mem.WritePhysical(addr + i, new_gpu_uuid_no_dashes.c_str(), 32)) {
                        spoof_count++;
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
    printf("------------------------------------------\n");
}
