#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include "offsets.h"
#include "Weapon.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uintptr_t aimentity;
extern float smooth;
extern bool flickbot;
extern bool flickbot_aiming;
extern float flickbot_fov;
extern float flickbot_smooth;
extern bool flickbot_auto_shoot;
extern int flickbot_auto_shoot_delay;
extern bool flickbot_flickback;
extern int flickbot_flickback_delay;
extern int flickbot_delay;
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern bool triggerbot_use_weapon_list;
extern bool firing_range;
extern bool is_aimentity_visible;
bool stuff_t = false;


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
        if (triggerbot && triggerbot_aiming)
        {
            uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
            for (int i = 0; i < 100; i++) {
                uint64_t centity = 0;
                if (!apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity) || centity == 0 || centity == LocalPlayer) continue;

                // Targeted reads to avoid getEntity overhead
                int health = 0;
                apex_mem.Read<int>(centity + OFFSET_HEALTH, health);
                if (health <= 0) continue;

                int team = 0;
                apex_mem.Read<int>(centity + OFFSET_TEAM, team);
                if (team == LPlayer.getTeamId() && !firing_range) continue;

                // Dummy check if firing range
                if (firing_range) {
                    char class_name[33] = {};
                    get_class_name(centity, class_name);
                    if (strncmp(class_name, "CAI_BaseNPC", 11) != 0) continue;
                }

                // Check FOV
                Vector EntityPosition;
                apex_mem.Read<Vector>(centity + OFFSET_ORIGIN, EntityPosition);
                float fov = Math::GetFov(LPlayer.GetViewAngles(), Math::CalcAngle(LPlayer.GetCamPos(), EntityPosition));
                if (fov > triggerbot_fov) continue;

                // Finally check crosshair
                float now_crosshair_target_time = 0;
                apex_mem.Read<float>(centity + OFFSET_CROSSHAIR_LAST, now_crosshair_target_time);

                static std::unordered_map<uint64_t, float> last_crosshair_times;
                if (last_crosshair_times.find(centity) == last_crosshair_times.end()) {
                    last_crosshair_times[centity] = now_crosshair_target_time;
                    continue;
                }

                if (now_crosshair_target_time > last_crosshair_times[centity]) {
                    int weaponId = LPlayer.getCurrentWeaponId();
                    printf("[TRIGGERBOT] Shooting at entity %d with weapon %s\n", i, get_weapon_name(weaponId).c_str());
                    TriggerBotRun();
                    last_crosshair_times[centity] = now_crosshair_target_time;
                    break;
                }
                last_crosshair_times[centity] = now_crosshair_target_time;
            }
        }

        // Flickbot logic
        static auto lastFlickTime = std::chrono::steady_clock::now();
        if (flickbot && flickbot_aiming && aimentity != 0)
        {
            auto now_flick = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(now_flick - lastFlickTime).count() >= flickbot_delay)
            {
                int weaponId = LPlayer.getCurrentWeaponId();
                if (isAllowedWeapon(weaponId, zoomElapsedMs))
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
                                if (flickbot_auto_shoot)
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(flickbot_auto_shoot_delay));
                                    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
                                    std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
                                }
                                if (flickbot_flickback)
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(flickbot_flickback_delay));
                                    LPlayer.SetViewAngles(old_angles);
                                }
                                lastFlickTime = std::chrono::steady_clock::now();
                            }
                        }
                    }
                }
            }
        }
    }
}
