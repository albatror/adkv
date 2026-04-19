#include "gpuspoof.h"
#include <vector>
#include <random>
#include <sstream>
#include <iomanip>

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

        // Pattern for GpuMgr: 48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9
        // This is from M4L1's post.
        std::vector<uint8_t> buffer(nv_size);
        if (!mem.ReadKernelArray(nv_base, buffer.data(), nv_size)) {
            printf("[GPUSpoof] Failed to read nvlddmkm.sys\n");
            return false;
        }

        size_t off = findPattern(buffer.data(), nv_size, "48 8B 05 ? ? ? ? 4C 8B F2 44 8B E9");
        if (off == (size_t)-1) {
            printf("[GPUSpoof] Failed to find GpuMgr pattern\n");
            return false;
        }

        int32_t disp = 0;
        memcpy(&disp, &buffer[off + 3], 4);
        uint64_t g_system_ptr = nv_base + off + 7 + disp;

        uint64_t g_system = 0;
        if (!mem.ReadKernel(g_system_ptr, g_system) || !g_system) {
            printf("[GPUSpoof] Failed to read g_system\n");
            return false;
        }

        uint64_t gpu_sys = 0;
        if (!mem.ReadKernel(g_system + 0x1C0, gpu_sys) || !gpu_sys) {
            printf("[GPUSpoof] Failed to read gpu_sys\n");
            return false;
        }

        uint32_t gpu_mask = 0;
        if (!mem.ReadKernel(gpu_sys + 0x754, gpu_mask)) {
            printf("[GPUSpoof] Failed to read gpu_mask\n");
            return false;
        }

        uint64_t gpu_mgr = 0;
        if (!mem.ReadKernel(gpu_sys + 0x3CAD0, gpu_mgr)) {
             // Fallback or handle differently? M4L1 uses this to check.
        }

        uint64_t gpu_sys_iterator = gpu_sys + 0x3C8D0;
        bool spoofed = false;

        for (int i = 0; i < 32; i++) {
            if (!(gpu_mask & (1U << i))) continue;

            uint64_t gpu_device = 0;
            uint32_t found_instance = 0;

            // Search for the device instance in the iterator
            // This logic is a simplified version of M4L1's loop
            uint64_t current_iterator = gpu_sys_iterator;
            while(true) {
                if (!mem.ReadKernel(current_iterator + 0x8, found_instance)) break;
                if (found_instance == i) {
                    mem.ReadKernel(current_iterator, gpu_device);
                    break;
                }
                current_iterator += 0x10;
                if (current_iterator > gpu_sys_iterator + 0x1000) break; // Safety
            }

            if (gpu_device) {
                uint8_t uuid[16];
                if (mem.ReadKernelArray(gpu_device + 0x849, uuid, 16)) {
                    real_uuid = UUIDToString(uuid);

                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_int_distribution<> dis(0, 255);

                    for (int j = 0; j < 16; ++j) uuid[j] = dis(gen);

                    if (mem.WriteKernelArray(gpu_device + 0x849, uuid, 16)) {
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
