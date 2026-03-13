#include "spoof.h"
#include <iostream>
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>
#include <set>

// Offsets from dma-spoofer
namespace offsets {
    constexpr uint64_t DRIVER_CONTEXT_GLOBAL = 0x132DF30;
    constexpr uint64_t DEVICE_MGR_BASE = 0x1F0;
    constexpr uint64_t DEV_MGR_DEVICE_COUNT = 0x3F440;
    constexpr uint64_t DEVICE_CTX_GPU_MGR = 0x1F70;
    constexpr uint64_t GPU_MGR_GPU_COUNT = 0x1A20;
    constexpr uint64_t GPU_MGR_GPU_ARRAY = 0x19E0;
    constexpr size_t GPU_UUID_SIZE = 16;
    constexpr size_t GPU_UUID_SEARCH_SIZE = 0x2000;
}


struct GpuDevice {
    uint32_t index;
    uint64_t address;
};

// Helper functions for GPU discovery
bool is_valid_kernel_ptr(uint64_t ptr) {
    return ptr != 0 && ptr > 0xFFFF000000000000;
}

uint64_t find_list_head(uint64_t driver_base, uint64_t driver_size) {
    printf("[*] Scanning for ListHead global variable...\n");
    const uint8_t pattern[] = {0x48, 0x8B, 0x15};
    size_t scan_size = 0x800000;
    if (scan_size > driver_size) scan_size = driver_size;

    std::vector<uint8_t> data(scan_size);
    if (kernel->read_raw_into(driver_base, CSliceMut<uint8_t>((char*)data.data(), scan_size)) != 0) {
        return 0;
    }

    for (size_t i = 0; i < data.size() - 7; i++) {
        if (data[i] == pattern[0] && data[i+1] == pattern[1] && data[i+2] == pattern[2]) {
            int32_t rip_offset = *(int32_t*)&data[i + 3];
            uint64_t rip = driver_base + i + 7;
            uint64_t target_addr = (uint64_t)((int64_t)rip + (int64_t)rip_offset);

            if (target_addr < driver_base || target_addr >= driver_base + driver_size) continue;

            uint64_t flink = 0, blink = 0;
            if (kernel->read_raw_into(target_addr, CSliceMut<uint8_t>((char*)&flink, 8)) == 0 &&
                kernel->read_raw_into(target_addr + 8, CSliceMut<uint8_t>((char*)&blink, 8)) == 0) {
                if (is_valid_kernel_ptr(flink) && is_valid_kernel_ptr(blink)) {
                    if (flink != target_addr && blink != target_addr) {
                        printf("[+] Found potential ListHead @ 0x%lx\n", target_addr);
                        return target_addr;
                    }
                }
            }
        }
    }
    return 0;
}

std::vector<GpuDevice> enumerate_from_device_list(uint64_t driver_base, uint64_t driver_size) {
    const int64_t DEVICE_OBJECT_OFFSET = -0x4028;
    const uint64_t DEVICE_EXTENSION_OFFSET = 0x40;
    std::vector<GpuDevice> devices;

    uint64_t list_head_addr = find_list_head(driver_base, driver_size);
    if (!list_head_addr) return devices;

    uint64_t flink = 0;
    if (kernel->read_raw_into(list_head_addr, CSliceMut<uint8_t>((char*)&flink, 8)) != 0) return devices;

    uint64_t current = flink;
    std::set<uint64_t> visited;

    while (current != list_head_addr && visited.size() < 32) {
        if (visited.count(current)) break;
        visited.insert(current);

        uint64_t device_obj_addr = (uint64_t)((int64_t)current + DEVICE_OBJECT_OFFSET);
        if (is_valid_kernel_ptr(device_obj_addr)) {
            uint64_t device_extension_ptr = 0;
            if (kernel->read_raw_into(device_obj_addr + DEVICE_EXTENSION_OFFSET, CSliceMut<uint8_t>((char*)&device_extension_ptr, 8)) == 0) {
                if (is_valid_kernel_ptr(device_extension_ptr)) {
                    std::vector<uint8_t> ext_data(0x200);
                    if (kernel->read_raw_into(device_extension_ptr, CSliceMut<uint8_t>((char*)ext_data.data(), 0x200)) == 0) {
                        for (size_t offset = 0; offset < 0x200; offset += 8) {
                            uint64_t ptr = *(uint64_t*)&ext_data[offset];
                            if (!is_valid_kernel_ptr(ptr)) continue;

                            std::vector<uint8_t> test_data(0x100);
                            if (kernel->read_raw_into(ptr, CSliceMut<uint8_t>((char*)test_data.data(), 0x100)) == 0) {
                                int valid_ptr_count = 0;
                                for (size_t j = 0; j < 0x100; j += 8) {
                                    if (is_valid_kernel_ptr(*(uint64_t*)&test_data[j])) valid_ptr_count++;
                                }
                                if (valid_ptr_count >= 10) {
                                    printf("[+] Found GPU object @ 0x%lx via device list\n", ptr);
                                    bool exists = false;
                                    for (const auto& d : devices) if (d.address == ptr) exists = true;
                                    if (!exists) devices.push_back({(uint32_t)devices.size(), ptr});
                                }
                            }
                        }
                    }
                }
            }
        }
        if (kernel->read_raw_into(current, CSliceMut<uint8_t>((char*)&current, 8)) != 0) break;
    }
    return devices;
}

