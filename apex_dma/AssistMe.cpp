#include "AssistMe.h"
#include <thread>
#include <chrono>
#include "Game.h"
#include "Math.h"

extern Memory apex_mem;
extern uint64_t g_Base;
extern uintptr_t aimentity;
extern bool assist_aim;
extern bool assist_aim_active;
extern float assist_aim_fov;
extern float assist_aim_dist;
extern bool is_aimentity_visible;

bool assist_t = false;

void AssistMeLoop()
{
    assist_t = true;

    while (assist_t)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (g_Base == 0) continue;

        if (assist_aim && assist_aim_active && aimentity != 0)
        {
            uint64_t LocalPlayerPtr = 0;
            apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayerPtr);
            if (LocalPlayerPtr == 0) continue;

            Entity Target = getEntity(aimentity);
            if (Target.isAlive() && is_aimentity_visible)
            {
                Entity LPlayer = getEntity(LocalPlayerPtr);
                float distance = LPlayer.getPosition().DistTo(Target.getPosition());

                // Automatic smooth calculation
                // Base smooth 100, increases with distance and FOV
                float fov = CalculateFov(LPlayer, Target);
                if (fov <= assist_aim_fov)
                {
                    float auto_smooth = 12.0f; // Lower base smooth for more responsiveness

                    // Adaptive scaling
                    if (distance < 100.0f * 40.0f) {
                        auto_smooth += (distance / 40.0f) * 0.1f;
                    } else {
                        auto_smooth += (distance / 40.0f) * 0.5f;
                    }

                    auto_smooth += fov * 2.0f;

                    QAngle aim_angles = CalculateBestBoneAim(LPlayer, aimentity, assist_aim_fov, 1.0f); // Use smooth 1.0 here to get the raw target angle
                    if (aim_angles.x != 0 || aim_angles.y != 0)
                    {
                        static auto last_log_time = std::chrono::steady_clock::now();
                        auto now = std::chrono::steady_clock::now();
                        if (std::chrono::duration_cast<std::chrono::seconds>(now - last_log_time).count() >= 5) {
                            printf("[ASSIST_AIM] Tracking target %lx, fov: %.2f, dist: %.2f, smooth: %.2f, angles: %.2f %.2f\n", aimentity, fov, distance/40.0f, auto_smooth, aim_angles.x, aim_angles.y);
                            last_log_time = now;
                        }

                        QAngle current_angles = LPlayer.GetViewAngles();
                        QAngle delta = aim_angles - current_angles;
                        Math::NormalizeAngles(delta);

                        QAngle smoothed = current_angles + delta / auto_smooth;
                        Math::NormalizeAngles(smoothed);

                        // Use direct write to ensure it hits the local entity properly
                        apex_mem.Write<SVector>(LPlayer.ptr + OFFSET_VIEWANGLES, SVector(smoothed));
                    }
                }
            }
        }
    }
}
