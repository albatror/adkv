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
extern int triggerbot_hitbox;
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

                    // Check FOV using selected hitboxes
                    Entity Target = getEntity(centity);
                    Vector LPlayerPos = LPlayer.getPosition();
                    Vector TargetPos = Target.getPosition();
                    float dist = LPlayerPos.DistTo(TargetPos);

                    if (aim_dist > 0.0f && dist > aim_dist) {
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        continue;
                    }

                    bool can_shoot = false;
                    Vector CamPos = LPlayer.GetCamPos();
                    QAngle ViewAngles = LPlayer.GetViewAngles();

                    if (triggerbot_hitbox == -1) // NONE mode: uses absOrigin.z + static value
                    {
                        Vector targetHitPos = TargetPos;
                        targetHitPos.z += 60.0f; // Approx chest height
                        float fov = Math::GetFov(ViewAngles, Math::CalcAngle(CamPos, targetHitPos));
                        if (fov <= triggerbot_fov) can_shoot = true;
                    }
                    else if (triggerbot_hitbox >= 0 && triggerbot_hitbox <= 100) // Static bone mode
                    {
                        Vector BonePos = Target.getBonePositionByHitbox(triggerbot_hitbox);
                        if (!BonePos.IsZero()) {
                            float fov = Math::GetFov(ViewAngles, Math::CalcAngle(CamPos, BonePos));
                            if (fov <= triggerbot_fov) can_shoot = true;
                        }
                    }
                    else // Multi-bone modes
                    {
                        static const int NEAR3_BONES[] = { 0, 1, 2 };
                        static const int NEAR6_BONES[] = { 0, 1, 2, 3, 4, 11 };
                        static const int NEAR12_BONES[] = { 0, 1, 2, 3, 4, 5, 8, 11, 12, 14, 15, 6 };

                        const int* pBones = nullptr;
                        int boneCount = 0;

                        if (triggerbot_hitbox == 101) { pBones = NEAR3_BONES; boneCount = 3; }
                        else if (triggerbot_hitbox == 102) { pBones = NEAR6_BONES; boneCount = 6; }
                        else if (triggerbot_hitbox == 103) { pBones = NEAR12_BONES; boneCount = 12; }
                        else if (triggerbot_hitbox == 104) { boneCount = 17; } // ALL

                        for (int b = 0; b < boneCount; b++) {
                            int boneID = (triggerbot_hitbox == 104) ? b : pBones[b];
                            Vector BonePos = Target.getBonePositionByHitbox(boneID);
                            if (BonePos.IsZero()) continue;
                            float fov = Math::GetFov(ViewAngles, Math::CalcAngle(CamPos, BonePos));
                            if (fov <= triggerbot_fov) {
                                can_shoot = true;
                                break;
                            }
                        }
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
