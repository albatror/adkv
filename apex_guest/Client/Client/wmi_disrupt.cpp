#include "wmi_disrupt.h"
#include <windows.h>
#include <iostream>
#include <string>
#include <vector>

// GPU UUID retrieval logic
std::string GetGPUUUIDFromNvidiaSmi() {
    char buffer[128];
    std::string result = "";
    // Execute command and capture output
    FILE* pipe = _popen("nvidia-smi -L", "r");
    if (!pipe) return "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    _pclose(pipe);

    // Expected output format:
    // GPU 0: NVIDIA GeForce RTX 2070 SUPER (UUID: GPU-83904901-65ad-9709-a59d-7a664aa707c1)
    size_t pos = result.find("UUID: ");
    if (pos != std::string::npos) {
        std::string uuid = result.substr(pos + 6);
        size_t end_pos = uuid.find(")");
        if (end_pos != std::string::npos) {
            return uuid.substr(0, end_pos);
        }
    }
    return "";
}

void PrintNvidiaSmiInfo() {
    std::cout << "\n--- NVIDIA-SMI (Guest) Info ---" << std::endl;
    system("nvidia-smi -L");
    system("nvidia-smi --query-gpu=uuid,name --format=csv");
    std::cout << "-------------------------------" << std::endl;
}

void RestoreGPU() {
    // No-op
}
