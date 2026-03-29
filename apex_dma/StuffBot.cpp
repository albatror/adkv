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
            if (aimentity != 0 && is_aimentity_visible)
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
                        if (!TargetPos.IsZero())
                        {
                            QAngle CurrentAngles = LPlayer.GetViewAngles();
                            QAngle TargetAngles = Math::CalcAngle(LPlayer.GetCamPos(), TargetPos);

                            // Refined pitch calculation - account for sway/recoil if needed
                            QAngle SwayAngles = LPlayer.GetSwayAngles();
                            if (aim_no_recoil)
                                TargetAngles -= (SwayAngles - CurrentAngles);

                            Math::NormalizeAngles(TargetAngles);

                            float fov = Math::GetFov(CurrentAngles, TargetAngles);
                            if (fov <= aimassist_fov)
                            {
                                // Smoothing with deadzone
                                QAngle Delta = TargetAngles - CurrentAngles;
                                Math::NormalizeAngles(Delta);

                                float delta_len = sqrt(Delta.x * Delta.x + Delta.y * Delta.y);
                                if (delta_len > 0.005f) // Deadzone to eliminate micro-shaking
                                {
                                    // Smooth out movement using dynamic smoothing based on distance to target
                                    float current_smoothing = aimassist_smooth;
                                    if (delta_len < 1.0f) {
                                        current_smoothing *= (1.0f + (1.0f - delta_len) * 2.0f); // Increase smoothing near center
                                    }

                                    QAngle SmoothedDelta = Delta / current_smoothing;
                                    QAngle NewAngles = CurrentAngles + SmoothedDelta;
                                    Math::NormalizeAngles(NewAngles);
                                    LPlayer.SetViewAngles(NewAngles);
                                }

                                // Auto-shoot integration
                                if (aimassist_auto_shoot)
                                {
                                    // Reuse triggerbot firing logic if crosshair is close enough
                                    if (delta_len < 0.5f) // Tight threshold for auto-shoot
                                    {
                                        TriggerBotRun();
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

        // Triggerbot logic
        if (triggerbot && triggerbot_aiming)
        {
            uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
            int ent_count = firing_range ? 10000 : 100;
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

                    // Dummy check if firing range
                    if (firing_range) {
                        char class_name[33] = {};
                        get_class_name(centity, class_name);
                        if (strncmp(class_name, "CAI_BaseNPC", 11) != 0) {
                            last_crosshair_times[centity] = now_crosshair_target_time;
                            continue;
                        }
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

                    if (dist < DDS) {
                        float distRatio = (DDS > 0.0f) ? (dist / DDS) : 0.0f;
                        float distanceFactor = 1.0f - distRatio;
                        float easedDistanceFactor = Math::SmoothStep(0.0f, 1.0f, distanceFactor);
                        float fovDiff = max_max_fov - min_max_fov;
                        float current_max_fov = min_max_fov + (fovDiff * easedDistanceFactor);

                        if (fov <= current_max_fov) {
                            can_shoot = true;
                        }
                    } else {
                        // Prediction for targets outside DDS
                        if (fov <= triggerbot_fov) {
                            WeaponXEntity curweap = WeaponXEntity();
                            curweap.update(LPlayer.ptr);
                            float BulletSpeed = curweap.get_projectile_speed();
                            float BulletGrav = curweap.get_projectile_gravity();

                            if (BulletSpeed > 1.f) {
                                PredictCtx Ctx;
                                Ctx.StartPos = LPlayer.GetCamPos();
                                Ctx.TargetPos = HeadPos;
                                // Scale bullet speed and gravity for prediction offset
                                Ctx.BulletSpeed = BulletSpeed - (BulletSpeed * 0.08f);
                                Ctx.BulletGravity = BulletGrav + (BulletGrav * 0.05f);
                                Ctx.TargetVel = Target.getAbsVelocity();

                                if (BulletPredict(Ctx)) {
                                    QAngle PredictedAngles = QAngle{ Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f };
                                    float predicted_fov = Math::GetFov(LPlayer.GetViewAngles(), PredictedAngles);
                                    if (predicted_fov <= triggerbot_fov) {
                                        can_shoot = true;
                                    }
                                }
                            } else {
                                can_shoot = true;
                            }
                        }
                    }

                    if (can_shoot) {
                        char weaponModel[256] = { 0 };
                        LPlayer.getWeaponModelName(weaponModel, 256);
                        std::string weaponName = get_weapon_name_by_model(weaponModel);
                        if (weaponName == "Unknown" || weaponName == "unknown") {
                            int weaponId = LPlayer.getCurrentWeaponId();
                            weaponName = get_weapon_name(weaponId);
                        }
                        printf("[TRIGGERBOT] Shooting at entity %d (dist: %.2f, fov: %.2f) with weapon %s\n", i, dist / 40.0f, fov, weaponName.c_str());
                        TriggerBotRun();
                        last_crosshair_times[centity] = now_crosshair_target_time;
                        break;
                    }
                }
                last_crosshair_times[centity] = now_crosshair_target_time;
            }
        }

    }
}
