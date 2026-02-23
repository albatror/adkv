#pragma once
#include <string>
#include <vector>
#include <Windows.h>

extern char real_gpu_uuid[256];
extern char spoofed_gpu_uuid[256];
extern bool disrupt_wmi;
extern bool gpu_spoofed;

void IdentifyAndSpoofGPU();
void RestoreGPU();
void DisruptWMI();
bool IsGPUAlreadySpoofed();
