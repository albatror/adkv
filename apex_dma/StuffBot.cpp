#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include "offsets.h"
#include "Weapon.h"
#include "prediction.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uintptr_t aimentity;
extern float smooth;
extern bool flickbot;
extern bool flickbot_aiming;
extern float flickbot_fov;
extern float flickbot_smooth;
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern bool triggerbot_use_weapon_list;
extern float triggerbot_speed;
extern float triggerbot_gravity;
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

bool isAllowedWeapon(int weaponId, int zoomElapsedMs) {
    // Special long zoom delay for Kraber and Sentinel
    if (weaponId == WeaponIDs::KRABER || weaponId == WeaponIDs::SENTINEL) {
        if (zoomElapsedMs < 950)
            return false; // Not ready to shoot yet
    } else {
        // All other weapons need some delay after zoom
        if (zoomElapsedMs < 600)
            return false; // Not ready yet
    }

    // List of allowed weapons
    return (
        weaponId == WeaponIDs::KRABER ||
        weaponId == WeaponIDs::WINGMAN ||
        weaponId == WeaponIDs::LONGBOW ||
        weaponId == WeaponIDs::SENTINEL ||
        weaponId == WeaponIDs::G7_SCOUT ||
        weaponId == WeaponIDs::HEMLOCK ||
        weaponId == WeaponIDs::REPEATER_3030 ||
        weaponId == WeaponIDs::TRIPLE_TAKE ||
        weaponId == WeaponIDs::BOCEK ||
        weaponId == WeaponIDs::KNIFE ||
        weaponId == WeaponIDs::P2020 ||
        weaponId == WeaponIDs::MOZAMBIQUE ||
        weaponId == WeaponIDs::EVA8 ||
        weaponId == WeaponIDs::PEACEKEEPER ||
        weaponId == WeaponIDs::MASTIFF ||
        weaponId == WeaponIDs::NEMESIS ||
        weaponId == WeaponIDs::RE45 ||
        weaponId == WeaponIDs::ALTERNATOR ||
        weaponId == WeaponIDs::R99 ||
        weaponId == WeaponIDs::R301 ||
        weaponId == WeaponIDs::SPITFIRE ||
        weaponId == WeaponIDs::FLATLINE ||
        weaponId == WeaponIDs::PROWLER ||
        weaponId == WeaponIDs::RAMPAGE ||
        weaponId == WeaponIDs::CAR ||
        weaponId == WeaponIDs::HAVOC ||
        weaponId == WeaponIDs::DEVOTION ||
        weaponId == WeaponIDs::LSTAR ||
        weaponId == WeaponIDs::VOLT ||
        weaponId == WeaponIDs::CHARGE_RIFLE
    );
}

void StuffBotLoop()
{
    stuff_t = true;
    // Zoom tracking
    static auto zoomStartTime = std::chrono::steady_clock::now();
    static bool wasZooming = false;

    while (stuff_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (g_Base == 0) continue;

        uint64_t LocalPlayer = 0;
        apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
        if (LocalPlayer == 0) continue;
        Entity LPlayer = getEntity(LocalPlayer);

        // Update zoom timing
        bool isZooming = LPlayer.isZooming();
        auto now = std::chrono::steady_clock::now();

        if (isZooming && !wasZooming) {
            zoomStartTime = now;  // just started zooming
        }

        int zoomElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - zoomStartTime).count();
        wasZooming = isZooming;

        // Triggerbot logic
        if (triggerbot && triggerbot_aiming && aimentity != 0)
        {
            Entity Target = getEntity(aimentity);

            // Save original bone and aim_no_recoil settings if needed, but here we just override for the call
            extern int bone;
            int old_bone = bone;
            bone = 0; // Headshot focus

            // Custom prediction if provided
            WeaponXEntity curweap = WeaponXEntity();
            curweap.update(LPlayer.ptr);
            float speed = triggerbot_speed > 0 ? triggerbot_speed : curweap.get_projectile_speed();
            float gravity = triggerbot_gravity > 0 ? triggerbot_gravity : curweap.get_projectile_gravity();

            // Calculate angles with headshot focus and custom prediction
            // Note: We'd need to modify CalculateBestBoneAim or implement a specific version here
            // For now, let's use a simplified headshot aim
            Vector LocalCamera = LPlayer.GetCamPos();
            Vector TargetHead = Target.getBonePositionByHitbox(0);

            if (speed > 1.0f) {
                PredictCtx Ctx;
                Ctx.StartPos = LocalCamera;
                Ctx.TargetPos = TargetHead;
                Ctx.BulletSpeed = speed;
                Ctx.BulletGravity = gravity;
                Ctx.TargetVel = Target.getAbsVelocity();

                if (BulletPredict(Ctx)) {
                    QAngle aim_angles = { -RAD2DEG(Ctx.AimAngles.x), RAD2DEG(Ctx.AimAngles.y), 0.f };

                    // Apply recoil compensation if enabled
                    extern bool aim_no_recoil;
                    if (aim_no_recoil) {
                        aim_angles -= LPlayer.GetSwayAngles() - LPlayer.GetViewAngles();
                    }
                    Math::NormalizeAngles(aim_angles);

                    // Smoothing
                    QAngle view_angles = LPlayer.GetViewAngles();
                    QAngle delta = aim_angles - view_angles;
                    Math::NormalizeAngles(delta);
                    LPlayer.SetViewAngles(view_angles + delta / smooth);
                }
            } else {
                 QAngle aim_angles = Math::CalcAngle(LocalCamera, TargetHead);
                 LPlayer.SetViewAngles(aim_angles);
            }

            bone = old_bone; // Restore original bone

            bool shoot = false;
            if (triggerbot_use_weapon_list) {
                int weaponId = LPlayer.getCurrentWeaponId();
                if (isAllowedWeapon(weaponId, zoomElapsedMs)) {
                    if (IsInCrossHair(Target)) shoot = true;
                }
            } else {
                if (IsInCrossHair(Target)) shoot = true;
            }

            if (shoot) {
                int weaponId = LPlayer.getCurrentWeaponId();
                printf("[TRIGGERBOT] Shooting with %s\n", get_weapon_name(weaponId).c_str());
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
