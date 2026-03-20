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
            Entity LPlayer = getEntity(LocalPlayerPtr);

            Entity Target = getEntity(aimentity);
            if (Target.isAlive() && is_aimentity_visible)
            {
                float distance = LPlayer.getPosition().DistTo(Target.getPosition());

                // Automatic smooth calculation
                // Base smooth 100, increases with distance and FOV
                float fov = CalculateFov(LPlayer, Target);
                if (fov <= assist_aim_fov)
                {
                    float auto_smooth = 100.0f;

                    // Increase smooth (slower) for further targets
                    auto_smooth += (distance / 40.0f) * 0.5f;

                    // Increase smooth for larger FOV diff
                    auto_smooth += fov * 5.0f;

                    QAngle aim_angles = CalculateBestBoneAim(LPlayer, aimentity, assist_aim_fov, auto_smooth);
                    if (aim_angles.x != 0 || aim_angles.y != 0)
                    {
                        LPlayer.SetViewAngles(aim_angles);
                    }
                }
            }
        }
    }
}
