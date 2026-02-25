#include "spoof.h"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

// Helper to convert GUID to string
std::string guid_to_string(const uint8_t* guid) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (int i = 0; i < 4; ++i) ss << std::setw(2) << (int)guid[i];
    ss << "-";
    for (int i = 4; i < 6; ++i) ss << std::setw(2) << (int)guid[i];
    ss << "-";
    for (int i = 6; i < 8; ++i) ss << std::setw(2) << (int)guid[i];
    ss << "-";
    for (int i = 8; i < 10; ++i) ss << std::setw(2) << (int)guid[i];
    ss << "-";
    for (int i = 10; i < 16; ++i) ss << std::setw(2) << (int)guid[i];
    return ss.str();
}

bool spoof_gpu_uuid(std::string &real_uuid, std::string &fake_uuid) {
    if (!kernel) return false;

    ModuleInfo module_info;
    if (kernel->module_by_name("nvlddmkm.sys", &module_info) != 0) {
        printf("Failed to find nvlddmkm.sys\n");
        return false;
    }

    printf("nvlddmkm.sys found at %lx, size: %lx\n", module_info.base, module_info.size);

    // Limit scan range to first 32MB for performance and reliability
    size_t scan_size = std::min((size_t)module_info.size, (size_t)0x2000000);
    std::vector<uint8_t> module_data(scan_size);

    printf("Reading %zu bytes from nvlddmkm.sys in 1MB chunks...\n", scan_size);
    size_t chunk_size = 1024 * 1024;
    for (size_t offset = 0; offset < scan_size; offset += chunk_size) {
        size_t current_chunk = std::min(chunk_size, scan_size - offset);
        if (kernel->read_raw_into(module_info.base + offset, CSliceMut<uint8_t>((char*)module_data.data() + offset, current_chunk)) != 0) {
            printf("Warning: Failed to read chunk at offset %zx\n", offset);
            // We keep going, maybe the pattern is elsewhere
        }
    }

    // Try both patterns
    const char* pattern1 = "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9"; // Thread 1
    const char* pattern2 = "E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15"; // Thread 2

    size_t offset = findPattern(module_data.data(), module_data.size(), pattern1);
    bool using_p1 = true;
    if (offset == -1) {
        offset = findPattern(module_data.data(), module_data.size(), pattern2);
        using_p1 = false;
    }

    if (offset == -1) {
        printf("Failed to find any GPU manager pattern in nvlddmkm.sys\n");
        return false;
    }

    printf("Pattern found at offset %zx using pattern %d\n", offset, using_p1 ? 1 : 2);

    uint64_t g_system = 0;
    if (using_p1) {
        uint32_t rip_offset = *(uint32_t*)&module_data[offset + 3];
        uint64_t g_system_ptr = module_info.base + offset + 7 + rip_offset;
        kernel->read_raw_into(g_system_ptr, CSliceMut<uint8_t>((char*)&g_system, 8));
    } else {
        // Pattern 2 logic (GpuMgrGetGpuFromId)
        // This usually requires following calls and reading offsets.
        // For simplicity, we try to use Pattern 1's g_system if found.
        // If not, we might need a more complex implementation.
        printf("Pattern 2 logic not fully implemented, please use Pattern 1 or provide offsets.\n");
        return false;
    }

    if (!g_system) {
        printf("Failed to find g_system\n");
        return false;
    }
    printf("g_system found at %lx\n", g_system);

    uint64_t gpu_sys = 0;
    kernel->read_raw_into(g_system + 0x1C0, CSliceMut<uint8_t>((char*)&gpu_sys, 8));
    if (!gpu_sys) return false;

    uint32_t gpu_mask = 0;
    kernel->read_raw_into(gpu_sys + 0x754, CSliceMut<uint8_t>((char*)&gpu_mask, 4));
    printf("GPU Mask: %x\n", gpu_mask);

    bool success = false;
    for (int i = 0; i < 32; i++) {
        if (gpu_mask & (1U << i)) {
            uint64_t gpu_device_entry = gpu_sys + 0x3C8D0 + (i * 0x10);
            uint64_t gpu_object = 0;
            kernel->read_raw_into(gpu_device_entry, CSliceMut<uint8_t>((char*)&gpu_object, 8));

            if (gpu_object) {
                uint8_t is_valid = 0;
                kernel->read_raw_into(gpu_object + 0x848, CSliceMut<uint8_t>((char*)&is_valid, 1));
                if (!is_valid) {
                    printf("GPU %d object found but not initialized\n", i);
                    continue;
                }

                uint8_t current_uuid[16];
                kernel->read_raw_into(gpu_object + 0x849, CSliceMut<uint8_t>((char*)current_uuid, 16));
                real_uuid = guid_to_string(current_uuid);

                // Generate fake UUID
                uint8_t new_uuid[16];
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 255);
                for (int j = 0; j < 16; j++) new_uuid[j] = dis(gen);

                fake_uuid = guid_to_string(new_uuid);

                kernel->write_raw(gpu_object + 0x849, CSliceRef<uint8_t>((char*)new_uuid, 16));
                printf("GPU %d spoofed: %s -> %s\n", i, real_uuid.c_str(), fake_uuid.c_str());
                success = true;
            }
        }
    }

    return success;
}

