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
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern bool silent_aim;
extern float silent_aim_fov;
extern float aim_dist;
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

        // Triggerbot + Silent Aim logic
        if (triggerbot_aiming && (triggerbot || silent_aim))
        {
            uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
            int ent_count = firing_range ? 10000 : 100;
            static std::unordered_map<uint64_t, float> last_crosshair_times;

            uint64_t silent_target = 0;
            float best_silent_fov = 999.0f;
            QAngle silent_angles = { 0, 0, 0 };

            for (int i = 0; i < ent_count; i++) {
                uint64_t centity = 0;
                if (!apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity) || centity == 0 || centity == LocalPlayer) continue;

                if (silent_aim) {
                    int health = 0;
                    apex_mem.Read<int>(centity + OFFSET_HEALTH, health);
                    if (health <= 0) continue;

                    int team = 0;
                    apex_mem.Read<int>(centity + OFFSET_TEAM, team);
                    if (team == LPlayer.getTeamId() && !firing_range) continue;

                    if (firing_range) {
                        char class_name[33] = {};
                        get_class_name(centity, class_name);
                        if (strncmp(class_name, "CAI_BaseNPC", 11) != 0) continue;
                    }

                    Entity Target = getEntity(centity);
                    Vector HeadPos = Target.getBonePositionByHitbox(0);
                    if (HeadPos.IsZero()) continue;

                    float dist = LPlayer.getPosition().DistTo(Target.getPosition());
                    if (aim_dist > 0.0f && dist > aim_dist) continue;

                    QAngle TargetAngles = Math::CalcAngle(LPlayer.GetCamPos(), HeadPos);

                    WeaponXEntity curweap = WeaponXEntity();
                    curweap.update(LocalPlayer);
                    float BulletSpeed = curweap.get_projectile_speed();
                    float BulletGrav = curweap.get_projectile_gravity();

                    if (BulletSpeed > 1.f) {
                        PredictCtx Ctx;
                        Ctx.StartPos = LPlayer.GetCamPos();
                        Ctx.TargetPos = HeadPos;
                        Ctx.BulletSpeed = BulletSpeed - (BulletSpeed * 0.08f);
                        Ctx.BulletGravity = BulletGrav + (BulletGrav * 0.05f);
                        Ctx.TargetVel = Target.getAbsVelocity();
                        if (BulletPredict(Ctx)) {
                            TargetAngles = QAngle{ Ctx.AimAngles.x, Ctx.AimAngles.y, 0.f };
                        }
                    }

                    float fov = Math::CalcFOV(LPlayer.GetViewAngles(), TargetAngles);
                    if (fov <= silent_aim_fov && fov < best_silent_fov) {
                        best_silent_fov = fov;
                        silent_target = centity;
                        silent_angles = TargetAngles;
                    }
                }

                if (triggerbot && !silent_aim) {
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

                        if (can_shoot) {
                            TriggerBotRun();
                            last_crosshair_times[centity] = now_crosshair_target_time;
                            break;
                        }
                    }
                    last_crosshair_times[centity] = now_crosshair_target_time;
                }
            }

            if (silent_aim && silent_target != 0) {
                QAngle OldAngles = LPlayer.GetViewAngles();
                Math::NormalizeAngles(silent_angles);
                LPlayer.SetViewAngles(silent_angles);
                if (triggerbot) {
                    TriggerBotRun();
                }
                // Silent correction: Write back old angles in the same frame/as soon as possible
                // This might flick slightly if not handled by UserCmd manipulation,
                // but for external DMA this is the standard approach for "silent" behavior.
                LPlayer.SetViewAngles(OldAngles);
            }
        }
    }
}
