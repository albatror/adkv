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

        uint64_t gpu_sys = 0;
        uint32_t gpu_mask = 0;

        // Deep discovery: scan g_system structure for gpu_sys pointer
        std::vector<uint8_t> system_buffer(0x1000);
        if (mem.ReadKernelRobust(g_system, system_buffer.data(), 0x1000)) {
             for (uint32_t i = 0; i < 0x1000; i += 8) {
                  uint64_t candidate = 0;
                  memcpy(&candidate, &system_buffer[i], 8);

                  if (candidate > 0xFFFF000000000000) { // Kernel address range
                       const uint32_t mask_offsets[] = { 0x754, 0x74C, 0x9E4, 0x764 };
                       for (uint32_t m_off : mask_offsets) {
                            uint32_t test_mask = 0;
                            if (mem.ReadKernel(candidate + m_off, test_mask) && test_mask != 0 && test_mask != 0xFFFFFFFF) {
                                 // Basic sanity check for mask: should have some bits set but not all (usually)
                                 // Also checking for some common bit patterns if possible
                                 gpu_sys = candidate;
                                 gpu_mask = test_mask;
                                 printf("[GPUSpoof] Discovered gpu_sys at g_system offset 0x%X: 0x%lX (mask 0x%X at 0x%X)\n", i, gpu_sys, gpu_mask, m_off);
                                 goto found_gpu_sys;
                            }
                       }
                  }
             }
        }

        if (gpu_sys == 0) {
            printf("[GPUSpoof] Failed to discover gpu_sys via deep scan. Logging neighborhood pointers:\n");
            for (int i = 0; i < 64; i++) {
                 uint64_t tmp = 0;
                 memcpy(&tmp, &system_buffer[i*8], 8);
                 if (tmp != 0) printf("[GPUSpoof] +0x%X: 0x%lX\n", i*8, tmp);
            }
            return false;
        }

found_gpu_sys:
        printf("[GPUSpoof] Final gpu_mask: 0x%X\n", gpu_mask);

        uint64_t gpu_sys_iterator = gpu_sys + 0x3C8D0;
        bool spoofed = false;

        if (gpu_mask == 0) {
             printf("[GPUSpoof] gpu_mask is 0, no devices to spoof.\n");
        }

        for (int i = 0; i < 32; i++) {
            if (!(gpu_mask & (1U << i))) continue;
            printf("[GPUSpoof] Searching for GPU instance %d...\n", i);

            uint64_t gpu_device = 0;
            uint32_t found_instance = 0;

            // Try different iterator starting points if the default fails
            const uint32_t iter_offsets[] = { 0x3C8D0, 0x3C8E0, 0x3CAD0, 0x3CAF0 };

            for (uint32_t iter_off : iter_offsets) {
                uint64_t current_iterator = gpu_sys + iter_off;
                bool found_in_this_iter = false;

                while(true) {
                    if (!mem.ReadKernel(current_iterator + 0x8, found_instance)) break;
                    if (found_instance == i) {
                        mem.ReadKernel(current_iterator, gpu_device);
                        if (gpu_device != 0) {
                             found_in_this_iter = true;
                             break;
                        }
                    }
                    current_iterator += 0x10;
                    if (current_iterator > gpu_sys + iter_off + 0x400) break;
                }

                if (found_in_this_iter) {
                     printf("[GPUSpoof] Found device for instance %d at offset 0x%X\n", i, iter_off);
                     break;
                }
            }

            if (gpu_device) {
                printf("[GPUSpoof] Found GPU device at 0x%lX\n", gpu_device);
                uint8_t uuid[16];
                if (mem.ReadKernelArray(gpu_device + uuid_valid_offset + 1, uuid, 16)) {
                    real_uuid = UUIDToString(uuid);

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, 255);

                    for (int j = 0; j < 16; ++j) uuid[j] = dis(gen);

                    if (mem.WriteKernelArray(gpu_device + uuid_valid_offset + 1, uuid, 16)) {
                         fake_uuid = UUIDToString(uuid);
                         spoofed = true;
                         printf("[GPUSpoof] Spoofed GPU %d: %s -> %s\n", i, real_uuid.c_str(), fake_uuid.c_str());
                    }
                }
            }
        }

        return spoofed;
    }

    std::string GetRealUUID() { return real_uuid; }
    std::string GetFakeUUID() { return fake_uuid; }
}
