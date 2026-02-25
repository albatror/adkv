#include "spoof.h"
#include "memory.h"
#include <iostream>
#include <fstream>
#include <random>
#include <thread>
#include <chrono>
#include <vector>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <map>

std::string real_gpu_uuid;
std::string fake_gpu_uuid;
bool gpu_spoofed = false;

std::string generate_random_uuid() {
    static const char hex_chars[] = "0123456789abcdef";
    std::string uuid = "GPU-";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 15);

    for (int i = 0; i < 8; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 4; ++i) uuid += hex_chars[dis(gen)];
    uuid += "-";
    for (int i = 0; i < 12; ++i) uuid += hex_chars[dis(gen)];

    return uuid;
}

bool is_uuid_char(char c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F') || c == '-';
}

bool compare_uuid(const std::string& a, const std::string& b) {
    if (a.length() != b.length()) return false;
    return std::equal(a.begin(), a.end(), b.begin(),
                      [](char c1, char c2) {
                          return std::tolower((unsigned char)c1) == std::tolower((unsigned char)c2);
                      });
}

std::string to_lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

std::vector<uint8_t> uuid_to_bytes(const std::string& uuid_str) {
    std::string hex;
    for (char c : uuid_str) {
        if (std::isxdigit((unsigned char)c)) {
            hex += c;
        }
    }
    if (hex.length() != 32) return {};

    std::vector<uint8_t> bytes;
    for (size_t i = 0; i < 32; i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

std::vector<uint8_t> uuid_to_guid_bytes(const std::string& uuid_str) {
    std::string hex;
    for (char c : uuid_str) {
        if (std::isxdigit((unsigned char)c)) {
            hex += c;
        }
    }
    if (hex.length() != 32) return {};

    std::vector<uint8_t> bytes(16);
    uint32_t d1 = (uint32_t)strtoul(hex.substr(0, 8).c_str(), nullptr, 16);
    bytes[0] = d1 & 0xFF; bytes[1] = (d1 >> 8) & 0xFF; bytes[2] = (d1 >> 16) & 0xFF; bytes[3] = (d1 >> 24) & 0xFF;
    uint16_t d2 = (uint16_t)strtoul(hex.substr(8, 4).c_str(), nullptr, 16);
    bytes[4] = d2 & 0xFF; bytes[5] = (d2 >> 8) & 0xFF;
    uint16_t d3 = (uint16_t)strtoul(hex.substr(12, 4).c_str(), nullptr, 16);
    bytes[6] = d3 & 0xFF; bytes[7] = (d3 >> 8) & 0xFF;
    for (int i = 0; i < 8; i++) {
        bytes[8+i] = (uint8_t)strtol(hex.substr(16 + i*2, 2).c_str(), nullptr, 16);
    }
    return bytes;
}

bool spoof_gpu_uuid() {
    if (!conn || !kernel) {
        return false;
    }

    std::cout << "--- Host Hardware Info ---" << std::endl;
    system("lspci -nn | grep -i vga");
    std::cout << "--------------------------" << std::endl;

    std::ifstream infile("real_gpu.txt");
    if (infile.is_open()) {
        std::getline(infile, real_gpu_uuid);
        infile.close();
    }

    if (real_gpu_uuid.empty()) {
        std::cout << "Error: real_gpu.txt is missing or empty. Cannot proceed with stable spoofing." << std::endl;
        std::cout << "Please create real_gpu.txt with your GPU UUID (e.g., GPU-xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx)." << std::endl;
        return false;
    }

    fake_gpu_uuid = generate_random_uuid();
    std::cout << "Target Real GPU UUID: " << real_gpu_uuid << std::endl;
    std::cout << "Generated Fake GPU UUID: " << fake_gpu_uuid << std::endl;

    uint64_t max_addr = MAX_PHYADDR;
    if (conn->vtbl_physicalmemory) {
        auto metadata = conn->metadata();
        if (metadata.max_address > 0) max_addr = metadata.max_address;
    } else if (kernel->vtbl_physicalmemory) {
        auto metadata = kernel->physicalmemory_metadata();
        if (metadata.max_address > 0) max_addr = metadata.max_address;
    }

    std::cout << "Starting physical memory scan and patch (Max: " << (max_addr / 0x40000000) << "GB)..." << std::endl;

    const size_t chunk_size = 0x200000; // 2MB for faster scanning
    std::vector<uint8_t> buffer(chunk_size);
    int replaced_count = 0;
    int binary_replaced_count = 0;

    std::vector<uint8_t> real_bytes_be = uuid_to_bytes(real_gpu_uuid);
    std::vector<uint8_t> fake_bytes_be = uuid_to_bytes(fake_gpu_uuid);
    std::vector<uint8_t> real_bytes_le = uuid_to_guid_bytes(real_gpu_uuid);
    std::vector<uint8_t> fake_bytes_le = uuid_to_guid_bytes(fake_gpu_uuid);

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
        if (addr % (0x40000000) == 0) { // Progress every 1GB
            std::cout << "  Progress: " << (addr / 0x40000000) << "GB / " << (max_addr / 0x40000000) << "GB" << std::endl;
        }

        size_t read_sz = chunk_size;
        if (addr + read_sz > max_addr) read_sz = max_addr - addr;

        bool success = false;
        if (conn->vtbl_physicalmemory && conn->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) success = true;
        else if (kernel->vtbl_physicalmemory && kernel->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) success = true;

        if (!success) continue;

        bool chunk_modified = false;
        // String scan
        for (size_t i = 0; i < read_sz - 40; ++i) {
            if (buffer[i] == 'G' && buffer[i+1] == 'P' && buffer[i+2] == 'U' && buffer[i+3] == '-') {
                std::string current_uuid((char*)&buffer[i], 40);
                if (compare_uuid(current_uuid, real_gpu_uuid)) {
                    memcpy(&buffer[i], fake_gpu_uuid.c_str(), 40);
                    replaced_count++;
                    chunk_modified = true;
                }
            }
        }
        // Binary scan
        if (!real_bytes_be.empty()) {
            for (size_t i = 0; i < read_sz - 16; ++i) {
                if (memcmp(&buffer[i], real_bytes_be.data(), 16) == 0) {
                    memcpy(&buffer[i], fake_bytes_be.data(), 16);
                    binary_replaced_count++;
                    chunk_modified = true;
                } else if (memcmp(&buffer[i], real_bytes_le.data(), 16) == 0) {
                    memcpy(&buffer[i], fake_bytes_le.data(), 16);
                    binary_replaced_count++;
                    chunk_modified = true;
                }
            }
        }

        if (chunk_modified) {
            if (conn->vtbl_physicalmemory) conn->phys_view().write_raw(addr, CSliceRef<uint8_t>((char*)buffer.data(), read_sz));
            else if (kernel->vtbl_physicalmemory) kernel->phys_view().write_raw(addr, CSliceRef<uint8_t>((char*)buffer.data(), read_sz));
        }
    }

    if (replaced_count > 0 || binary_replaced_count > 0) {
        gpu_spoofed = true;
        std::cout << "Spoofing applied! Replaced " << replaced_count << " string occurrences and " << binary_replaced_count << " binary occurrences." << std::endl;
    } else {
        std::cout << "No matching GPU UUID found in physical memory. It might already be patched or the UUID in real_gpu.txt is incorrect." << std::endl;
        // Check for already patched state by finding any GPU- string
        // (Actually, a targeted scan didn't find anything, so we can't be sure)
    }

    std::cout << "REAL GPU-UUID: " << real_gpu_uuid << std::endl;
    std::cout << "FAKE GPU-UUID: " << fake_gpu_uuid << std::endl;
    std::cout << "\n--- You can now start the game! ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    return true;
}
