#pragma once
#include <Windows.h>
#include <string>

bool DisruptWMI();
void ApplyRegistrySpoofs();
std::string GetRealMachineGuid();
