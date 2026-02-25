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

    // Try patterns to find g_system
    const char* p_g_system1 = "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9"; // Thread 1
    const char* p_g_system2 = "48 8B 05 ? ? ? ? 48 8B 1D ? ? ? ? 48 85 C0 0F 84"; // Fallback
    const char* p_gpumgr_call = "E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15"; // Thread 2

    uint64_t g_system = 0;
    size_t offset = findPattern(module_data.data(), module_data.size(), p_g_system1);
    if (offset != -1) {
        printf("g_system found via pattern 1 at offset %zx\n", offset);
        uint32_t rip_offset = *(uint32_t*)&module_data[offset + 3];
        uint64_t g_system_ptr = module_info.base + offset + 7 + rip_offset;
        kernel->read_raw_into(g_system_ptr, CSliceMut<uint8_t>((char*)&g_system, 8));
    } else {
        offset = findPattern(module_data.data(), module_data.size(), p_g_system2);
        if (offset != -1) {
            printf("g_system found via fallback pattern at offset %zx\n", offset);
            uint32_t rip_offset = *(uint32_t*)&module_data[offset + 3];
            uint64_t g_system_ptr = module_info.base + offset + 7 + rip_offset;
            kernel->read_raw_into(g_system_ptr, CSliceMut<uint8_t>((char*)&g_system, 8));
        }
    }

    uint32_t uuid_valid_offset = 0x848; // Default

    // Try to find uuid_valid_offset via Pattern 2
    size_t gpumgr_off = findPattern(module_data.data(), module_data.size(), p_gpumgr_call);
    if (gpumgr_off != -1) {
        printf("Pattern 2 (GpuMgrGetGpuFromId call) found at offset %zx\n", gpumgr_off);
        int32_t rel_offset = *(int32_t*)&module_data[gpumgr_off + 1];
        uint64_t GpuMgrGetGpuFromId_addr = module_info.base + gpumgr_off + 5 + rel_offset;

        std::vector<uint8_t> func_data(512);
        if (kernel->read_raw_into(GpuMgrGetGpuFromId_addr, CSliceMut<uint8_t>((char*)func_data.data(), 512)) == 0) {
            for (size_t i = 0; i < 512 - 7; i++) {
                // Search for lea rX, [rcx + offset] or lea rX, [rax + offset]
                // Specifically looking for 0x818D4C which is 4C 8D 81
                if (func_data[i] == 0x4C && func_data[i+1] == 0x8D && func_data[i+2] == 0x81) {
                    uuid_valid_offset = *(uint32_t*)&func_data[i+3] - 1;
                    printf("Found uuid_valid_offset via Pattern 2: %x\n", uuid_valid_offset);
                    break;
                }
            }
        }
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
                kernel->read_raw_into(gpu_object + uuid_valid_offset, CSliceMut<uint8_t>((char*)&is_valid, 1));
                if (!is_valid) {
                    printf("GPU %d object found but not initialized (offset: %x)\n", i, uuid_valid_offset);
                    continue;
                }

                uint8_t current_uuid[16];
                kernel->read_raw_into(gpu_object + uuid_valid_offset + 1, CSliceMut<uint8_t>((char*)current_uuid, 16));
                real_uuid = guid_to_string(current_uuid);

                // Generate fake UUID
                uint8_t new_uuid[16];
                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 255);
                for (int j = 0; j < 16; j++) new_uuid[j] = dis(gen);

                fake_uuid = guid_to_string(new_uuid);

                kernel->write_raw(gpu_object + uuid_valid_offset + 1, CSliceRef<uint8_t>((char*)new_uuid, 16));
                printf("GPU %d spoofed via driver: %s -> %s\n", i, real_uuid.c_str(), fake_uuid.c_str());
                success = true;
            }
        }
    }

    return success;
}