// Legacy enumeration methods
uint64_t get_driver_context(uint64_t driver_base) {
    uint64_t ctx = 0;
    kernel->read_raw_into(driver_base + offsets::DRIVER_CONTEXT_GLOBAL, CSliceMut<uint8_t>((char*)&ctx, 8));
    return ctx;
}

uint64_t get_device_manager(uint64_t driver_base) {
    uint64_t ctx = get_driver_context(driver_base);
    if (!ctx) return 0;
    uint64_t mgr = 0;
    kernel->read_raw_into(ctx + offsets::DEVICE_MGR_BASE, CSliceMut<uint8_t>((char*)&mgr, 8));
    return mgr;
}

uint64_t get_first_device_context(uint64_t driver_base) {
    uint64_t mgr = get_device_manager(driver_base);
    if (!mgr) return 0;
    uint32_t count = 0;
    kernel->read_raw_into(mgr + offsets::DEV_MGR_DEVICE_COUNT, CSliceMut<uint8_t>((char*)&count, 4));
    if (count == 0 || count > 32) return 0;
    uint64_t ptr_offset = 16 * 16164u;
    uint64_t ctx = 0;
    kernel->read_raw_into(mgr + ptr_offset, CSliceMut<uint8_t>((char*)&ctx, 8));
    return is_valid_kernel_ptr(ctx) ? ctx : 0;
}

std::vector<GpuDevice> enumerate_legacy(uint64_t driver_base) {
    std::vector<GpuDevice> devices;
    uint64_t device_ctx = get_first_device_context(driver_base);
    if (!device_ctx) return devices;

    uint64_t gpu_mgr = 0;
    kernel->read_raw_into(device_ctx + offsets::DEVICE_CTX_GPU_MGR, CSliceMut<uint8_t>((char*)&gpu_mgr, 8));
    if (!gpu_mgr) return devices;

    uint32_t gpu_count = 0;
    kernel->read_raw_into(gpu_mgr + offsets::GPU_MGR_GPU_COUNT, CSliceMut<uint8_t>((char*)&gpu_count, 4));

    for (uint32_t i = 0; i < std::min(gpu_count, 32u); i++) {
        uint64_t gpu_obj = 0;
        kernel->read_raw_into(gpu_mgr + offsets::GPU_MGR_GPU_ARRAY + (i * 8), CSliceMut<uint8_t>((char*)&gpu_obj, 8));
        if (is_valid_kernel_ptr(gpu_obj)) {
            devices.push_back({i, gpu_obj});
        }
    }
    return devices;
}


uint64_t find_gpu_manager_array(uint64_t driver_base, uint64_t driver_size) {
    const uint8_t pattern[] = {0x33, 0xC9, 0x4C, 0x8D, 0x05};
    size_t scan_size = 0x800000;
    if (scan_size > driver_size) scan_size = driver_size;

    std::vector<uint8_t> data(scan_size);
    if (kernel->read_raw_into(driver_base, CSliceMut<uint8_t>((char*)data.data(), scan_size)) != 0) return 0;

    for (size_t i = 0; i < data.size() - 16; i++) {
        if (memcmp(&data[i], pattern, sizeof(pattern)) == 0) {
            if (data[i + 9] == 0x0F && data[i + 10] == 0x1F && data[i + 11] == 0x00) {
                if (data[i + 12] == 0x49 && data[i + 13] == 0x8B && data[i + 14] == 0x1C && data[i + 15] == 0xC8) {
                    int32_t rip_offset = *(int32_t*)&data[i + 5];
                    uint64_t rip = driver_base + i + 9;
                    uint64_t array_addr = (uint64_t)((int64_t)rip + (int64_t)rip_offset);
                    printf("[+] Found GPU manager array @ 0x%lx\n", array_addr);
                    return array_addr;
                }
            }
        }
    }
    return 0;
}

