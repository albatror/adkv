#pragma once
#include <cstdint>

extern bool flickbot;
extern bool flickbot_aiming;
extern float flickbot_fov;
extern float flickbot_smooth;

extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;

extern bool stuff_t;

void FlickbotTriggerbotLoop();
void UpdateStuffVars(uint64_t add_addr);
