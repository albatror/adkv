#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
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
extern bool rapidfire;
extern bool shooting;
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

bool TriggerBotRun(bool should_fire)
{
    static auto lastFireTime = std::chrono::steady_clock::now();
    static bool isFiring = false;
    auto now = std::chrono::steady_clock::now();

    if (should_fire && !isFiring) {
        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
        isFiring = true;
        lastFireTime = now;
    } else if (isFiring) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFireTime).count() > 20) {
            apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
            isFiring = false;
        }
    }
    return isFiring;
}

bool RapidFireRun(bool should_fire)
{
    static auto lastFireTime = std::chrono::steady_clock::now();
    static bool isFiring = false;
    auto now = std::chrono::steady_clock::now();

    if (should_fire && !isFiring) {
        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
        isFiring = true;
        lastFireTime = now;
    } else if (isFiring) {
        if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFireTime).count() > 20) {
            apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
            isFiring = false;
        }
    }
    return isFiring;
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
        bool trigger_active = false;
        if (triggerbot && triggerbot_aiming && aimentity != 0)
        {
            if (isZooming) {
                int weaponId = LPlayer.getCurrentWeaponId();
                if (isAllowedWeapon(weaponId, zoomElapsedMs)) {
                    Entity Target = getEntity(aimentity);
                    if (IsInCrossHair(Target))
                    {
                        printf("[TRIGGERBOT] Shooting with %s (ID: %d)\n", get_weapon_name(weaponId).c_str(), weaponId);
                        trigger_active = true;
                    }
                }
            }
        }
        TriggerBotRun(trigger_active);

        // Flickbot logic
        static auto lastFlickTime = std::chrono::steady_clock::now();
        if (flickbot && flickbot_aiming && aimentity != 0)
        {
            auto nowFlick = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(nowFlick - lastFlickTime).count() > 200)
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
                            std::this_thread::sleep_for(std::chrono::milliseconds(20)); // Keep sleep short or use state machine
                            apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);

                            // Zap-client like flickback
                            LPlayer.SetViewAngles(old_angles);
                            lastFlickTime = nowFlick;
                        }
                    }
                }
            }
        }

        // Rapidfire logic
        bool rapid_active = false;
        if (rapidfire && shooting)
        {
            int weaponId = LPlayer.getCurrentWeaponId();
            if (weaponId == WeaponIDs::P2020 ||
                weaponId == WeaponIDs::G7_SCOUT ||
                weaponId == WeaponIDs::HEMLOK ||
                weaponId == WeaponIDs::EVA8 ||
                weaponId == WeaponIDs::MOZAMBIQUE ||
                weaponId == WeaponIDs::WINGMAN ||
                weaponId == WeaponIDs::REPEATER_3030 ||
                weaponId == WeaponIDs::NEMESIS ||
                weaponId == WeaponIDs::FLATLINE || // Some people use it for Flatline too
                weaponId == WeaponIDs::R301)
            {
                rapid_active = true;
            }
        }
        RapidFireRun(rapid_active);
    }
}