bool physical_spoof(const std::string& target_uuid, std::string& fake_uuid) {
    if (!conn) return false;

    if (target_uuid.empty() || target_uuid == "Unknown") {
        printf("Invalid target UUID for physical spoof\n");
        return false;
    }

    printf("Starting physical memory spoofing for UUID: %s\n", target_uuid.c_str());

    // Generate fake UUID string
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);
    const char* hex_chars = "0123456789abcdef";

    std::string new_uuid = target_uuid;
    for (size_t i = 0; i < new_uuid.length(); ++i) {
        if (isxdigit(new_uuid[i])) {
            new_uuid[i] = hex_chars[dis(gen)];
        }
    }
    fake_uuid = new_uuid;
    printf("Generated fake UUID: %s\n", fake_uuid.c_str());

    // Prepare binary forms
    auto string_to_binary = [](const std::string& s) {
        std::vector<uint8_t> bin;
        std::string clean = "";
        for (char c : s) if (isxdigit(c)) clean += c;
        for (size_t i = 0; i < clean.length(); i += 2) {
            bin.push_back((uint8_t)std::stoul(clean.substr(i, 2), nullptr, 16));
        }
        return bin;
    };

    std::vector<uint8_t> target_bin = string_to_binary(target_uuid);
    std::vector<uint8_t> fake_bin = string_to_binary(fake_uuid);

    if (target_bin.size() != 16 || fake_bin.size() != 16) {
        printf("Failed to parse UUID binary form\n");
        return false;
    }

    auto phys_view = conn->phys_view();
    PhysicalMemoryMetadata meta = conn->metadata();
    uint64_t max_addr = meta.max_address;

    printf("Physical memory range: 0 - %lx\n", max_addr);

    size_t found_count = 0;
    size_t chunk_size = 2 * 1024 * 1024; // 2MB chunks
    std::vector<uint8_t> buffer(chunk_size + 256); // Extra space for overlap

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
        if (addr % (1024ULL * 1024 * 1024) == 0) {
            printf("Scanning physical memory... %lu GB / %lu GB\r", addr / (1024*1024*1024), max_addr / (1024*1024*1024));
            fflush(stdout);
        }

        if (phys_view.read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), chunk_size)) != 0) {
            continue;
        }

        // Search for ASCII string
        for (size_t i = 0; i < chunk_size - target_uuid.length(); ++i) {
            if (memcmp(buffer.data() + i, target_uuid.c_str(), target_uuid.length()) == 0) {
                printf("\nFound ASCII UUID at physical address %lx\n", addr + i);
                phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_uuid.c_str(), fake_uuid.length()));
                found_count++;
            }
        }

        // Search for binary GUID
        for (size_t i = 0; i < chunk_size - 16; ++i) {
            if (memcmp(buffer.data() + i, target_bin.data(), 16) == 0) {
                printf("\nFound Binary UUID at physical address %lx\n", addr + i);
                phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_bin.data(), 16));
                found_count++;
            }
        }
    }

    printf("\nPhysical spoofing completed. Found and patched %zu occurrences.\n", found_count);
    return found_count > 0;
}
