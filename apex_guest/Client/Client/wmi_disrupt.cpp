#include "wmi_disrupt.h"
#include <windows.h>
#include <iostream>

// Registry manipulation and WMI disruption removed to ensure system stability.
// All GPU spoofing is now handled host-side via physical memory patching.

void PrintNvidiaSmiInfo() {
    std::cout << "\n--- NVIDIA-SMI (Guest) Info ---" << std::endl;
    system("nvidia-smi -L");
    system("nvidia-smi --query-gpu=uuid,name --format=csv");
    std::cout << "-------------------------------" << std::endl;
}

void RestoreGPU() {
    // No-op
}
