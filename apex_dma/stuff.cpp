#include "stuff.h"
#include "Game.h"
#include "offsets_dynamic.h"
#include <thread>
#include <chrono>
#include <unordered_map>
#include <cmath>

extern Memory apex_mem;
extern Memory client_mem;
extern uint64_t g_Base;
extern uint64_t c_Base;
extern uintptr_t aimentity;
extern bool firing_range;
extern bool is_aimentity_visible;

bool flickbot = false;
bool flickbot_aiming = false;
float flickbot_fov = 10.0f;
float flickbot_smooth = 20.0f;

bool triggerbot = false;
bool triggerbot_aiming = false;
float triggerbot_fov = 10.0f;

bool stuff_t = false;

bool isAllowedWeapon(int weaponId, int zoomElapsedMs) {
    // Special long zoom delay for Kraber and Sentinel
    if (weaponId == 98 || weaponId == 1) { // KRABER, SENTINEL
        if (zoomElapsedMs < 950)
            return false; // Not ready to shoot yet
    } else {
        // All other weapons need 600ms after zoom
        if (zoomElapsedMs < 600)
            return false; // Not ready yet
    }

    // List of allowed weapons
    return (
        weaponId == 98 ||   // KRABER
        weaponId == 117 ||  // WINGMAN
        weaponId == 89 ||   // LONGBOW
        weaponId == 1   ||  // SENTINEL
        weaponId == 95  ||  // G7 SCOUT
        weaponId == 96  ||  // HEMLOCK
        weaponId == 120 ||  // 30-30
        weaponId == 116 ||  // TRIPLE TAKE
        weaponId == 182 ||  // BOCEK
        weaponId == 2   ||  // THROWING KNIFE
        weaponId == 114 ||  // P2020
        weaponId == 103 ||  // MOZAMBIQUE
        weaponId == 92  ||  // EVA-8
        weaponId == 111 ||  // PEACEKEEPER
        weaponId == 101 ||  // MASTIFF
        weaponId == 122     // NEMESIS
    );
}

bool IsInCrossHair(Entity& target)
{
    static std::unordered_map<uint64_t, float> lastTimes;  // per target

    float now_crosshair_target_time = target.lastCrossHairTime();

    if (std::isnan(now_crosshair_target_time)) {
        return false;
    }

    bool triggered = false;
    float last_time = lastTimes[target.ptr];  // 0 if first time seen

    if (now_crosshair_target_time > last_time) {
        triggered = true;
    }

    lastTimes[target.ptr] = now_crosshair_target_time;  // update for next check

    return triggered;
}

void FlickbotTriggerbotLoop()
{
	static auto zoomStartTime = std::chrono::steady_clock::now();
	static bool wasZooming = false;

	stuff_t = true;
	while (stuff_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));

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
			if (triggerbot && triggerbot_aiming) {
				if (isZooming && aimentity != 0) {
					Entity Target = getEntity(aimentity);
					int weaponId = LPlayer.getCurrentWeaponId();
					if (isAllowedWeapon(weaponId, zoomElapsedMs) && IsInCrossHair(Target)) {
						apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 5);
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						apex_mem.Write<int>(g_Base + OFFSET_IN_ATTACK + 0x8, 4);
					}
				}
			}

			// Flickbot logic
			if (flickbot && flickbot_aiming && aimentity != 0) {
				Entity Target = getEntity(aimentity);
				if (Target.isAlive() && (!Target.isKnocked() || firing_range) && is_aimentity_visible) {
					float fov = CalculateFov(LPlayer, Target);
					if (fov <= flickbot_fov) {
						QAngle old_angles = LPlayer.GetViewAngles();
						QAngle aim_angles = CalculateBestBoneAim(LPlayer, aimentity, flickbot_fov, flickbot_smooth);
						if (aim_angles.x != 0 || aim_angles.y != 0) {
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
	stuff_t = false;
}

void UpdateStuffVars(uint64_t add_addr)
{
    uint64_t flickbot_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 34, flickbot_addr);
    if (flickbot_addr) client_mem.Read<bool>(flickbot_addr, flickbot);

    uint64_t flickbot_aiming_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 36, flickbot_aiming_addr);
    if (flickbot_aiming_addr) client_mem.Read<bool>(flickbot_aiming_addr, flickbot_aiming);

    uint64_t triggerbot_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 37, triggerbot_addr);
    if (triggerbot_addr) client_mem.Read<bool>(triggerbot_addr, triggerbot);

    uint64_t triggerbot_aiming_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 39, triggerbot_aiming_addr);
    if (triggerbot_aiming_addr) client_mem.Read<bool>(triggerbot_aiming_addr, triggerbot_aiming);

    uint64_t flickbot_fov_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 44, flickbot_fov_addr);
    if (flickbot_fov_addr) client_mem.Read<float>(flickbot_fov_addr, flickbot_fov);

    uint64_t flickbot_smooth_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 45, flickbot_smooth_addr);
    if (flickbot_smooth_addr) client_mem.Read<float>(flickbot_smooth_addr, flickbot_smooth);

    uint64_t triggerbot_fov_addr = 0;
    client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 46, triggerbot_fov_addr);
    if (triggerbot_fov_addr) client_mem.Read<float>(triggerbot_fov_addr, triggerbot_fov);
}
