#include "StuffBot.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <cfloat>
#include <algorithm>
#include "offsets.h"
#include "Weapon.h"
#include "prediction.h"

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
extern int triggerbot_type;
extern int triggerbot_delay;
extern float triggerbot_padding;
extern bool triggerbot_hitboxes;
extern bool triggerbot_prediction;

extern bool firing_range;
extern bool is_aimentity_visible;
bool stuff_t = false;

struct Matrix4x4 {
    float matrix[16];
};

Vector GetBoxDimensionsForBone(int boneIndex) {
    switch (boneIndex) {
    case 0: // Head
        return { 5.0f, 5.0f, 5.0f };
    case 1: // Neck
        return { 4.0f, 4.0f, 4.0f };
    case 2: // Upper Chest
        return { 7.0f, 7.0f, 10.0f };
    case 3: // Lower Chest
        return { 8.0f, 8.0f, 12.0f };
    default:
        return { 6.0f, 6.0f, 8.0f };
    }
}

std::vector<Vector> CalculateBoxCorners(Vector bonePos, Vector dimensions) {
    return {
        {bonePos.x + dimensions.x, bonePos.y + dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y + dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y - dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x + dimensions.x, bonePos.y - dimensions.y, bonePos.z + dimensions.z},
        {bonePos.x + dimensions.x, bonePos.y + dimensions.y, bonePos.z - dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y + dimensions.y, bonePos.z - dimensions.z},
        {bonePos.x - dimensions.x, bonePos.y - dimensions.y, bonePos.z - dimensions.z},
        {bonePos.x + dimensions.x, bonePos.y - dimensions.y, bonePos.z - dimensions.z}
    };
}

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

void TriggerBotRun(int delay)
{
    if (delay > 0) std::this_thread::sleep_for(std::chrono::milliseconds(delay));
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
        if (triggerbot && triggerbot_aiming && aimentity != 0)
        {
            if (isZooming) {
                int weaponId = LPlayer.getCurrentWeaponId();
                if (isAllowedWeapon(weaponId, zoomElapsedMs)) {
                    Entity Target = getEntity(aimentity);
                    bool shouldTrigger = false;

                    if (triggerbot_type == 0) {
                        shouldTrigger = IsInCrossHair(Target);
                    } else if (triggerbot_type == 1) {
                        // Bounding Box Logic
                        extern int screen_width;
                        extern int screen_height;
                        uint64_t viewRenderer = 0;
                        apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer);
                        uint64_t viewMatrixPtr = 0;
                        apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrixPtr);
                        Matrix4x4 m = {};
                        apex_mem.Read<Matrix4x4>(viewMatrixPtr, m);

                        std::vector<int> bones = { 0, 1, 2, 3 };
                        for (int boneIndex : bones) {
                            Vector bonePos = Target.getBonePositionByHitbox(boneIndex);
                            if (bonePos.IsZero()) continue;

                            if (triggerbot_prediction) {
                                WeaponXEntity curweap = WeaponXEntity();
                                curweap.update(LocalPlayer);
                                float bulletSpeed = curweap.get_projectile_speed();
                                float bulletGrav = curweap.get_projectile_gravity();

                                if (bulletSpeed > 1.0f) {
                                    PredictCtx ctx;
                                    ctx.StartPos = LPlayer.GetCamPos();
                                    ctx.TargetPos = bonePos;
                                    ctx.BulletSpeed = bulletSpeed - (bulletSpeed * 0.08f);
                                    ctx.BulletGravity = bulletGrav + (bulletGrav * 0.05f);
                                    ctx.TargetVel = Target.getAbsVelocity();
                                    if (BulletPredict(ctx)) {
                                        bonePos = ctx.TargetPos;
                                    }
                                }
                            }

                            Vector dimensions = GetBoxDimensionsForBone(boneIndex);
                            dimensions.x *= (1.0f + triggerbot_padding);
                            dimensions.y *= (1.0f + triggerbot_padding);
                            dimensions.z *= (1.0f + triggerbot_padding);

                            std::vector<Vector> corners = CalculateBoxCorners(bonePos, dimensions);
                            float minX = FLT_MAX, maxX = -FLT_MAX, minY = FLT_MAX, maxY = -FLT_MAX;
                            bool anyVisible = false;

                            for (const auto& corner : corners) {
                                Vector screenPos;
                                if (WorldToScreen(corner, m.matrix, screen_width, screen_height, screenPos)) {
                                    anyVisible = true;
                                    minX = std::min(minX, screenPos.x);
                                    maxX = std::max(maxX, screenPos.x);
                                    minY = std::min(minY, screenPos.y);
                                    maxY = std::max(maxY, screenPos.y);
                                }
                            }

                            if (anyVisible) {
                                float centerX = screen_width / 2.0f;
                                float centerY = screen_height / 2.0f;
                                if (centerX >= minX && centerX <= maxX && centerY >= minY && centerY <= maxY) {
                                    shouldTrigger = true;
                                    break;
                                }
                            }
                        }
                    }

                    if (shouldTrigger)
                    {
                        printf("[TRIGGERBOT] Shooting with %s (ID: %d)\n", get_weapon_name(weaponId).c_str(), weaponId);
                        TriggerBotRun(triggerbot_delay);
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
