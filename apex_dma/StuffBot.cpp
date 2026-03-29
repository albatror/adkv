#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <random>
#include <cfloat>
#include "offsets.h"
#include "Weapon.h"
#include "prediction.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uintptr_t aimentity;
extern float smooth;
extern bool aim_no_recoil;
extern bool aimassist;
extern bool aimassist_aiming;
extern float aimassist_fov;
extern float aimassist_max_dist;
extern bool aimassist_auto_shoot;
extern float aimassist_smooth;
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern float aim_dist;
extern float DDS;
extern float min_max_fov;
extern float max_max_fov;
extern bool firing_range;
extern bool is_aimentity_visible;
bool stuff_t = false;


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

        // AimAssist logic
        static uint64_t last_aim_target = 0;
        static int last_bone_id = 2; // Default to UpperChest/Neck area

        if (aimassist && aimassist_aiming)
        {
            if (aimentity != 0)
            {
                Entity Target = getEntity(aimentity);
                if (Target.isAlive() && (!Target.isKnocked() || firing_range))
                {
                    float dist = LPlayer.getPosition().DistTo(Target.getPosition());
                    if (dist <= aimassist_max_dist)
                    {
                        // Bone selection hysteresis
                        if (aimentity != last_aim_target)
                        {
                            last_aim_target = aimentity;
                            // Select bone closest to crosshair
                            float best_fov = FLT_MAX;
                            QAngle CurrentAngles = LPlayer.GetViewAngles();
                            Vector CamPos = LPlayer.GetCamPos();

                            // Iterate over key bones: 0:Head, 2:UpperChest, 3:LowerChest, 5:Stomach? (let's use 0 to 8 as common hitboxes)
                            for (int i = 0; i < 8; i++) {
                                Vector BonePos = Target.getBonePositionByHitbox(i);
                                if (BonePos.IsZero()) continue;
                                float fov = Math::GetFov(CurrentAngles, Math::CalcAngle(CamPos, BonePos));
                                if (fov < best_fov) {
                                    best_fov = fov;
                                    last_bone_id = i;
                                }
                            }
                        }

                        Vector TargetPos = Target.getBonePositionByHitbox(last_bone_id);
                        if (!TargetPos.IsZero() && (is_aimentity_visible || firing_range))
                        {
                            QAngle CurrentAngles = LPlayer.GetViewAngles();
                            QAngle TargetAngles = Math::CalcAngle(LPlayer.GetCamPos(), TargetPos);

                            // 1. Calculate FOV before compensation
                            float fov = Math::GetFov(CurrentAngles, TargetAngles);

                            if (fov <= aimassist_fov)
                            {
                            // 2. Refined calculation - always account for sway/recoil to prevent engine fighting
                            QAngle SwayAngles = LPlayer.GetSwayAngles();
                            QAngle Recoil = LPlayer.GetRecoil();
                            TargetAngles -= (SwayAngles - CurrentAngles);
                            TargetAngles -= Recoil;

                                Math::NormalizeAngles(TargetAngles);

                                // 3. Smoothing with deadzone
                                QAngle Delta = TargetAngles - CurrentAngles;
                                Math::NormalizeAngles(Delta);

                                float delta_len = sqrt(Delta.x * Delta.x + Delta.y * Delta.y);
                                if (delta_len > 0.01f && aimassist_smooth >= 1.0f) // Deadzone for stability
                                {
                                    // Smooth out movement
                                    QAngle SmoothedDelta = Delta / aimassist_smooth;
                                    QAngle NewAngles = CurrentAngles + SmoothedDelta;
                                    Math::NormalizeAngles(NewAngles);

                                    // Ensure we don't set invalid angles
                                    if (!isnan(NewAngles.x) && !isnan(NewAngles.y)) {
                                        LPlayer.SetViewAngles(NewAngles);
                                    }
                                }
                            }
                        }
                    }
                }
            }
            else
            {
                last_aim_target = 0;
            }
        }

        // Triggerbot logic - optimized to run in StuffBot thread
        if ((triggerbot && triggerbot_aiming) || (aimassist && aimassist_aiming && aimassist_auto_shoot))
        {
            if (aimentity != 0 && (is_aimentity_visible || firing_range))
            {
                // First check crosshair timestamp - fast read
                float now_crosshair_target_time = 0;
                if (apex_mem.Read<float>(aimentity + OFFSET_CROSSHAIR_LAST, now_crosshair_target_time))
                {
                    static std::unordered_map<uint64_t, float> last_crosshair_times;
                    if (now_crosshair_target_time > last_crosshair_times[aimentity])
                    {
                        Entity Target = getEntity(aimentity);
                        Vector HeadPos = Target.getBonePositionByHitbox(0);
                        if (!HeadPos.IsZero())
                        {
                            QAngle CurrentAngles = LPlayer.GetViewAngles();
                            float fov = Math::GetFov(CurrentAngles, Math::CalcAngle(LPlayer.GetCamPos(), HeadPos));

                            if (fov <= triggerbot_fov)
                            {
                                char weaponModel[256] = { 0 };
                                LPlayer.getWeaponModelName(weaponModel, 256);
                                std::string weaponName = get_weapon_name_by_model(weaponModel);
                                printf("[TRIGGERBOT] Shooting at aimentity with weapon %s (fov: %.2f)\n", weaponName.c_str(), fov);
                                TriggerBotRun();
                            }
                        }
                        last_crosshair_times[aimentity] = now_crosshair_target_time;
                    }
                }
            }
        }

    }
}
