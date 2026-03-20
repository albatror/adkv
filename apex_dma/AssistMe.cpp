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
extern float smooth;

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

            // Target existence and status check
            int health = 0;
            apex_mem.Read<int>(aimentity + OFFSET_HEALTH, health);
            if (health <= 0) continue;

            int lifeState = 0;
            apex_mem.Read<int>(aimentity + OFFSET_LIFE_STATE, lifeState);
            if (lifeState != 0) continue;

            if (!is_aimentity_visible) continue;

            // Retrieve only necessary data for LPlayer
            Vector LocalCamera;
            apex_mem.Read<Vector>(LocalPlayerPtr + OFFSET_CAMERAPOS, LocalCamera);

            QAngle currentAngles;
            apex_mem.Read<QAngle>(LocalPlayerPtr + OFFSET_VIEWANGLES, currentAngles);

            // Calculate Target Position
            Entity Target = getEntity(aimentity); // Still need bone logic, so using getEntity for now but may optimize later
            Vector TargetPos = Target.getBonePositionByHitbox(2); // Mid-chest for stability

            float distance = LocalCamera.DistTo(TargetPos);
            QAngle targetAngles = Math::CalcAngle(LocalCamera, TargetPos);

            // Recoil/Sway compensation
            QAngle SwayAngles;
            apex_mem.Read<QAngle>(LocalPlayerPtr + OFFSET_BREATH_ANGLES, SwayAngles);

            targetAngles -= (SwayAngles - currentAngles);
            Math::NormalizeAngles(targetAngles);

            float fov = Math::GetFov(currentAngles, targetAngles);
            if (fov <= assist_aim_fov)
            {
                float base_smooth = (smooth > 1.0f) ? smooth : 200.0f;
                float auto_smooth = base_smooth * 0.5f; // Faster than normal aimbot

                auto_smooth += fov * 2.0f;
                if (auto_smooth < 1.0f) auto_smooth = 1.0f;

                QAngle delta = targetAngles - currentAngles;
                Math::NormalizeAngles(delta);

                QAngle smoothed = currentAngles + delta / auto_smooth;
                Math::NormalizeAngles(smoothed);

                apex_mem.Write<SVector>(LocalPlayerPtr + OFFSET_VIEWANGLES, SVector(smoothed));

                static auto last_log_time = std::chrono::steady_clock::now();
                auto now = std::chrono::steady_clock::now();
                if (std::chrono::duration_cast<std::chrono::seconds>(now - last_log_time).count() >= 5) {
                    printf("[ASSIST_AIM] Tracking %lx, fov: %.2f, dist: %.2f, smooth: %.2f\n", aimentity, fov, distance/40.0f, auto_smooth);
                    last_log_time = now;
                }
            }
        }
    }
}
