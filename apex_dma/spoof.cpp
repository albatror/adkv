#include "spoof.h"
#include "memory.h"
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <cstring>

// Function to generate a random UUID
std::string generate_random_uuid() {
    const char* chars = "0123456789abcdef";
    std::string uuid = "";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    for (int i = 0; i < 32; ++i) {
        if (i == 8 || i == 12 || i == 16 || i == 20) {
            uuid += "-";
        }
        uuid += chars[dis(gen)];
    }
    return uuid;
}

// Helper to convert UUID string to binary
void uuid_to_binary(const std::string& uuid_str, uint8_t* out_bin) {
    std::string hex = uuid_str;
    hex.erase(std::remove(hex.begin(), hex.end(), '-'), hex.end());
    if (hex.length() != 32) return;

    for (int i = 0; i < 16; ++i) {
        out_bin[i] = (uint8_t)std::stoul(hex.substr(i * 2, 2), nullptr, 16);
    }

    // Windows GUID format: Data1 (4 LE), Data2 (2 LE), Data3 (2 LE), Data4 (8 BE)
    // Swap for Little-Endian
    std::swap(out_bin[0], out_bin[3]);
    std::swap(out_bin[1], out_bin[2]);
    std::swap(out_bin[4], out_bin[5]);
    std::swap(out_bin[6], out_bin[7]);
}

void spoof_gpu_uuid() {
    printf("--- GPU UUID Spoofer ---\n");
    printf("Hardware Info:\n");
    system("lspci -nn | grep -i nvidia");

    std::ifstream infile("real_gpu.txt");
    std::string real_uuid_str;
    if (!(infile >> real_uuid_str)) {
        printf("Error: Could not read real_gpu.txt\n");
        printf("Please create real_gpu.txt with your real GPU UUID (e.g., 63853174-886d-311b-71a2-52033c467615)\n");
        printf("You can find it by running 'nvidia-smi -L' in the guest VM.\n");
        return;
    }
    infile.close();

    std::string fake_uuid_str = generate_random_uuid();
    printf("Real UUID: %s\n", real_uuid_str.c_str());
    printf("Fake UUID: %s\n", fake_uuid_str.c_str());

    // Prepare patterns
    std::string real_ascii = "GPU-" + real_uuid_str;
    std::string fake_ascii = "GPU-" + fake_uuid_str;

    uint8_t real_bin[16];
    uint8_t fake_bin[16];
    uuid_to_binary(real_uuid_str, real_bin);
    uuid_to_binary(fake_uuid_str, fake_bin);

    printf("Starting physical memory scan...\n");

    const uint64_t chunk_size = 2 * 1024 * 1024; // 2MB
    std::vector<uint8_t> buffer(chunk_size);
    uint64_t total_patched = 0;

    for (uint64_t addr = 0; addr < MAX_PHYADDR; addr += chunk_size) {
        if (addr % (1024ULL * 1024 * 1024) == 0) {
            printf("Scanning... %llu GB / %llu GB\n", addr / (1024ULL * 1024 * 1024), MAX_PHYADDR / (1024ULL * 1024 * 1024));
        }

        if (kernel->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), chunk_size)) != 0) {
            continue;
        }

        bool modified = false;

        // Search for ASCII
        for (size_t i = 0; i <= chunk_size - real_ascii.length(); ++i) {
            if (memcmp(buffer.data() + i, real_ascii.c_str(), real_ascii.length()) == 0) {
                memcpy(buffer.data() + i, fake_ascii.c_str(), fake_ascii.length());
                modified = true;
                total_patched++;
            }
        }

        // Search for Binary
        for (size_t i = 0; i <= chunk_size - 16; ++i) {
            if (memcmp(buffer.data() + i, real_bin, 16) == 0) {
                memcpy(buffer.data() + i, fake_bin, 16);
                modified = true;
                total_patched++;
            }
        }

        if (modified) {
            kernel->phys_view().write_raw(addr, CSliceRef<uint8_t>((char*)buffer.data(), chunk_size));
        }
    }

    printf("Scan complete. Total occurrences patched: %lu\n", total_patched);
    printf("------------------------\n");
}
