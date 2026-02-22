#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include "offsets.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uintptr_t aimentity;
extern bool flickbot;
extern bool flickbot_aiming;
extern float flickbot_fov;
extern float flickbot_smooth;
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern bool firing_range;
extern bool is_aimentity_visible;
bool stuff_t = false;

bool IsInCrossHair(Entity &target)
{
    static uintptr_t last_t = 0;
    static float last_crosshair_target_time = -1.f;
    float now_crosshair_target_time = target.lastCrossHairTime();
    bool is_trigger = false;

    if (last_t == target.ptr)
    {
        if(last_crosshair_target_time != -1.f)
        {
            if(now_crosshair_target_time > last_crosshair_target_time)
            {
                is_trigger = true;
                last_crosshair_target_time = -1.f;
            }
            else
            {
                is_trigger = false;
                last_crosshair_target_time = now_crosshair_target_time;
            }
        }
        else
        {
            is_trigger = false;
            last_crosshair_target_time = now_crosshair_target_time;
        }
    }
    else
    {
        last_t = target.ptr;
        last_crosshair_target_time = -1.f;
    }
    return is_trigger;
}

void TriggerBotRun()
{
    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
}

void StuffBotLoop()
{
    stuff_t = true;
    while (stuff_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (g_Base == 0) continue;

        uint64_t LocalPlayer = 0;
        apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
        if (LocalPlayer == 0) continue;
        Entity LPlayer = getEntity(LocalPlayer);

        // Triggerbot logic
        if (triggerbot && triggerbot_aiming && aimentity != 0)
        {
            Entity Target = getEntity(aimentity);
            if (IsInCrossHair(Target))
            {
                TriggerBotRun();
            }
        }

        // Flickbot logic
        if (flickbot && flickbot_aiming && aimentity != 0)
        {
            Entity Target = getEntity(aimentity);
            if (Target.isAlive() && (!Target.isKnocked() || firing_range) && is_aimentity_visible)
            {
                float fov = CalculateFov(LPlayer, Target);
                if (fov <= flickbot_fov)
                {
                    QAngle old_angles = LPlayer.GetViewAngles();
                    QAngle aim_angles = CalculateBestBoneAim(LPlayer, aimentity, flickbot_fov, flickbot_smooth);
                    if (aim_angles.x != 0 || aim_angles.y != 0)
                    {
                        LPlayer.SetViewAngles(aim_angles);
                        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
                        std::this_thread::sleep_for(std::chrono::milliseconds(10));
                        LPlayer.SetViewAngles(old_angles);
                        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Cooldown
                    }
                }
            }
        }
    }
}
