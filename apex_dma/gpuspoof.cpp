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

    bool IsValidUUID(const uint8_t* uuid) {
        bool all_zero = true;
        bool all_ones = true;
        bool all_same = true;
        for (int i = 0; i < 16; i++) {
            if (uuid[i] != 0x00) all_zero = false;
            if (uuid[i] != 0xFF) all_ones = false;
            if (uuid[i] != uuid[0]) all_same = false;
        }
        return !all_zero && !all_ones && !all_same;
    }

    bool VerifyGpuDevice(Memory& mem, uint64_t ptr, uint32_t& discovered_offset) {
        if (ptr < 0xFFFF000000000000 || (ptr & 0xF) != 0) return false;

        // Read a large chunk of the potential object for internal scanning
        uint8_t obj[0x2000];
        if (!mem.ReadKernelRobust(ptr, obj, 0x2000)) return false;

        // Check common offsets first for speed
        const uint32_t common_offs[] = { 0x848, 0xC64, 0xB2C, 0xB30, 0x854, 0xC60, 0x850, 0x9E4, 0x754, 0x74C };
        for (uint32_t off : common_offs) {
            if (off < 0x1F00 && (obj[off] == 1 || obj[off] == 2)) {
                if (IsValidUUID(&obj[off + 1])) {
                    discovered_offset = off;
                    return true;
                }
            }
        }

        // Brute-force scan the object for flag + UUID pattern
        for (uint32_t j = 0x400; j < 0x1F00; j++) {
            if (obj[j] > 0 && obj[j] < 4) {
                if (IsValidUUID(&obj[j+1])) {
                    discovered_offset = j;
                    printf("[GPUSpoof] Discovered identity at non-standard object offset 0x%X\n", j);
                    return true;
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

        const uint32_t header_scan = 0x2000000;
        uint32_t actual_header = (nv_size < header_scan) ? nv_size : header_scan;
        std::vector<uint8_t> buffer(actual_header);
        if (!mem.ReadKernelRobust(nv_base, buffer.data(), actual_header)) {
            printf("[GPUSpoof] Failed to read nvlddmkm.sys header\n");
            return false;
        }

        uint32_t discovered_uuid_off = 0;
        std::vector<uint64_t> gpu_devices;

        // Strategy 1: Comprehensive Driver-wide Pointer Scan
        printf("[GPUSpoof] Starting comprehensive driver identity scan...\n");
        const uint32_t chunkSize = 0x20000; // 128KB
        std::vector<uint8_t> page(chunkSize);
        for (uint32_t section_off = 0; section_off < nv_size; section_off += chunkSize) {
             if (mem.ReadKernelRobust(nv_base + section_off, page.data(), chunkSize)) {
                  for (uint32_t i = 0; i < chunkSize; i += 8) {
                       uint64_t ptr = 0;
                       memcpy(&ptr, &page[i], 8);
                       if (ptr > 0xFFFF000000000000) {
                            if (VerifyGpuDevice(mem, ptr, discovered_uuid_off)) {
                                 bool duplicate = false;
                                 for (uint64_t existing : gpu_devices) if (existing == ptr) duplicate = true;
                                 if (!duplicate) {
                                      gpu_devices.push_back(ptr);
                                      printf("[GPUSpoof] Identified GPU at 0x%lX (Driver+0x%X, UUID Off: 0x%X)\n", ptr, section_off + i, discovered_uuid_off);
                                 }
                            }
                       }
                  }
             }
             if (!gpu_devices.empty() && section_off > 0x1000000) break;
        }

        // Strategy 2: Deep System Crawl
        if (gpu_devices.empty()) {
             printf("[GPUSpoof] Driver scan failed. Starting deep system crawl...\n");
             size_t sys_off = findPattern(buffer.data(), actual_header, "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9");
             if (sys_off == (size_t)-1) sys_off = findPattern(buffer.data(), actual_header, "48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 80");

             if (sys_off != (size_t)-1) {
                  int32_t disp = 0;
                  memcpy(&disp, &buffer[sys_off + 3], 4);
                  uint64_t g_system_ptr = nv_base + sys_off + 7 + disp;
                  uint64_t g_system = 0;
                  if (mem.ReadKernel(g_system_ptr, g_system) && g_system != 0) {
                       printf("[GPUSpoof] Crawling pool block at 0x%lX...\n", g_system);
                       const uint32_t crawl_size = 0x10000;
                       std::vector<uint8_t> block(crawl_size);
                       if (mem.ReadKernelRobust(g_system, block.data(), crawl_size)) {
                            for (uint32_t i = 0; i < crawl_size; i += 8) {
                                 uint64_t ptr1 = 0;
                                 memcpy(&ptr1, &block[i], 8);
                                 if (ptr1 > 0xFFFF000000000000) {
                                      if (VerifyGpuDevice(mem, ptr1, discovered_uuid_off)) {
                                           gpu_devices.push_back(ptr1);
                                           continue;
                                      }

                                      if ((ptr1 & 0xFFF) == 0) {
                                           std::vector<uint8_t> sub_block(0x2000);
                                           if (mem.ReadKernelRobust(ptr1, sub_block.data(), 0x2000)) {
                                                for (uint32_t j = 0; j < 0x2000; j += 8) {
                                                     uint64_t ptr2 = 0;
                                                     memcpy(&ptr2, &sub_block[j], 8);
                                                     if (ptr2 > 0xFFFF000000000000) {
                                                          if (VerifyGpuDevice(mem, ptr2, discovered_uuid_off)) {
                                                               gpu_devices.push_back(ptr2);
                                                               printf("[GPUSpoof] Found nested GPU: 0x%lX -> 0x%lX\n", ptr1, ptr2);
                                                          }
                                                     }
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
