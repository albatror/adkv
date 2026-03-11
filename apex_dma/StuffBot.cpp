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
            Entity Target = getEntity(aimentity);
            if (Target.isAlive() && (!Target.isKnocked() || firing_range) && is_aimentity_visible)
            {
                float distance = LPlayer.getPosition().DistTo(Target.getPosition());

                // Dynamic scaling based on distance (DDS style)
                float current_flick_fov = flickbot_fov;
                float current_flick_smooth = flickbot_smooth;
                int current_flick_delay = flickbot_delay;
                int current_shoot_delay = flickbot_auto_shoot_delay;
                int current_flickback_delay = flickbot_flickback_delay;

                if (distance < 2000.0f) // Close range scaling (approx < 50m)
                {
                    float scale = 1.0f - (distance / 2000.0f);
                    current_flick_fov *= (1.0f + scale * 2.0f); // Increase FOV up to 3x
                    current_flick_smooth /= (1.0f + scale * 3.0f); // Decrease smooth up to 4x
                    current_flick_delay = (int)(current_flick_delay / (1.0f + scale * 2.0f));
                    current_shoot_delay = (int)(current_shoot_delay / (1.0f + scale * 1.5f));
                    current_flickback_delay = (int)(current_flickback_delay / (1.0f + scale * 1.5f));
                }

                auto now_flick = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::milliseconds>(now_flick - lastFlickTime).count() >= current_flick_delay)
                {
                    float fov = CalculateFov(LPlayer, Target);
                    if (fov <= current_flick_fov)
                    {
                        QAngle old_angles = LPlayer.GetViewAngles();
                        QAngle aim_angles = CalculateBestBoneAim(LPlayer, aimentity, current_flick_fov, current_flick_smooth);
                        if (aim_angles.x != 0 || aim_angles.y != 0)
                        {
                            LPlayer.SetViewAngles(aim_angles);
                            if (flickbot_auto_shoot)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(current_shoot_delay));
                                apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
                                std::this_thread::sleep_for(std::chrono::milliseconds(50));
                                apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
                            }
                            if (flickbot_flickback)
                            {
                                std::this_thread::sleep_for(std::chrono::milliseconds(current_flickback_delay));
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
