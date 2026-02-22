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

void SpoofHardware() {
    printf("------------------------------------------\n");
    printf("Starting HWID Spoofing...\n");
    printf("Scanning guest physical memory (up to 64GB)...\n");

    const size_t chunk_size = 1024 * 1024 * 4; // 4MB chunks
    std::vector<uint8_t> buffer(chunk_size);

    std::string new_gpu_uuid_upper = GenerateRandomUUID(true);
    std::string new_gpu_uuid_lower = new_gpu_uuid_upper;
    std::transform(new_gpu_uuid_lower.begin(), new_gpu_uuid_lower.end(), new_gpu_uuid_lower.begin(), ::tolower);

    bool gpu_found = false;
    int spoof_count = 0;

    // Scan physical memory
    for (uint64_t addr = 0; addr < MAX_PHYADDR; addr += chunk_size - 1024) {
        if (apex_mem.ReadPhysical(addr, buffer.data(), chunk_size)) {
            for (size_t i = 0; i < chunk_size - 64; ++i) {
                // 1. NVIDIA GPU UUID Spoofing
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
                        }

                        const char* replacement = is_lower ? new_gpu_uuid_lower.c_str() : new_gpu_uuid_upper.c_str();
                        if (apex_mem.WritePhysical(addr + i + 4, replacement, 36)) {
                            spoof_count++;
                        }
                    }
                }
            }
        }

        if (addr % (1024LL * 1024 * 1024 * 4) == 0 && addr > 0) {
            printf("Progress: %lu GB scanned...\n", addr / (1024 * 1024 * 1024));
        }
    }

    if (gpu_found) {
        printf("\n[+] HWID Spoofing completed. Replaced %d occurrences of GPU UUID.\n", spoof_count);
    } else {
        printf("\n[-] NVIDIA GPU UUID not found in physical memory.\n");
    }
    printf("------------------------------------------\n");
}
