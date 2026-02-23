#pragma once
#include <string>
#include <vector>
#include <cstdint>
#include <mutex>
#include "memory.h"

class Spoof {
public:
    static bool ScanAndSpoof();
    static std::string getRealUUID();
    static std::string getSpoofedUUID();
    static bool isSpoofed();

private:
    static std::string realUUID;
    static std::string spoofedUUID;
    static bool spoofed;
    static std::mutex dataMutex;

    static std::string generateRandomUUID(const std::string& original);
    static bool matchesUUIDPattern(const char* str);
    static void randomHex(char* s, int len, bool uppercase);

    static void InternalScan(MemoryViewBase<CBox<void>, CArc<void>>& phys_view, uint64_t max_addr);
};
