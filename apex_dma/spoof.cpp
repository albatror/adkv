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
#include <thread>
#include <iomanip>
#include <algorithm>
#include <cstring>
#include <cctype>

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
void uuid_to_binary(const std::string& uuid_str, uint8_t* out_bin, bool windows_le) {
    std::string hex = uuid_str;
    hex.erase(std::remove(hex.begin(), hex.end(), '-'), hex.end());
    if (hex.length() != 32) return;

    for (int i = 0; i < 16; ++i) {
        try {
            out_bin[i] = (uint8_t)std::stoul(hex.substr(i * 2, 2), nullptr, 16);
        } catch (...) {
            out_bin[i] = 0;
        }
    }

    if (windows_le) {
        // Windows GUID format: Data1 (4 LE), Data2 (2 LE), Data3 (2 LE), Data4 (8 BE)
        std::swap(out_bin[0], out_bin[3]);
        std::swap(out_bin[1], out_bin[2]);
        std::swap(out_bin[4], out_bin[5]);
        std::swap(out_bin[6], out_bin[7]);
    }
}

std::string to_upper(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
    return s;
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

void spoof_gpu_uuid(std::string real_uuid_str, std::string& fake_uuid_str_out) {
    printf("--- GPU UUID Spoofer ---\n");
    if (!kernel) {
        printf("Error: Kernel not initialized\n");
        return;
    }

    printf("Hardware Info:\n");
    system("lspci -nn | grep -i nvidia");

    if (real_uuid_str.empty()) {
        printf("Error: Real UUID is empty\n");
        return;
    }

    // Normalize input
    real_uuid_str = to_lower(real_uuid_str);

    fake_uuid_str_out = generate_random_uuid();
    std::string fake_uuid_str = to_lower(fake_uuid_str_out);

    printf("Real UUID: %s\n", real_uuid_str.c_str());
    printf("Fake UUID: %s\n", fake_uuid_str.c_str());

    // Prepare ASCII patterns
    std::vector<std::pair<std::string, std::string>> ascii_patterns;

    // Pattern 1: GPU-UUID
    ascii_patterns.push_back({"GPU-" + real_uuid_str, "GPU-" + fake_uuid_str});
    ascii_patterns.push_back({"GPU-" + to_upper(real_uuid_str), "GPU-" + to_upper(fake_uuid_str)});

    // Pattern 2: UUID only
    ascii_patterns.push_back({real_uuid_str, fake_uuid_str});
    ascii_patterns.push_back({to_upper(real_uuid_str), to_upper(fake_uuid_str)});

    // Pattern 3: UUID without dashes
    std::string real_no_dashes = real_uuid_str;
    real_no_dashes.erase(std::remove(real_no_dashes.begin(), real_no_dashes.end(), '-'), real_no_dashes.end());
    std::string fake_no_dashes = fake_uuid_str;
    fake_no_dashes.erase(std::remove(fake_no_dashes.begin(), fake_no_dashes.end(), '-'), fake_no_dashes.end());

    ascii_patterns.push_back({real_no_dashes, fake_no_dashes});
    ascii_patterns.push_back({to_upper(real_no_dashes), to_upper(fake_no_dashes)});

    // Prepare Binary patterns
    struct BinPattern {
        uint8_t real[16];
        uint8_t fake[16];
    };
    std::vector<BinPattern> bin_patterns;

    // Raw bytes (BE)
    BinPattern raw;
    uuid_to_binary(real_uuid_str, raw.real, false);
    uuid_to_binary(fake_uuid_str, raw.fake, false);
    bin_patterns.push_back(raw);

    // Windows GUID (LE)
    BinPattern win_le;
    uuid_to_binary(real_uuid_str, win_le.real, true);
    uuid_to_binary(fake_uuid_str, win_le.fake, true);
    bin_patterns.push_back(win_le);

    printf("Starting physical memory scan (64GB)...\n");

    const uint64_t chunk_size = 1024 * 1024; // 1MB
    const uint64_t page_size = 4096;
    std::vector<uint8_t> buffer(chunk_size);
    uint64_t total_patched = 0;

    auto scan_buffer = [&](uint8_t* data, size_t size, uint64_t current_addr) {
        bool modified = false;
        // ASCII search
        for (const auto& p : ascii_patterns) {
            for (size_t i = 0; i <= size - p.first.length(); ++i) {
                if (memcmp(data + i, p.first.c_str(), p.first.length()) == 0) {
                    memcpy(data + i, p.second.c_str(), p.second.length());
                    modified = true;
                    total_patched++;
                    printf("[+] Patched ASCII pattern at %lx\n", current_addr + i);
                }
            }
        }
        // Binary search
        for (const auto& p : bin_patterns) {
            for (size_t i = 0; i <= size - 16; ++i) {
                if (memcmp(data + i, p.real, 16) == 0) {
                    memcpy(data + i, p.fake, 16);
                    modified = true;
                    total_patched++;
                    printf("[+] Patched Binary pattern at %lx\n", current_addr + i);
                }
            }
        }
        return modified;
    };

    for (uint64_t addr = 0; addr < MAX_PHYADDR; addr += chunk_size) {
        if (addr % (1024ULL * 1024 * 1024) == 0) {
            printf("Scanning... %llu GB / %llu GB\n", (unsigned long long)(addr / (1024ULL * 1024 * 1024)), (unsigned long long)(MAX_PHYADDR / (1024ULL * 1024 * 1024)));
        }

        if (kernel->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), chunk_size)) == 0) {
            if (scan_buffer(buffer.data(), chunk_size, addr)) {
                kernel->phys_view().write_raw(addr, CSliceRef<uint8_t>((char*)buffer.data(), chunk_size));
            }
        } else {
            // Chunk read failed, try page by page
            for (uint64_t page_addr = addr; page_addr < addr + chunk_size; page_addr += page_size) {
                std::vector<uint8_t> page_buffer(page_size);
                if (kernel->phys_view().read_raw_into(page_addr, CSliceMut<uint8_t>((char*)page_buffer.data(), page_size)) == 0) {
                    if (scan_buffer(page_buffer.data(), page_size, page_addr)) {
                        kernel->phys_view().write_raw(page_addr, CSliceRef<uint8_t>((char*)page_buffer.data(), page_size));
                    }
                }
            }
        }
    }

    printf("Scan complete. Total occurrences patched: %llu\n", (unsigned long long)total_patched);
    printf("Wait 30 seconds for user to start the game...\n");
    std::this_thread::sleep_for(std::chrono::seconds(30));
    printf("------------------------\n");
}
