#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include <thread>
#include <unordered_map>
#include <cmath>
#include "Game.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uint64_t c_Base;
extern bool stuff_t;
extern uintptr_t aimentity;
extern bool is_aimentity_visible;
extern bool firing_range;

extern bool flickbot;
extern bool flickbot_aiming;
extern float flickbot_fov;
extern float flickbot_smooth;

extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;

extern int aim;
extern bool aiming;

bool IsInCrossHair(Entity& target)
{
    static std::unordered_map<uint64_t, float> lastTimes;  // per target

    float now_crosshair_target_time = target.lastCrossHairTime();

    if (std::isnan(now_crosshair_target_time)) {
        return false;
    }

    bool triggered = false;

    float last_time = lastTimes[target.ptr];  // 0 if first time seen

    if (now_crosshair_target_time > last_time) {
        triggered = true;
    }

    lastTimes[target.ptr] = now_crosshair_target_time;  // update for next check

    return triggered;
}

void StuffbotLoop()
{
	stuff_t = true;
	while (stuff_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(5));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(5));

			if (aimentity == 0) continue;

			// Priority check: if main aimbot is active and aiming, yield
			if (aim > 0 && aiming) continue;

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
			if (LocalPlayer == 0) continue;
			Entity LPlayer = getEntity(LocalPlayer);

			Entity Target = getEntity(aimentity);
			if (!Target.isAlive() || (Target.isKnocked() && !firing_range) || !is_aimentity_visible)
				continue;

			float fov = CalculateFov(LPlayer, Target);

			// Flickbot logic
			if (flickbot && flickbot_aiming)
			{
				if (fov <= flickbot_fov)
				{
					QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, flickbot_fov, flickbot_smooth);
					if (Angles.x != 0 || Angles.y != 0)
					{
						QAngle current_angles = LPlayer.GetViewAngles();
						double angular_delta = Math::GetFov(current_angles, Angles);
						if (angular_delta > 0.001)
						{
							LPlayer.SetViewAngles(Angles);
						}
					}
				}
			}

			// Triggerbot logic
			if (triggerbot && triggerbot_aiming)
			{
				if (fov <= triggerbot_fov)
				{
					if (IsInCrossHair(Target))
					{
						apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
					}
				}
			}
		}
	}
	stuff_t = false;
}