bool physical_spoof(const std::string& target_uuid, std::string& fake_uuid) {
    if (!kernel) return false;

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
        if (isxdigit((unsigned char)new_uuid[i])) {
            new_uuid[i] = hex_chars[dis(gen)];
        }
    }
    fake_uuid = new_uuid;
    printf("Generated fake UUID: %s\n", fake_uuid.c_str());

    // Prepare binary forms
    auto string_to_binary = [](const std::string& s) {
        std::vector<uint8_t> bin;
        std::string clean = "";
        for (char c : s) if (isxdigit((unsigned char)c)) clean += c;
        for (size_t i = 0; i + 1 < clean.length(); i += 2) {
            bin.push_back((uint8_t)std::stoul(clean.substr(i, 2), nullptr, 16));
        }
        return bin;
    };

    auto to_guid_bin = [](std::vector<uint8_t> bin) {
        if (bin.size() != 16) return bin;
        // GUID: Data1 (4 LE), Data2 (2 LE), Data3 (2 LE), Data4 (8 BE)
        // UUID string: 83904901-65ad-9709-a59d-7a664aa707c1
        // bin has them in straight order: [83 90 49 01] [65 ad] [97 09] [a5 9d 7a 66 4a a7 07 c1]
        std::swap(bin[0], bin[3]); std::swap(bin[1], bin[2]); // Data1
        std::swap(bin[4], bin[5]); // Data2
        std::swap(bin[6], bin[7]); // Data3
        return bin;
    };

    std::vector<uint8_t> target_bin = string_to_binary(target_uuid);
    std::vector<uint8_t> fake_bin = string_to_binary(fake_uuid);

    if (target_bin.size() != 16 || fake_bin.size() != 16) {
        printf("Failed to parse UUID binary form\n");
        return false;
    }

    std::vector<uint8_t> target_guid = to_guid_bin(target_bin);
    std::vector<uint8_t> fake_guid = to_guid_bin(fake_bin);

    auto phys_view = kernel->phys_view();
    PhysicalMemoryMetadata meta = kernel->physicalmemory_metadata();
    uint64_t max_addr = meta.max_address;

    printf("Physical memory range: 0 - %lx\n", max_addr);

    size_t found_count = 0;
    size_t chunk_size = 2 * 1024 * 1024; // 2MB chunks
    size_t overlap = 256;
    std::vector<uint8_t> buffer(chunk_size + overlap);

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
        if (addr % (1024ULL * 1024 * 1024) == 0) {
            printf("Scanning physical memory... %lu GB / %lu GB\r", addr / (1024*1024*1024), max_addr / (1024*1024*1024));
            fflush(stdout);
        }

        size_t to_read = std::min((uint64_t)buffer.size(), max_addr - addr);
        if (phys_view.read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), to_read)) != 0) {
            continue;
        }

        // Search for ASCII string
        for (size_t i = 0; i <= to_read - target_uuid.length() && i < to_read; ++i) {
            if (memcmp(buffer.data() + i, target_uuid.c_str(), target_uuid.length()) == 0) {
                printf("\nFound ASCII UUID at physical address %lx\n", addr + i);
                phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_uuid.c_str(), fake_uuid.length()));
                found_count++;
            }
        }

        // Search for binary GUID (Straight)
        for (size_t i = 0; i <= to_read - 16 && i < to_read; ++i) {
            if (memcmp(buffer.data() + i, target_bin.data(), 16) == 0) {
                printf("\nFound Binary UUID (Straight) at physical address %lx\n", addr + i);
                phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_bin.data(), 16));
                found_count++;
            }
        }

        // Search for binary GUID (LE)
        for (size_t i = 0; i <= to_read - 16 && i < to_read; ++i) {
            if (memcmp(buffer.data() + i, target_guid.data(), 16) == 0) {
                printf("\nFound Binary UUID (LE) at physical address %lx\n", addr + i);
                phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_guid.data(), 16));
                found_count++;
            }
        }
    }

    printf("\nPhysical spoofing completed. Found and patched %zu occurrences.\n", found_count);
    return found_count > 0;
}
