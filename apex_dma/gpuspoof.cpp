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
            uuid_valid_offset = 0x848; // Common hardcoded fallback
            printf("[GPUSpoof] Using hardcoded offset 0x848\n");
        } else {
            printf("[GPUSpoof] Discovered UUID offset: 0x%X\n", uuid_valid_offset);
        }

        // Logic to get GpuMgr/System pointers
        // M4L1: 48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9
        size_t sys_off = findPattern(buffer.data(), nv_size, "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9");
        if (sys_off == (size_t)-1) return false;

        int32_t disp = 0;
        memcpy(&disp, &buffer[sys_off + 3], 4);
        uint64_t g_system_ptr = nv_base + sys_off + 7 + disp;

        uint64_t g_system = 0;
        if (!mem.ReadKernel(g_system_ptr, g_system) || !g_system) return false;

        uint64_t gpu_sys = 0;
        if (!mem.ReadKernel(g_system + 0x1C0, gpu_sys) || !gpu_sys) return false;

        uint32_t gpu_mask = 0;
        if (!mem.ReadKernel(gpu_sys + 0x754, gpu_mask)) return false;

        uint64_t gpu_sys_iterator = gpu_sys + 0x3C8D0;
        bool spoofed = false;

        for (int i = 0; i < 32; i++) {
            if (!(gpu_mask & (1U << i))) continue;

            uint64_t gpu_device = 0;
            uint32_t found_instance = 0;

            uint64_t current_iterator = gpu_sys_iterator;
            while(true) {
                if (!mem.ReadKernel(current_iterator + 0x8, found_instance)) break;
                if (found_instance == i) {
                    mem.ReadKernel(current_iterator, gpu_device);
                    break;
                }
                current_iterator += 0x10;
                if (current_iterator > gpu_sys_iterator + 0x1000) break;
            }

            if (gpu_device) {
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
