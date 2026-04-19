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

    bool Apply(Memory& mem) {
        uint64_t nv_base = mem.GetKernelModuleBase("nvlddmkm.sys");
        uint32_t nv_size = mem.GetKernelModuleSize("nvlddmkm.sys");

        if (!nv_base || !nv_size) {
            printf("[GPUSpoof] nvlddmkm.sys not found\n");
            return false;
        }
        printf("[GPUSpoof] nvlddmkm.sys found at 0x%lX (size: 0x%X)\n", nv_base, nv_size);

        std::vector<uint8_t> buffer(nv_size);
        if (!mem.ReadKernelRobust(nv_base, buffer.data(), nv_size)) {
            printf("[GPUSpoof] Failed to read nvlddmkm.sys\n");
            return false;
        }

        // Pattern for GpuMgr: E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15
        size_t off = findPattern(buffer.data(), nv_size, "E8 ? ? ? ? 48 8B D8 48 85 C0 0F 84 ? ? ? ? 44 8B 80 ? ? ? ? 48 8D 15");
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

        // Logic to get GpuMgr/System pointers
        // M4L1: 48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9
        size_t sys_off = findPattern(buffer.data(), nv_size, "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9");
        if (sys_off == (size_t)-1) {
            // Try another pattern for g_system
            sys_off = findPattern(buffer.data(), nv_size, "48 8B 05 ? ? ? ? 48 85 C0 74 ? 48 8B 80");
            if (sys_off == (size_t)-1) {
                printf("[GPUSpoof] Failed to find system pattern\n");
                return false;
            }
            printf("[GPUSpoof] Found system pattern (alternate) at offset 0x%lX\n", sys_off);
        } else {
             printf("[GPUSpoof] Found system pattern at offset 0x%lX\n", sys_off);
        }

        int32_t disp = 0;
        memcpy(&disp, &buffer[sys_off + 3], 4);
        uint64_t g_system_ptr = nv_base + sys_off + 7 + disp;
        printf("[GPUSpoof] g_system_ptr: 0x%lX\n", g_system_ptr);

        uint64_t g_system = 0;
        // Try to read from buffer first if within range
        if (g_system_ptr >= nv_base && g_system_ptr + 8 <= nv_base + nv_size) {
             memcpy(&g_system, &buffer[g_system_ptr - nv_base], 8);
             printf("[GPUSpoof] Read g_system from buffer: 0x%lX\n", g_system);
        }

        if (g_system == 0) {
            if (!mem.ReadKernel(g_system_ptr, g_system) || !g_system) {
                printf("[GPUSpoof] Failed to read g_system from memory at 0x%lX\n", g_system_ptr);
                return false;
            }
            printf("[GPUSpoof] g_system: 0x%lX\n", g_system);
        }

        bool spoofed = false;
        std::vector<uint64_t> gpu_devices;

        // Discovery Strategy: Direct Pointer Scan
        // We scan a large block of g_system for anything that looks like a gpu_device
        std::vector<uint8_t> system_block(0x4000);
        if (mem.ReadKernelRobust(g_system, system_block.data(), 0x4000)) {
             for (uint32_t i = 0; i < 0x4000; i += 8) {
                  uint64_t ptr = 0;
                  memcpy(&ptr, &system_block[i], 8);

                  if (ptr > 0xFFFF000000000000) {
                       uint8_t valid_flag = 0;
                       if (mem.ReadKernel(ptr + uuid_valid_offset, valid_flag) && valid_flag == 1) {
                            // Verify it's not a duplicate
                            bool duplicate = false;
                            for (uint64_t existing : gpu_devices) if (existing == ptr) duplicate = true;
                            if (!duplicate) {
                                 gpu_devices.push_back(ptr);
                                 printf("[GPUSpoof] Discovered potential GPU device at 0x%lX (via g_system+0x%X)\n", ptr, i);
                            }
                       }
                  }
             }
        }

        // Fallback: search neighborhood of common gpu_sys offsets for the mask
        if (gpu_devices.empty()) {
             printf("[GPUSpoof] Direct scan failed. Trying structural discovery...\n");
             for (uint32_t i = 0; i < 0x1000; i += 8) {
                  uint64_t candidate = 0;
                  memcpy(&candidate, &system_block[i], 8);
                  if (candidate > 0xFFFF000000000000) {
                       uint32_t mask = 0;
                       if (mem.ReadKernel(candidate + 0x754, mask) && mask != 0 && mask != 0xFFFFFFFF) {
                            printf("[GPUSpoof] Found gpu_sys candidate at +0x%X: 0x%lX (mask 0x%X)\n", i, candidate, mask);
                            // Try to find devices from this gpu_sys
                            for (uint32_t iter_off : {0x3C8D0, 0x3C8E0, 0x3CAD0, 0x3CAF0}) {
                                 uint64_t iter = candidate + iter_off;
                                 for (int j = 0; j < 64; j++) {
                                      uint64_t dev = 0;
                                      if (mem.ReadKernel(iter + (j * 0x10), dev) && dev > 0xFFFF000000000000) {
                                           uint8_t flag = 0;
                                           if (mem.ReadKernel(dev + uuid_valid_offset, flag) && flag == 1) {
                                                gpu_devices.push_back(dev);
                                           }
                                      }
                                 }
                                 if (!gpu_devices.empty()) break;
                            }
                       }
                  }
                  if (!gpu_devices.empty()) break;
             }
        }

        for (uint64_t dev : gpu_devices) {
            uint8_t uuid[16];
            if (mem.ReadKernelArray(dev + uuid_valid_offset + 1, uuid, 16)) {
                real_uuid = UUIDToString(uuid);

                std::random_device rd;
                std::mt19937 gen(rd());
                std::uniform_int_distribution<> dis(0, 255);

                for (int j = 0; j < 16; ++j) uuid[j] = dis(gen);

                if (mem.WriteKernelArray(dev + uuid_valid_offset + 1, uuid, 16)) {
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
