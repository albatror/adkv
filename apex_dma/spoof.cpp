#include "spoof.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cstring>
#include <ctime>

std::string Spoof::realUUID = "";
std::string Spoof::spoofedUUID = "";
bool Spoof::spoofed = false;

bool Spoof::ScanAndSpoof() {
    if (spoofed) return true;
    if (!conn) {
        printf("[-] Spoof: conn is null\n");
        return false;
    }

    if (conn.get()->vtbl_physicalmemory == nullptr) {
        printf("[-] Spoof: vtbl_physicalmemory is null\n");
        return false;
    }

    auto phys_view = conn.get()->phys_view();
    auto metadata = conn.get()->metadata();
    uint64_t max_addr = (uint64_t)metadata.max_address;
    if (max_addr == 0) max_addr = MAX_PHYADDR;

    const size_t chunkSize = 0x100000; // 1MB
    std::vector<uint8_t> buffer(chunkSize + 100);

    printf("[+] Scanning physical memory for GPU UUID (Max: 0x%lx)...\n", max_addr);

    for (uint64_t addr = 0; addr < max_addr; addr += chunkSize) {
        size_t toRead = (addr + chunkSize > max_addr) ? (max_addr - addr) : chunkSize;
        if (toRead < 40) break;

        if (phys_view.read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), (uintptr_t)toRead)) != 0)
            continue;

        for (size_t i = 0; i < toRead - 40; ++i) {
            // Match GPU- or gpu-
            if ((buffer[i] == 'G' || buffer[i] == 'g') &&
                (buffer[i+1] == 'P' || buffer[i+1] == 'p') &&
                (buffer[i+2] == 'U' || buffer[i+2] == 'u') &&
                buffer[i+3] == '-') {

                if (matchesUUIDPattern((char*)&buffer[i] + 4)) {
                    std::string foundUUID((char*)&buffer[i], 40);
                    std::string replacement;

                    if (realUUID.empty()) {
                        realUUID = foundUUID;
                        spoofedUUID = generateRandomUUID(realUUID);
                        printf("[+] Found original GPU UUID: %s\n", realUUID.c_str());
                        printf("[+] Generated spoofed UUID: %s\n", spoofedUUID.c_str());
                        replacement = spoofedUUID;
                    } else if (foundUUID == realUUID) {
                        replacement = spoofedUUID;
                    } else {
                        replacement = generateRandomUUID(foundUUID);
                    }

                    if (phys_view.write_raw(addr + i, CSliceRef<uint8_t>(replacement.c_str(), (uintptr_t)40)) == 0) {
                        spoofed = true;
                    }
                }
            }
        }
    }

    if (spoofed) {
        printf("[+] GPU UUID spoofing applied successfully.\n");
        return true;
    } else {
        printf("[-] Failed to find or spoof GPU UUID.\n");
        return false;
    }
}

std::string Spoof::getRealUUID() {
    return realUUID;
}

std::string Spoof::getSpoofedUUID() {
    return spoofedUUID;
}

bool Spoof::isSpoofed() {
    return spoofed;
}

bool Spoof::matchesUUIDPattern(const char* str) {
    // Expected pattern after GPU-: XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX (36 chars)
    // Indices relative to str:
    // 8, 13, 18, 23 should be '-'
    if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-')
        return false;

    for (int i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        if (!isxdigit((unsigned char)str[i])) return false;
    }
    return true;
}

std::string Spoof::generateRandomUUID(const std::string& original) {
    // original: GPU-XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
    std::string result = original.substr(0, 4); // Keep "GPU-" or "gpu-"

    bool uppercase = false;
    // Check if original UUID part is primarily uppercase
    int upperCount = 0;
    int lowerCount = 0;
    for (int i = 4; i < 40; ++i) {
        if (isupper((unsigned char)original[i])) upperCount++;
        else if (islower((unsigned char)original[i])) lowerCount++;
    }
    if (upperCount > lowerCount) uppercase = true;

    char part[40];
    memset(part, 0, 40);

    randomHex(part, 8, uppercase);
    result += part;
    result += "-";

    randomHex(part, 4, uppercase);
    result += part;
    result += "-";

    randomHex(part, 4, uppercase);
    result += part;
    result += "-";

    randomHex(part, 4, uppercase);
    result += part;
    result += "-";

    randomHex(part, 12, uppercase);
    result += part;

    return result;
}

void Spoof::randomHex(char* s, int len, bool uppercase) {
    static const char hexUpper[] = "0123456789ABCDEF";
    static const char hexLower[] = "0123456789abcdef";
    const char* hex = uppercase ? hexUpper : hexLower;

    static std::random_device rd;
    static std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    for (int i = 0; i < len; ++i) {
        s[i] = hex[dis(gen)];
    }
    s[len] = 0;
}
