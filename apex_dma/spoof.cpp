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

    std::cout << "--- Host PCI Info ---" << std::endl;
    system("lspci -nn | grep -i vga");
    std::cout << "----------------------" << std::endl;

    std::string saved_real_uuid;
    std::ifstream infile("real_gpu.txt");
    if (infile.is_open()) {
        std::getline(infile, saved_real_uuid);
        infile.close();
    }

    std::cout << "Scanning physical memory for GPU UUIDs..." << std::endl;

    std::map<std::string, int> candidates;
    std::map<std::string, std::string> canonical_to_original;

    uint64_t max_addr = MAX_PHYADDR;
    if (conn->vtbl_physicalmemory) {
        auto metadata = conn->metadata();
        if (metadata.max_address > 0) max_addr = metadata.max_address;
    } else if (kernel->vtbl_physicalmemory) {
        auto metadata = kernel->physicalmemory_metadata();
        if (metadata.max_address > 0) max_addr = metadata.max_address;
    }

    const size_t chunk_size = 0x100000; // 1MB
    std::vector<uint8_t> buffer(chunk_size);

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
        size_t read_sz = chunk_size;
        if (addr + read_sz > max_addr) read_sz = max_addr - addr;

        bool success = false;
        if (conn->vtbl_physicalmemory && conn->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) success = true;
        else if (kernel->vtbl_physicalmemory && kernel->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) success = true;

        if (!success) continue;

        for (size_t i = 0; i < read_sz - 40; ++i) {
            if (buffer[i] == 'G' && buffer[i+1] == 'P' && buffer[i+2] == 'U' && buffer[i+3] == '-') {
                std::string current_uuid((char*)&buffer[i], 40);
                bool valid = true;
                int dash_count = 0;
                for(int k=4; k<40; k++) {
                    if (!is_uuid_char(current_uuid[k])) { valid = false; break; }
                    if (current_uuid[k] == '-') dash_count++;
                }
                if (valid && dash_count == 4) {
                    std::string lower = to_lower(current_uuid);
                    candidates[lower]++;
                    if (canonical_to_original.find(lower) == canonical_to_original.end() || (std::any_of(current_uuid.begin(), current_uuid.end(), ::islower))) {
                         canonical_to_original[lower] = current_uuid;
                    }
                }
            }
        }
    }

    if (candidates.empty()) {
        std::cout << "No GPU UUID found in memory." << std::endl;
        return false;
    }

    std::cout << "Found candidates (aggregated case-insensitively):" << std::endl;
    for (auto const& [uuid, count] : candidates) {
        std::cout << "  " << uuid << " (x" << count << ")" << std::endl;
    }

    std::string found_real_uuid;
    if (!saved_real_uuid.empty()) {
        std::string lower_saved = to_lower(saved_real_uuid);
        if (candidates.count(lower_saved)) found_real_uuid = canonical_to_original[lower_saved];
    }

    if (found_real_uuid.empty()) {
        int max_count = -1;
        std::string best_lower;
        for (auto const& [uuid, count] : candidates) {
            if (count > max_count) { max_count = count; best_lower = uuid; }
        }
        found_real_uuid = canonical_to_original[best_lower];
        if (saved_real_uuid.empty()) {
            std::ofstream outfile("real_gpu.txt");
            outfile << found_real_uuid;
            outfile.close();
            saved_real_uuid = found_real_uuid;
        }
    }

    real_gpu_uuid = saved_real_uuid;

    if (!compare_uuid(found_real_uuid, real_gpu_uuid)) {
        fake_gpu_uuid = found_real_uuid;
        gpu_spoofed = true;
        std::cout << "Already patched!" << std::endl;
    } else {
        fake_gpu_uuid = generate_random_uuid();
        std::cout << "Applying GPU spoofing (replacing all occurrences)..." << std::endl;
        int replaced_count = 0;
        int binary_replaced_count = 0;

        std::vector<uint8_t> real_bytes_be = uuid_to_bytes(real_gpu_uuid);
        std::vector<uint8_t> fake_bytes_be = uuid_to_bytes(fake_gpu_uuid);
        std::vector<uint8_t> real_bytes_le = uuid_to_guid_bytes(real_gpu_uuid);
        std::vector<uint8_t> fake_bytes_le = uuid_to_guid_bytes(fake_gpu_uuid);

        for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
            size_t read_sz = chunk_size;
            if (addr + read_sz > max_addr) read_sz = max_addr - addr;

            bool success = false;
            if (conn->vtbl_physicalmemory && conn->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) success = true;
            else if (kernel->vtbl_physicalmemory && kernel->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) success = true;

            if (!success) continue;

            bool chunk_modified = false;
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
        }
    }

    std::cout << "REAL GPU-UUID " << real_gpu_uuid << std::endl;
    std::cout << "FAKE GPU-UUID " << fake_gpu_uuid << std::endl;
    std::cout << "\n--- You can now start the game! ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    return true;
}
