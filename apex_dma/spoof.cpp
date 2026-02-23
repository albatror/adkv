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

std::vector<uint8_t> uuid_to_bytes(const std::string& uuid_str) {
    std::string hex;
    for (char c : uuid_str) {
        if (std::isxdigit((unsigned char)c)) {
            hex += c;
        }
    }

    std::vector<uint8_t> bytes;
    if (hex.length() != 32) return bytes;

    for (size_t i = 0; i < 32; i += 2) {
        std::string byteString = hex.substr(i, 2);
        uint8_t byte = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
        bytes.push_back(byte);
    }
    return bytes;
}

bool spoof_gpu_uuid() {
    if (!conn || !kernel) {
        return false;
    }

    std::string saved_real_uuid;
    std::ifstream infile("real_gpu.txt");
    if (infile.is_open()) {
        std::getline(infile, saved_real_uuid);
        infile.close();
    }

    std::cout << "Applying GPU spoofing (replacing all occurrences)..." << std::endl;

    std::string found_real_uuid;
    fake_gpu_uuid = generate_random_uuid();
    int replaced_count = 0;
    int binary_replaced_count = 0;

    std::vector<uint8_t> real_bytes;
    std::vector<uint8_t> fake_bytes = uuid_to_bytes(fake_gpu_uuid);

    // Scan physical memory
    uint64_t max_addr = MAX_PHYADDR;

    // Try to get max address from connector metadata if available
    if (conn->vtbl_physicalmemory) {
        auto metadata = conn->metadata();
        if (metadata.max_address > 0) {
            max_addr = metadata.max_address;
        }
    } else if (kernel->vtbl_physicalmemory) {
        auto metadata = kernel->physicalmemory_metadata();
        if (metadata.max_address > 0) {
            max_addr = metadata.max_address;
        }
    }

    const size_t chunk_size = 0x100000; // 1MB
    std::vector<uint8_t> buffer(chunk_size);

    for (uint64_t addr = 0; addr < max_addr; addr += chunk_size) {
        size_t read_sz = chunk_size;
        if (addr + read_sz > max_addr) read_sz = max_addr - addr;

        // Try reading via conn first, then kernel for compatibility
        bool success = false;
        if (conn->vtbl_physicalmemory && conn->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) {
            success = true;
        } else if (kernel->vtbl_physicalmemory && kernel->phys_view().read_raw_into(addr, CSliceMut<uint8_t>((char*)buffer.data(), read_sz)) == 0) {
            success = true;
        }

        if (!success) continue;

        bool chunk_modified = false;
        for (size_t i = 0; i < read_sz - 40; ++i) {
            if (buffer[i] == 'G' && buffer[i+1] == 'P' && buffer[i+2] == 'U' && buffer[i+3] == '-') {
                // Potential UUID found
                std::string current_uuid((char*)&buffer[i], 40);

                // Validate UUID format roughly: GPU- + hex chars and dashes
                bool valid = true;
                int dash_count = 0;
                for(int k=4; k<40; k++) {
                    if (!is_uuid_char(current_uuid[k])) {
                        valid = false;
                        break;
                    }
                    if (current_uuid[k] == '-') dash_count++;
                }
                if (!valid || dash_count != 4) continue;

                if (found_real_uuid.empty()) {
                    found_real_uuid = current_uuid;

                    if (!saved_real_uuid.empty() && !compare_uuid(found_real_uuid, saved_real_uuid)) {
                        // Already spoofed with a different UUID!
                        real_gpu_uuid = saved_real_uuid;
                        fake_gpu_uuid = found_real_uuid;
                        gpu_spoofed = true;
                        std::cout << "Already patched!" << std::endl;
                        std::cout << "REAL GPU-UUID " << real_gpu_uuid << std::endl;
                        std::cout << "FAKE GPU-UUID " << fake_gpu_uuid << std::endl;
                        std::cout << "\n--- You can now start the game! ---" << std::endl;
                        std::this_thread::sleep_for(std::chrono::seconds(30));
                        return true;
                    }

                    // Not spoofed yet, or we found the real UUID (ignoring case), or we didn't have a saved one.
                    if (saved_real_uuid.empty()) {
                        std::ofstream outfile("real_gpu.txt");
                        outfile << found_real_uuid;
                        outfile.close();
                        saved_real_uuid = found_real_uuid;
                    }

                    real_gpu_uuid = saved_real_uuid;
                    real_bytes = uuid_to_bytes(real_gpu_uuid);
                    std::cout << "REAL GPU-UUID " << real_gpu_uuid << std::endl;
                    std::cout << "FAKE GPU-UUID " << fake_gpu_uuid << std::endl;
                }

                if (compare_uuid(current_uuid, real_gpu_uuid)) {
                    // Replace it in buffer
                    memcpy(&buffer[i], fake_gpu_uuid.c_str(), 40);
                    replaced_count++;
                    chunk_modified = true;
                }
            }
        }

        // Binary scan
        if (!real_bytes.empty()) {
            for (size_t i = 0; i < read_sz - 16; ++i) {
                if (memcmp(&buffer[i], real_bytes.data(), 16) == 0) {
                    memcpy(&buffer[i], fake_bytes.data(), 16);
                    binary_replaced_count++;
                    chunk_modified = true;
                }
            }
        }

        if (chunk_modified) {
            // Write back to physical memory
            if (conn->vtbl_physicalmemory && conn->phys_view().write_raw(addr, CSliceRef<uint8_t>((char*)buffer.data(), read_sz)) != 0) {
                if (kernel->vtbl_physicalmemory) {
                    kernel->phys_view().write_raw(addr, CSliceRef<uint8_t>((char*)buffer.data(), read_sz));
                }
            }
        }
    }

    if (replaced_count > 0 || binary_replaced_count > 0) {
        gpu_spoofed = true;
        std::cout << "Spoofing applied! Replaced " << replaced_count << " string occurrences and " << binary_replaced_count << " binary occurrences." << std::endl;
    } else {
        if (found_real_uuid.empty()) {
             std::cout << "No GPU UUID found in memory." << std::endl;
        }
    }

    std::cout << "\n--- You can now start the game! ---" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(30));

    return true;
}
