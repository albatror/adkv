#pragma once
#include <string>
#include "memory.h"

namespace GPUSpoof {
    bool Apply(Memory& mem);
    std::string GetRealUUID();
    std::string GetFakeUUID();
}
