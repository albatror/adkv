#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <random>
#include <cfloat>
#include "offsets.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern float aim_dist;
extern bool firing_range;
bool stuff_t = false;


void TriggerBotRun(bool shoot = false)
{
    static std::chrono::steady_clock::time_point trigger_release_time;
    static bool trigger_active = false;

    auto now = std::chrono::steady_clock::now();

    if (shoot && !trigger_active) {
        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
        trigger_release_time = now + std::chrono::milliseconds(60);
        trigger_active = true;
    } else if (trigger_active && now >= trigger_release_time) {
        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
        trigger_active = false;
    }
}

void StuffBotLoop()
{
    stuff_t = true;
    static std::chrono::steady_clock::time_point trigger_again_time;

    while (stuff_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (g_Base == 0) continue;

        uint64_t LocalPlayer = 0;
        apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
        if (LocalPlayer == 0) continue;
        Entity LPlayer = getEntity(LocalPlayer);

        // Always run TriggerBotRun to handle release if active
        TriggerBotRun(false);

        // Triggerbot logic
        if (triggerbot && triggerbot_aiming)
        {
            auto now = std::chrono::steady_clock::now();
            if (now < trigger_again_time) continue;

            uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
            int ent_count = firing_range ? 10000 : 1000;
            static std::unordered_map<uint64_t, float> last_crosshair_times;

            for (int i = 0; i < ent_count; i++) {
                uint64_t centity = 0;
                if (!apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity) || centity == 0 || centity == LocalPlayer) continue;

                // First check crosshair timestamp - fast read
                float now_crosshair_target_time = 0;
                if (!apex_mem.Read<float>(centity + OFFSET_CROSSHAIR_LAST, now_crosshair_target_time)) continue;

                if (last_crosshair_times.find(centity) == last_crosshair_times.end()) {
                    last_crosshair_times[centity] = now_crosshair_target_time;
                    continue;
                }

                if (now_crosshair_target_time > last_crosshair_times[centity]) {
                    // Possible target detected by crosshair, now perform detailed checks
                    int health = 0;
                    apex_mem.Read<int>(centity + OFFSET_HEALTH, health);
                    if (health <= 0) {
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        continue;
                    }

                    int team = 0;
                    apex_mem.Read<int>(centity + OFFSET_TEAM, team);
                    if (team == LPlayer.getTeamId() && !firing_range) {
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        continue;
                    }

                    // Check FOV using head bone for better accuracy
                    Entity Target = getEntity(centity);
                    Vector HeadPos = Target.getBonePositionByHitbox(0);
                    if (HeadPos.IsZero()) {
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        continue;
                    }

                    float fov = Math::GetFov(LPlayer.GetViewAngles(), Math::CalcAngle(LPlayer.GetCamPos(), HeadPos));

                    bool can_shoot = false;
                    float dist = LPlayer.getPosition().DistTo(Target.getPosition());

                    if (aim_dist > 0.0f && dist > aim_dist) {
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        continue;
                    }

                    if (fov <= triggerbot_fov) {
                        can_shoot = true;
                    }

                    if (can_shoot) {
                        TriggerBotRun(true);
                        trigger_again_time = now + std::chrono::milliseconds(80);
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        break;
                    }
                } else {
                    last_crosshair_times[centity] = now_crosshair_target_time;
                }
            }
        }
    }
}
