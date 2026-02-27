#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <cmath>
#include "offsets.h"
#include "Weapon.h"

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
    static std::unordered_map<uint64_t, float> lastTimes;
    float now_crosshair_target_time = target.lastCrossHairTime();

    if (std::isnan(now_crosshair_target_time)) return false;

    float last_time = lastTimes[target.ptr];
    bool triggered = (now_crosshair_target_time > last_time) && (last_time != 0.0f);
    lastTimes[target.ptr] = now_crosshair_target_time;

    return triggered;
}

void TriggerBotRun()
{
    // Deprecated: Using non-blocking state machine in StuffBotLoop
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
        weaponId == WeaponIDs::HEMLOK ||
        weaponId == WeaponIDs::REPEATER_3030 ||
        weaponId == WeaponIDs::TRIPLE_TAKE ||
        weaponId == WeaponIDs::BOCEK ||
        weaponId == WeaponIDs::THROWING_KNIFE ||
        weaponId == WeaponIDs::P2020 ||
        weaponId == WeaponIDs::MOZAMBIQUE ||
        weaponId == WeaponIDs::EVA8 ||
        weaponId == WeaponIDs::PEACEKEEPER ||
        weaponId == WeaponIDs::MASTIFF ||
        weaponId == WeaponIDs::NEMESIS
    );
}

void StuffBotLoop()
{
    stuff_t = true;
    // Zoom tracking
    static auto zoomStartTime = std::chrono::steady_clock::now();
    static bool wasZooming = false;
    // Triggerbot state
    static bool isShooting = false;
    static auto shootStartTime = std::chrono::steady_clock::now();

    while (stuff_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (g_Base == 0) continue;

        uint64_t LocalPlayer = 0;
        apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
        if (LocalPlayer == 0) continue;
        Entity LPlayer = getEntity(LocalPlayer);

        auto now = std::chrono::steady_clock::now();

        // Update zoom timing
        bool isZooming = LPlayer.isZooming();
        if (isZooming && !wasZooming) {
            zoomStartTime = now;  // just started zooming
        }
        int zoomElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - zoomStartTime).count();
        wasZooming = isZooming;

        // Triggerbot firing state machine
        if (isShooting) {
            int elapsedShoot = std::chrono::duration_cast<std::chrono::milliseconds>(now - shootStartTime).count();
            if (elapsedShoot >= 60) {
                apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
                isShooting = false;
            }
        }

        // Triggerbot logic
        if (triggerbot && triggerbot_aiming && aimentity != 0 && !isShooting)
        {
            if (isZooming) {
                int weaponId = LPlayer.getCurrentWeaponId();
                if (isAllowedWeapon(weaponId, zoomElapsedMs)) {
                    Entity Target = getEntity(aimentity);
                    if (IsInCrossHair(Target))
                    {
                        printf("[TRIGGERBOT] Shooting with %s (ID: %d)\n", get_weapon_name(weaponId).c_str(), weaponId);
                        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
                        shootStartTime = now;
                        isShooting = true;
                    }
                }
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
