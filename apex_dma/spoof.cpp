#include "spoof.h"
#include <iostream>
#include <random>
#include <algorithm>
#include <cstring>
#include <ctime>
#include <cctype>

std::string Spoof::realUUID = "";
std::string Spoof::spoofedUUID = "";
bool Spoof::spoofed = false;
std::mutex Spoof::dataMutex;

void Spoof::InternalScan(MemoryViewBase<CBox<void>, CArc<void>>& phys_view, uint64_t max_addr) {
    if (phys_view.vtbl == nullptr) {
        printf("[-] Spoof: phys_view.vtbl is null\n");
        return;
    }

    const size_t chunkSize = 0x100000; // 1MB
    std::vector<uint8_t> buffer(chunkSize + 100);

    printf("[+] Scanning physical memory (Max: 0x%lx)...\n", max_addr);

    uint64_t last_log = 0;
    for (uint64_t addr = 0; addr < max_addr; addr += chunkSize) {
        if (addr >= last_log + 0x10000000) { // Every 256MB
            printf("[+] Scanning... %.2f GB / %.2f GB\n", (double)addr / (1024.0*1024.0*1024.0), (double)max_addr / (1024.0*1024.0*1024.0));
            last_log = addr;
        }

        size_t toRead = (addr + chunkSize > max_addr) ? (size_t)(max_addr - addr) : chunkSize;
        if (toRead < 40) break;

        if (phys_view.read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), (uintptr_t)toRead)) != 0)
            continue;

        for (size_t i = 0; i <= toRead - 40; ++i) {
            // Match GPU- or gpu-
            if ((buffer[i] == 'G' || buffer[i] == 'g') &&
                (buffer[i+1] == 'P' || buffer[i+1] == 'p') &&
                (buffer[i+2] == 'U' || buffer[i+2] == 'u') &&
                buffer[i+3] == '-') {

                if (matchesUUIDPattern((char*)&buffer[i] + 4)) {
                    std::string foundUUID((char*)&buffer[i], 40);
                    std::string replacement;

                    {
                        std::lock_guard<std::mutex> lock(dataMutex);
                        if (realUUID.empty()) {
                            realUUID = foundUUID;
                            spoofedUUID = generateRandomUUID(realUUID);
                            printf("[+] Found original GPU UUID: %s\n", realUUID.c_str());
                            printf("[+] Generated spoofed UUID: %s\n", spoofedUUID.c_str());
                        }

                        if (foundUUID == realUUID) {
                            replacement = spoofedUUID;
                        } else {
                            replacement = generateRandomUUID(foundUUID);
                        }
                    }

                    if (phys_view.write_raw(addr + i, CSliceRef<uint8_t>(replacement.c_str(), (uintptr_t)40)) == 0) {
                        std::lock_guard<std::mutex> lock(dataMutex);
                        spoofed = true;
                    }
                }
            }
        }
    }
}

bool Spoof::ScanAndSpoof() {
    // Only return if we already found something AND finished scanning?
    // Actually, if we are in the process of scanning, we should let it finish.
    // The detached thread only runs once anyway because of the guard in apex_dma.cpp.

    if (kernel && kernel.get()->vtbl_clone) {
        printf("[+] Spoof: Cloning kernel for physical memory access\n");
        auto kernel_clone = kernel.get()->clone();
        auto phys_view = kernel_clone.phys_view();
        auto metadata = kernel_clone.physicalmemory_metadata();

        uint64_t max_addr = (uint64_t)metadata.max_address;
        if (max_addr == 0 || max_addr > MAX_PHYADDR) max_addr = MAX_PHYADDR;

        InternalScan(phys_view, max_addr);
        return true;
    } else if (conn && conn.get()->vtbl_clone) {
        printf("[+] Spoof: Cloning conn for physical memory access\n");
        auto conn_clone = conn.get()->clone();
        auto phys_view = conn_clone.phys_view();
        auto metadata = conn_clone.metadata();

        uint64_t max_addr = (uint64_t)metadata.max_address;
        if (max_addr == 0 || max_addr > MAX_PHYADDR) max_addr = MAX_PHYADDR;

        InternalScan(phys_view, max_addr);
        return true;
    }

    printf("[-] Spoof: No cloning support or instance available\n");
    return false;
}

std::string Spoof::getRealUUID() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return realUUID;
}

std::string Spoof::getSpoofedUUID() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return spoofedUUID;
}

bool Spoof::isSpoofed() {
    std::lock_guard<std::mutex> lock(dataMutex);
    return spoofed;
}

bool Spoof::matchesUUIDPattern(const char* str) {
    if (str[8] != '-' || str[13] != '-' || str[18] != '-' || str[23] != '-')
        return false;

    for (int i = 0; i < 36; ++i) {
        if (i == 8 || i == 13 || i == 18 || i == 23) continue;
        if (!isxdigit((unsigned char)str[i])) return false;
    }
    return true;
}

std::string Spoof::generateRandomUUID(const std::string& original) {
    std::string result = original.substr(0, 4); // Keep "GPU-" or "gpu-"

    bool uppercase = false;
    int upperCount = 0;
    int lowerCount = 0;
    for (int i = 4; i < (int)original.size(); ++i) {
        if (isupper((unsigned char)original[i])) upperCount++;
        else if (islower((unsigned char)original[i])) lowerCount++;
    }
    if (upperCount > lowerCount) uppercase = true;

    char part[40];

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
