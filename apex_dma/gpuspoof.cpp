#include "gpuspoof.h"
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>
#include <cstring>

namespace GPUSpoof {
    std::string real_uuid = "Not Found";
    std::string fake_uuid = "Not Found";

    std::string UUIDToString(const uint8_t* uuid) {
        std::stringstream ss;
        for (int i = 0; i < 16; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)uuid[i];
            if (i == 3 || i == 5 || i == 7 || i == 9) ss << "-";
        }
        return ss.str();
    }

    bool VerifyGpuDevice(Memory& mem, uint64_t ptr, uint32_t& discovered_offset) {
        if (ptr < 0xFFFF000000000000 || (ptr & 0x7) != 0) return false;

        const uint32_t offset_candidates[] = { 0x848, 0xC64, 0x854, 0xC60, 0x850, 0x9E4, 0x754, 0x74C, 0xB2C, 0xB30 };
        for (uint32_t off : offset_candidates) {
            uint8_t valid_flag = 0;
            if (mem.ReadKernel(ptr + off, valid_flag) && valid_flag == 1) {
                uint8_t uuid[16];
                if (mem.ReadKernelArray(ptr + off + 1, uuid, 16)) {
                    bool all_zero = true;
                    for (int i = 0; i < 16; i++) if (uuid[i] != 0) all_zero = false;
                    if (!all_zero) {
                        discovered_offset = off;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool Apply(Memory& mem) {
        uint64_t nv_base = mem.GetKernelModuleBase("nvlddmkm.sys");
        uint32_t nv_size = mem.GetKernelModuleSize("nvlddmkm.sys");

        if (!nv_base || !nv_size) {
            printf("[GPUSpoof] nvlddmkm.sys not found\n");
            return false;
        }
        printf("[GPUSpoof] nvlddmkm.sys found at 0x%lX (size: 0x%X)\n", nv_base, nv_size);

        // Instead of reading the whole driver (which can be huge), we read a portion for pattern scanning
        // and then perform direct pointer scanning in likely sections.
        const uint32_t scan_range = 0x2000000; // First 32MB
        uint32_t actual_scan = (nv_size < scan_range) ? nv_size : scan_range;
        std::vector<uint8_t> buffer(actual_scan);
        if (!mem.ReadKernelRobust(nv_base, buffer.data(), actual_scan)) {
            printf("[GPUSpoof] Failed to read nvlddmkm.sys header\n");
            return false;
        }

        uint32_t discovered_uuid_off = 0;
        std::vector<uint64_t> gpu_devices;

        // Pattern for GpuMgr: E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15
        size_t off = findPattern(buffer.data(), actual_scan, "E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15");
        if (off == (size_t)-1) {
            // Fallback to the other pattern
            off = findPattern(buffer.data(), nv_size, "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9");
            if (off == (size_t)-1) {
                printf("[GPUSpoof] Failed to find GpuMgr pattern\n");
                return false;
            }
            printf("[GPUSpoof] Found GpuMgr pattern (fallback) at offset 0x%lX\n", off);
        } else {
            printf("[GPUSpoof] Found GpuMgr pattern at offset 0x%lX\n", off);
        }

        uint32_t uuid_valid_offset = 0;
        uint64_t GpuMgrGetGpuFromId_ptr = 0;

        // Extract GpuMgrGetGpuFromId from the first pattern (E8 CALL)
        if (buffer[off] == 0xE8) {
            int32_t call_disp = 0;
            memcpy(&call_disp, &buffer[off + 1], 4);
            GpuMgrGetGpuFromId_ptr = nv_base + off + 5 + call_disp;

            // Search for the UUID offset inside GpuMgrGetGpuFromId
            // Instruction: lea r8, [rcx + offset] -> 4C 8D 81 XX XX XX XX
            size_t func_off = off + 5 + call_disp;
            if (func_off + 100 < nv_size) {
                for (int i = 0; i < 100; i++) {
                    if (buffer[func_off + i] == 0x4C && buffer[func_off + i + 1] == 0x8D && buffer[func_off + i + 2] == 0x81) {
                        memcpy(&uuid_valid_offset, &buffer[func_off + i + 3], 4);
                        uuid_valid_offset -= 1; // The offset in research is often the valid flag, UUID is +1
                        break;
                    }
                }
            }
        }

        if (uuid_valid_offset == 0) {
            // Try to extract from the pattern itself: 44 8B 80 XX XX XX XX
            // The pattern was: E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15
            // Offset from start of pattern to 44 8B 80 is 17 bytes. Displacement is at 20.
            if (off + 23 < nv_size && buffer[off + 17] == 0x44 && buffer[off + 18] == 0x8B && buffer[off + 19] == 0x80) {
                 memcpy(&uuid_valid_offset, &buffer[off + 20], 4);
                 printf("[GPUSpoof] Extracted UUID offset from pattern: 0x%X\n", uuid_valid_offset);
            }
        }

        if (uuid_valid_offset == 0) {
            uuid_valid_offset = 0x848; // Common hardcoded fallback
            printf("[GPUSpoof] Using hardcoded offset 0x848\n");
        } else {
            printf("[GPUSpoof] Discovered UUID offset: 0x%X\n", uuid_valid_offset);
        }

        // Strategy: Driver-wide pointer scan in potential identity regions
        printf("[GPUSpoof] Starting driver-wide identity scan...\n");
        for (uint32_t section_off = 0; section_off < actual_scan; section_off += 0x1000) {
             std::vector<uint8_t> page(0x1000);
             if (mem.ReadKernelRobust(nv_base + section_off, page.data(), 0x1000)) {
                  for (uint32_t i = 0; i < 0x1000; i += 8) {
                       uint64_t ptr = 0;
                       memcpy(&ptr, &page[i], 8);
                       if (VerifyGpuDevice(mem, ptr, discovered_uuid_off)) {
                            bool duplicate = false;
                            for (uint64_t existing : gpu_devices) if (existing == ptr) duplicate = true;
                            if (!duplicate) {
                                 gpu_devices.push_back(ptr);
                                 printf("[GPUSpoof] Discovered GPU device at 0x%lX (at driver offset 0x%X)\n", ptr, section_off + i);
                            }
                       }
                  }
             }
             if (!gpu_devices.empty() && section_off > 0x1000000) break; // Found some and scanned enough
        }

        // Discovery Strategy: Shallow-Crawl structural discovery
        if (gpu_devices.empty()) {
             printf("[GPUSpoof] Driver-wide scan failed. Starting recursive crawl discovery...\n");
             size_t sys_off = findPattern(buffer.data(), actual_scan, "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9");
             if (sys_off == (size_t)-1) sys_off = findPattern(buffer.data(), actual_scan, "48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 80");

             if (sys_off != (size_t)-1) {
                  int32_t disp = 0;
                  memcpy(&disp, &buffer[sys_off + 3], 4);
                  uint64_t g_system_ptr = nv_base + sys_off + 7 + disp;
                  uint64_t g_system = 0;
                  if (mem.ReadKernel(g_system_ptr, g_system) && g_system != 0) {
                       printf("[GPUSpoof] Crawling g_system structure at 0x%lX...\n", g_system);

                       const uint32_t crawl_size = 0x8000;
                       std::vector<uint8_t> block(crawl_size);
                       if (mem.ReadKernelRobust(g_system, block.data(), crawl_size)) {
                            for (uint32_t i = 0; i < crawl_size; i += 8) {
                                 uint64_t ptr1 = 0;
                                 memcpy(&ptr1, &block[i], 8);
                                 if (VerifyGpuDevice(mem, ptr1, discovered_uuid_off)) {
                                      gpu_devices.push_back(ptr1);
                                      continue;
                                 }

                                 // Shallow crawl: check memory pointed to by ptr1
                                 if (ptr1 > 0xFFFF000000000000 && (ptr1 & 0xFFF) == 0) {
                                      std::vector<uint8_t> sub_block(0x1000);
                                      if (mem.ReadKernelRobust(ptr1, sub_block.data(), 0x1000)) {
                                           for (uint32_t j = 0; j < 0x1000; j += 8) {
                                                uint64_t ptr2 = 0;
                                                memcpy(&ptr2, &sub_block[j], 8);
                                                if (VerifyGpuDevice(mem, ptr2, discovered_uuid_off)) {
                                                     gpu_devices.push_back(ptr2);
                                                     printf("[GPUSpoof] Discovered GPU via nested ptr at 0x%lX+0x%X -> 0x%lX\n", ptr1, j, ptr2);
                                                }
                                           }
                                      }
                                 }
                            }
                       }
                  }
             }
        }

        bool spoofed = false;

        for (uint64_t dev : gpu_devices) {
            uint8_t uuid[16];
            if (mem.ReadKernelArray(dev + discovered_uuid_off + 1, uuid, 16)) {
                real_uuid = UUIDToString(uuid);

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 255);

                for (int j = 0; j < 16; ++j) uuid[j] = dis(gen);

                if (mem.WriteKernelArray(dev + discovered_uuid_off + 1, uuid, 16)) {
                     fake_uuid = UUIDToString(uuid);
                     spoofed = true;
                     printf("[GPUSpoof] Spoofed GPU at 0x%lX: %s -> %s\n", dev, real_uuid.c_str(), fake_uuid.c_str());
                }
            }
        }

        return spoofed;
    }

    std::string GetRealUUID() { return real_uuid; }
    std::string GetFakeUUID() { return fake_uuid; }
}
