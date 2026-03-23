#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <random>
#include "offsets.h"
#include "Weapon.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uintptr_t aimentity;
extern float smooth;
extern bool flickbot;
extern bool flickbot_aiming;
extern float flickbot_fov;
extern float flickbot_max_dist;
extern bool flickbot_auto_shoot;
extern int flickbot_auto_shoot_delay;
extern bool flickbot_flickback;
extern int flickbot_flickback_delay;
extern int flickbot_delay;
extern bool flickbot_use_list;
extern uint64_t flickbot_weapons[4];
extern bool triggerbot;
extern bool triggerbot_aiming;
extern float triggerbot_fov;
extern bool triggerbot_use_list;
extern uint64_t triggerbot_weapons[4];
extern bool firing_range;
extern bool is_aimentity_visible;
bool stuff_t = false;


void TriggerBotRun()
{
    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
    std::this_thread::sleep_for(std::chrono::milliseconds(60));
    apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
}

bool isWeaponAllowed(int weaponId, uint64_t* allowedMask) {
    if (weaponId < 0 || weaponId >= 256) return true; // Default allow if unknown or too high ID
    int q = weaponId / 64;
    int r = weaponId % 64;
    return (allowedMask[q] & (1ULL << r)) != 0;
}

void StuffBotLoop()
{
    stuff_t = true;
    auto zoomStartTime = std::chrono::steady_clock::now();
    bool wasZooming = false;

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
            zoomStartTime = now;
        }
        int zoomElapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(now - zoomStartTime).count();
        wasZooming = isZooming;

        // Triggerbot logic
        if (triggerbot && triggerbot_aiming)
        {
            uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
            for (int i = 0; i < 256; i++) {
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

                    if (triggerbot_use_list) {
                        if (!isWeaponAllowed(weaponId, triggerbot_weapons)) {
                            last_crosshair_times[centity] = now_crosshair_target_time;
                            continue;
                        }

                        // Zoom delay logic
                        if (weaponId == WeaponIDs::KRABER || weaponId == WeaponIDs::SENTINEL) {
                            if (zoomElapsedMs < 950) {
                                last_crosshair_times[centity] = now_crosshair_target_time;
                                continue;
                            }
                        } else {
                            if (zoomElapsedMs < 500) {
                                last_crosshair_times[centity] = now_crosshair_target_time;
                                continue;
                            }
                        }

                        // Force zoom for triggerbot if list is used
                        if (!isZooming) {
                            last_crosshair_times[centity] = now_crosshair_target_time;
                            continue;
                        }
                    }

                    char weaponModel[256] = { 0 };
                    LPlayer.getWeaponModelName(weaponModel, 256);
                    std::string weaponName = get_weapon_name_by_model(weaponModel);
                    if (weaponName == "Unknown") {
                        weaponName = get_weapon_name(weaponId);
                    }
                    printf("[TRIGGERBOT] Shooting at entity %d with weapon %s (ID: %d)\n", i, weaponName.c_str(), weaponId);
                    TriggerBotRun();
                    last_crosshair_times[centity] = now_crosshair_target_time;
                    break;
                }
                last_crosshair_times[centity] = now_crosshair_target_time;
            }
        }

        // Flickbot logic
        static auto lastFlickTime = std::chrono::steady_clock::now() - std::chrono::seconds(10);
        static QAngle manual_angles = { 0, 0, 0 };
        static bool is_flicking = false;

        if (flickbot && flickbot_aiming && aimentity != 0)
        {
            int weaponId = LPlayer.getCurrentWeaponId();
            if (flickbot_use_list && !isWeaponAllowed(weaponId, flickbot_weapons)) {
                is_flicking = false;
            } else {
                char weaponModel[256] = { 0 };
                LPlayer.getWeaponModelName(weaponModel, 256);
                std::string weaponName = get_weapon_name_by_model(weaponModel);

                // Filter weapons (don't flick with melee or unknown items)
                bool isMelee = (weaponName == "Fists" || weaponName == "Throwing Knife" || weaponName == "Unknown");

                if (!isMelee)
            {
                Entity Target = getEntity(aimentity);
                if (Target.isAlive() && (!Target.isKnocked() || firing_range) && is_aimentity_visible)
                {
                    float distance = LPlayer.getPosition().DistTo(Target.getPosition());

                    // Adaptive system based on distance
                    float current_flick_smooth = smooth;
                    float current_flick_fov = flickbot_fov;
                    int current_flick_delay = flickbot_delay;
                    int current_shoot_delay = flickbot_auto_shoot_delay;
                    int current_flickback_delay = flickbot_flickback_delay;

                    if (distance < flickbot_max_dist && flickbot_max_dist > 0.0f)
                    {
                        float scale = 1.0f - (distance / flickbot_max_dist);
                        // Increase speed (decrease smooth) for close targets
                        current_flick_smooth /= (1.0f + scale * 3.0f);
                        // Increase FOV for close targets to make acquisition easier
                        current_flick_fov *= (1.0f + scale * 2.0f);
                        // Decrease delays for faster reaction at close range
                        current_flick_delay = (int)(current_flick_delay / (1.0f + scale * 2.0f));
                        current_shoot_delay = (int)(current_shoot_delay / (1.0f + scale * 1.5f));
                        current_flickback_delay = (int)(current_flickback_delay / (1.0f + scale * 1.5f));
                    }

                    float fov = CalculateFov(LPlayer, Target);
                    if (fov <= current_flick_fov)
                    {
                        if (!is_flicking) {
                            manual_angles = LPlayer.GetViewAngles();
                            is_flicking = true;
                        }

                        // 1. Read current angle (already done in manual_angles capture or in LPlayer.GetViewAngles())
                        // 2. Calculate target angle
                        // 3. delta = difference
                        // 4. Calculate FOV (already done above)
                        // 5. If FOV OK:
                        QAngle aim_angles = CalculateBestBoneAim(LPlayer, aimentity, current_flick_fov, current_flick_smooth);
                        if (aim_angles.x != 0 || aim_angles.y != 0)
                        {
                            // -> apply flick (smooth)
                            LPlayer.SetViewAngles(aim_angles);

                            auto now = std::chrono::steady_clock::now();
                            if (std::chrono::duration_cast<std::chrono::milliseconds>(now - lastFlickTime).count() >= current_flick_delay)
                            {
                                // -> optionally shoot if autoshoot is selected by the user
                                if (flickbot_auto_shoot)
                                {
                                    // Zoom delay for flickbot autoshoot if list is used
                                    bool canShoot = true;
                                    if (flickbot_use_list) {
                                        if (weaponId == WeaponIDs::KRABER || weaponId == WeaponIDs::SENTINEL) {
                                            if (zoomElapsedMs < 950) canShoot = false;
                                        } else {
                                            if (zoomElapsedMs < 500) canShoot = false;
                                        }
                                        if (!isZooming) canShoot = false;
                                    }

                                    if (canShoot) {
                                        std::this_thread::sleep_for(std::chrono::milliseconds(current_shoot_delay));
                                        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
                                        std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                        apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
                                    }
                                }

                                // -> Optional: return to the initial angle if flickback is selected by the user
                                if (flickbot_flickback)
                                {
                                    std::this_thread::sleep_for(std::chrono::milliseconds(current_flickback_delay));
                                    LPlayer.SetViewAngles(manual_angles);
                                    is_flicking = false; // Reset to allow fresh manual angle capture
                                }

                                // Add random jitter
                                static std::default_random_engine engine(std::chrono::system_clock::now().time_since_epoch().count());
                                std::uniform_int_distribution<int> dist(0, 10);
                                lastFlickTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(dist(engine));
                            }
                        }
                    } else {
                        is_flicking = false;
                    }
                } else {
                    is_flicking = false;
                }
            }
        } else {
            is_flicking = false;
        }
    }
}