std::vector<GpuDevice> enumerate_from_gpu_manager_array(uint64_t driver_base, uint64_t driver_size) {
    std::vector<GpuDevice> devices;
    uint64_t array_addr = find_gpu_manager_array(driver_base, driver_size);
    if (!array_addr) return devices;

    for (int i = 0; i < 32; i++) {
        uint64_t gpu_mgr_ptr = 0;
        if (kernel->read_raw_into(array_addr + (i * 8), CSliceMut<uint8_t>((char*)&gpu_mgr_ptr, 8)) == 0) {
            if (is_valid_kernel_ptr(gpu_mgr_ptr)) {
                std::vector<uint8_t> mgr_data(0x1000);
                if (kernel->read_raw_into(gpu_mgr_ptr, CSliceMut<uint8_t>((char*)mgr_data.data(), 0x1000)) == 0) {
                    for (size_t offset = 0; offset < 0x1000; offset += 8) {
                        uint64_t ptr = *(uint64_t*)&mgr_data[offset];
                        if (is_valid_kernel_ptr(ptr)) {
                            std::vector<uint8_t> test_data(0x100);
                            if (kernel->read_raw_into(ptr, CSliceMut<uint8_t>((char*)test_data.data(), 0x100)) == 0) {
                                int valid_ptr_count = 0;
                                for (size_t j = 0; j < 0x100; j += 8) {
                                    if (is_valid_kernel_ptr(*(uint64_t*)&test_data[j])) valid_ptr_count++;
                                }
                                if (valid_ptr_count >= 8) {
                                    printf("[+] Found GPU object @ 0x%lx via GPU manager array\n", ptr);
                                    bool exists = false;
                                    for (const auto& d : devices) if (d.address == ptr) exists = true;
                                    if (!exists) devices.push_back({(uint32_t)devices.size(), ptr});
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return devices;
}

// Prepare binary forms (ported from physical_spoof helper)
std::vector<uint8_t> string_to_binary(const std::string& s) {
    std::vector<uint8_t> bin;
    std::string clean = "";
    for (char c : s) if (isxdigit((unsigned char)c)) clean += c;
    try {
        for (size_t i = 0; i + 1 < clean.length(); i += 2) {
            bin.push_back((uint8_t)std::stoul(clean.substr(i, 2), nullptr, 16));
        }
    } catch (...) {
        return std::vector<uint8_t>();
    }
    return bin;
}

std::vector<uint8_t> to_guid_bin(std::vector<uint8_t> bin) {
    if (bin.size() != 16) return bin;
    std::swap(bin[0], bin[3]); std::swap(bin[1], bin[2]);
    std::swap(bin[4], bin[5]);
    std::swap(bin[6], bin[7]);
    return bin;
}

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

bool spoof_gpu_uuid(const std::string& target_uuid, std::string &real_uuid, std::string &fake_uuid) {
    if (!kernel) return false;

    std::vector<uint8_t> target_bin = string_to_binary(target_uuid);
    std::vector<uint8_t> target_guid = to_guid_bin(target_bin);

    if (target_bin.size() != 16) {
        printf("[-] Invalid target UUID format for driver spoof\n");
        return false;
    }

    ModuleInfo module_info;
    if (kernel->module_by_name("nvlddmkm.sys", &module_info) != 0) {
        printf("Failed to find nvlddmkm.sys\n");
        return false;
    }

    printf("nvlddmkm.sys found at %lx, size: %lx\n", module_info.base, module_info.size);

    std::vector<GpuDevice> devices = enumerate_from_device_list(module_info.base, module_info.size);
    if (devices.empty()) {
        printf("[*] Falling back to GPU manager array enumeration...\n");
        devices = enumerate_from_gpu_manager_array(module_info.base, module_info.size);
    }
    if (devices.empty()) {
        printf("[*] Falling back to legacy enumeration method...\n");
        devices = enumerate_legacy(module_info.base);
    }

    if (devices.empty()) {
        printf("[-] Failed to find any GPU devices via driver structures\n");
        return false;
    }

    bool any_success = false;
    for (const auto& dev : devices) {
        printf("[*] Scanning GPU object at 0x%lx for UUID matching %s...\n", dev.address, target_uuid.c_str());

        std::vector<uint8_t> data(offsets::GPU_UUID_SEARCH_SIZE);
        if (kernel->read_raw_into(dev.address, CSliceMut<uint8_t>((char*)data.data(), data.size())) != 0) continue;

        size_t found_offset = (size_t)-1;
        // Search for target binary patterns in this GPU object
        for (size_t i = 0; i <= data.size() - 16; i++) {
            if (memcmp(&data[i], target_bin.data(), 16) == 0 || memcmp(&data[i], target_guid.data(), 16) == 0) {
                found_offset = i;
                break;
            }
        }

        if (found_offset == (size_t)-1) {
            printf("[-] Target UUID not found in GPU object at 0x%lx\n", dev.address);
            continue;
        }

        real_uuid = guid_to_string(&data[found_offset]);

        uint8_t new_uuid[16];
        std::random_device rd; std::mt19937 gen(rd()); std::uniform_int_distribution<> dis(0, 255);
        for (int j = 0; j < 16; j++) new_uuid[j] = dis(gen);
        fake_uuid = guid_to_string(new_uuid);

        if (kernel->write_raw(dev.address + found_offset, CSliceRef<uint8_t>((char*)new_uuid, 16)) == 0) {
            printf("[+] GPU successfully spoofed via driver structures: %s -> %s\n",
                real_uuid.c_str(), fake_uuid.c_str());
            any_success = true;
        } else {
            printf("[-] Failed to write new UUID to GPU object\n");
        }
    }

    return any_success;
}

bool physical_spoof(const std::string& target_uuid, std::string& fake_uuid) {
    if (!kernel) return false;

    printf("Listing NVIDIA GPUs on the host...\n");
    system("lspci -nn | grep -i nvidia");

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
            if (isupper((unsigned char)new_uuid[i]))
                new_uuid[i] = toupper((unsigned char)hex_chars[dis(gen)]);
            else
                new_uuid[i] = hex_chars[dis(gen)];
        }
    }
    fake_uuid = new_uuid;
    printf("Generated fake UUID: %s\n", fake_uuid.c_str());


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
    size_t chunk_size = 16 * 1024 * 1024; // Increased to 16MB chunks for speed
    size_t overlap = 512;
    std::vector<uint8_t> buffer(chunk_size + overlap);

    uint8_t ascii_start = (uint8_t)target_uuid[0];
    uint8_t bin_start = target_bin[0];
    uint8_t guid_start = target_guid[0];
    size_t uuid_len = target_uuid.length();

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
        if (addr % (512ULL * 1024 * 1024) == 0) {
            printf("Scanning physical memory... %.1f GB / %.1f GB (Found: %zu)\r", (double)addr / (1024*1024*1024), (double)max_addr / (1024*1024*1024), found_count);
            fflush(stdout);
        }

        size_t to_read = std::min((uint64_t)buffer.size(), max_addr - addr);
        if (to_read < 16) break;

        if (phys_view.read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), to_read)) != 0) {
            continue;
        }

        // Single-pass search for all three formats
        for (size_t i = 0; i <= to_read - 16; ++i) {
            uint8_t b = buffer[i];

            // Match ASCII
            if (b == ascii_start && (to_read - i) >= uuid_len) {
                if (memcmp(buffer.data() + i, target_uuid.c_str(), uuid_len) == 0) {
                    printf("\nFound ASCII UUID at physical address %lx. Patching...\n", addr + i);
                    if (phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_uuid.c_str(), uuid_len)) == 0) {
                        found_count++;
                        memcpy(buffer.data() + i, fake_uuid.c_str(), uuid_len);
                        i += uuid_len - 1;
                        continue;
                    }
                }
            }

            // Match Binary (Straight)
            if (b == bin_start) {
                if (memcmp(buffer.data() + i, target_bin.data(), 16) == 0) {
                    printf("\nFound Binary UUID (Straight) at physical address %lx. Patching...\n", addr + i);
                    if (phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_bin.data(), 16)) == 0) {
                        found_count++;
                        memcpy(buffer.data() + i, fake_bin.data(), 16);
                        i += 15;
                        continue;
                    }
                }
            }

            // Match Binary (LE)
            if (b == guid_start) {
                if (memcmp(buffer.data() + i, target_guid.data(), 16) == 0) {
                    printf("\nFound Binary UUID (LE) at physical address %lx. Patching...\n", addr + i);
                    if (phys_view.write_raw(addr + i, CSliceRef<uint8_t>((char*)fake_guid.data(), 16)) == 0) {
                        found_count++;
                        memcpy(buffer.data() + i, fake_guid.data(), 16);
                        i += 15;
                        continue;
                    }
                }
            }
        }
    }

    printf("\nPhysical spoofing completed. Found and patched %zu occurrences.\n", found_count);
    return found_count > 0;
}
