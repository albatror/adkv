#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <random>
#include <chrono>
#include <iostream>
#include <cfloat>
#include "offsets_dynamic.h"
#include "Game.h"
#include "Config.h"
#include <thread>
#include <array>
#include <fstream>
////////////////////////
////////////////////////

Memory apex_mem;

bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
int team_player = 0;
const int toRead = 100;

bool shooting = false;

bool isGrappling;
int grappleAttached;

///////////
//bool medbackpack = true;
///////////
bool updateInsideValue_t = false;
///////////////////////////
//Player Glow Color and Brightness.
//Item Configs
//loot Fill
//unsigned char lootfilled = 5;  //0 no fill, 14 100% fill
//loot outline siez
//unsigned char lootoutline = 0;
///////////////////////////

bool actions_t = false;
bool aim_t = false;
//bool item_t = false;
uint64_t g_Base;
bool lock = false;
bool is_aimentity_visible = false;

//map
int map = 0;


// DÃ©clarations au niveau global (ou dans votre classe si besoin)

int tapstrafe = 0;  // InitialisÃ© Ã  0, commence le comptage

bool forward_hold = false;  // InitialisÃ© Ã  faux, pas de clÃ© maintenue au dÃ©but

struct Matrix
{
	float matrix[16];
};

float lastvis_aim[toRead];

////////////
int settingIndex;
int contextId;
std::array<float, 3> highlightParameter;
///////////

//////////////////////////////////////////
//works
// Inside SetPlayerGlow function
void SetPlayerGlow(Entity& LPlayer, Entity& Target, int index)
{
	if (config.player_glow)
    	{
    			if (!Target.isGlowing() || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE] != 1) {
    				float currentEntityTime = 5000.f;
    				if (!isnan(currentEntityTime) && currentEntityTime > 0.f) {
					if (!(config.firing_range) && (Target.isKnocked() || !Target.isAlive()))
    					{
						contextId = config.knocked_context_id;
						settingIndex = config.knocked_setting_index;
						highlightParameter = { config.glow_r_knocked, config.glow_g_knocked, config.glow_b_knocked };
    					}
    					else if (Target.lastVisTime() > lastvis_aim[index] || (Target.lastVisTime() < 0.f && lastvis_aim[index] > 0.f))
    					{
						contextId = config.visible_context_id;
						settingIndex = config.visible_setting_index;
						highlightParameter = { config.glow_r_viz, config.glow_g_viz, config.glow_b_viz };
    					}
    					else 
    					{
						contextId = config.not_visible_context_id;
						settingIndex = config.not_visible_setting_index;
						highlightParameter = { config.glow_r, config.glow_g, config.glow_b };
    					}
    					Target.enableGlow();
    				}
    			}
    	}
    	//////////////////////////////////////////////////////////////////////////////////////////////////
		else if(!config.player_glow && Target.isGlowing())
		{
			Target.disableGlow();
		}
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessPlayer(Entity &LPlayer, Entity &target, uint64_t entitylist, int index)
{
	int entity_team = target.getTeamId();

	if (!target.isAlive())
	{
		return;
	}

	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);
		if (dist > config.max_dist)
		return;

	if(!config.firing_range && !config.onevone)
		if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
			return;
	
	bool visible = (target.lastVisTime() > lastvis_aim[index]);
	float fov = CalculateFov(LPlayer, target);

	if (target.ptr == aimentity)
	{
		is_aimentity_visible = visible;
	}

	if (aimentity != 0 && lock)
	{
		// Stick to target
	}
	else if (config.aim == 2)
	{
		if (visible && fov <= config.max_fov)
		{
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
		else
		{
			if (aimentity == target.ptr && !shooting)
			{
				aimentity = tmp_aimentity = lastaimentity = 0;
			}
		}
	}
	else
	{
		if (fov <= config.max_fov)
		{
			if (fov < max)
			{
				max = fov;
				tmp_aimentity = target.ptr;
			}
		}
	}
	////
	SetPlayerGlow(LPlayer, target, index);
	////
	lastvis_aim[index] = target.lastVisTime();
}

//walljump +//////////////////////////////////////
	int onWallTmp = 0;
	int wallJumpNow = 0;
	int onWallOffTmp = 0;
	float onEdgeTmp = 0;

	//bool autoWallJumpEnabled = true; // Initialize auto wall jump as enabled
//walljump *//////////////////////////////////////

void DoActions()
{
	actions_t = true;

///////////////

//////////////

	while (actions_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));

		while (g_Base != 0)
		{
///////////////
			char MapName[200] = { 0 };
			uint64_t MapName_ptr = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_HOST_MAP, MapName_ptr);
			apex_mem.ReadArray<char>(MapName_ptr, MapName, 200);
					
			//printf("%s\n", MapName);
			if (strcmp(MapName, "mp_rr_tropic_island_mu1_storm") == 0) 
			{
				map = 1;
			} 
			else if (strcmp(MapName, "mp_rr_canyonlands_mu") == 0) 
			{
				map = 2;
			}
			else if (strcmp(MapName, "mp_rr_desertlands_hu") == 0) 
			{
				map = 3;
			}
			else if (strcmp(MapName, "mp_rr_olympus") == 0) 
			{
				map = 4;
			} 
			else  if (strcmp(MapName, "mp_rr_divided_moon") == 0)
			{
				map = 5;
			}
			else
			{
				map = 0;
			}
			
///////////////
			std::this_thread::sleep_for(std::chrono::milliseconds(30));

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

// Ensure the local player entity is valid
			if (LocalPlayer == 0)
				continue;

// Retrieve the local player entity object
Entity LPlayer = getEntity(LocalPlayer);

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || (team_player > 50 && !config.onevone))
			{
				continue;
			}

			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

//////////////////////////////////

//walljump ++/////////////////////////////////////
    bool success; // Declare success once
    int onWall;
    // Corrected memory read call
    success = apex_mem.Read<int>(LocalPlayer + OFFSET_WALL_RUN_START_TIME, onWall);
    if (success && onWall != onWallTmp)
    {
        int inForward;
        success = apex_mem.Read<int>(g_Base + OFFSET_IN_FORWARD, inForward);
        if (success && inForward == 0)  
        {
            wallJumpNow = 1;
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
        }
    }
    onWallTmp = onWall;

    int onWallOff;
    success = apex_mem.Read<int>(LocalPlayer + OFFSET_WALL_RUN_CLEAR_TIME, onWallOff);
    if (success && wallJumpNow == 1 && onWallOff != onWallOffTmp)
    {
        wallJumpNow = 0;
        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
    }
    onWallOffTmp = onWallOff;

    float onEdge;
    success = apex_mem.Read<float>(LocalPlayer + OFFSET_TRAVERSAL_PROGRESS, onEdge);
    if (success && onEdge != onEdgeTmp)
    {
        int inForward;
        success = apex_mem.Read<int>(g_Base + OFFSET_IN_FORWARD, inForward);
        if (success && inForward == 0)
        {
            wallJumpNow = 2;
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
            // Consider the impact of sleep here, potentially remove or adjust
            // std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        }
    }
    onEdgeTmp = onEdge;

    if (wallJumpNow == 2)
    {
        uint32_t flags;
        success = apex_mem.Read<uint32_t>(LocalPlayer + OFFSET_FLAGS, flags);
        if (success && (flags & 0x1) == 1)
        {
            wallJumpNow = 0;
            apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
        }
    }

//walljump ++///////////////

    // SUPERGLIDE

       static float startjumpTime = 0;
       static bool startSg = false;
       static float traversalProgressTmp = 0.0;
        
       float worldtime;
       if (!apex_mem.Read<float>(LocalPlayer + OFFSET_TIME_BASE, worldtime)) {
         // error handling 
       }
        
       float traversalStartTime;
       if (!apex_mem.Read<float>(LocalPlayer + OFFSET_TRAVERSAL_STARTTIME, traversalStartTime)) {
         // error handling
       }
        
       float traversalProgress;
       if (!apex_mem.Read<float>(LocalPlayer + OFFSET_TRAVERSAL_PROGRESS, traversalProgress)) {
         // error handling
       }
        
       auto HangOnWall = -(traversalStartTime - worldtime);
        
       // Adjust thresholds and delays based on frame rate
       float wallHangThreshold = 0.1f;
       float wallHangMax = 1.5f;
       float traversalProgressThreshold = 0.87f;
       float jumpPressLoopTime = 0.011f;
       int duckActionDelay = 50;
       int jumpResetDelay = 800;
        
       // Check if SPACEBAR is pressed and held
       if (config.super_key) {
           if (HangOnWall > wallHangThreshold && HangOnWall < wallHangMax) {
             apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
           }
            
           if (traversalProgress > traversalProgressThreshold && !startSg && HangOnWall > wallHangThreshold && HangOnWall < wallHangMax) {
             // Start SG
             startjumpTime = worldtime; 
             startSg = true;
           }
            
           if (startSg) {
             // Press button
             apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
            
             float currentTime;
             while (true) {
               if (apex_mem.Read<float>(LocalPlayer + OFFSET_TIME_BASE, currentTime)) {
                 if (currentTime - startjumpTime < jumpPressLoopTime) {
                   // Keep looping
                 } else {
                   break; 
                 }
               }
             }
             
             // Execute actions during SG
             apex_mem.Write<int>(g_Base + OFFSET_IN_DUCK + 0x8, 6);
             std::this_thread::sleep_for(std::chrono::milliseconds(duckActionDelay));
             apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
             std::this_thread::sleep_for(std::chrono::milliseconds(jumpResetDelay));
            
             startSg = false;
           }
       }
       //////////////////////////////

////////////////////////////////
//WALLJUMP END
////////////////////////////////

// Check if grapple is active
apex_mem.Read<bool>(LocalPlayer + OFFSET_GRAPPLEACTIVED, isGrappling);

// Check if grapple is attached
apex_mem.Read<int>(LocalPlayer + OFFSET_GRAPPLE + OFFSET_GRAPPLEATTACHED, grappleAttached);

// Main logic
if (isGrappling && grappleAttached == 1) {

  // Boost by jumping
  apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5); 
  std::this_thread::sleep_for(std::chrono::milliseconds(5));
  apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);

} else {

  // Reset values
  isGrappling = false;
  grappleAttached = 0;

}

//grapple END/////////////////////////////

//bhop///
//if (bhop_enable) {
//apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
//std::this_thread::sleep_for(std::chrono::milliseconds(1)); 
//apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
//std::this_thread::sleep_for(std::chrono::milliseconds(1));
//}
//bhop END/////////////////////////////

			uint64_t baseent = 0;
			apex_mem.Read<uint64_t>(entitylist, baseent);
			if (baseent == 0)
			{
				continue;
			}

			max = 999.0f;
			tmp_aimentity = 0;
			is_aimentity_visible = false;

			// Read shooting state from game
			kbutton_t in_attack_button;
			apex_mem.Read<kbutton_t>(g_Base + OFFSET_IN_ATTACK, in_attack_button);
			shooting = (in_attack_button.state & 1) != 0;

			if (config.firing_range)
			{
				int c = 0;
				for (int i = 0; i < 10000; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
						continue;

					Entity Target = getEntity(centity);
					if (!Target.isDummy())
					{
						continue;
					}

					if (config.player_glow && !Target.isGlowing())
					{
						Target.enableGlow();
					}
					else if (!config.player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}

					ProcessPlayer(LPlayer, Target, entitylist, c);
					c++;
				}
			}
			else
			{
				for (int i = 0; i < toRead; i++)
				{
					uint64_t centity = 0;
					apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
					if (centity == 0)
						continue;
					if (LocalPlayer == centity)
						continue;
					Entity Target = getEntity(centity);
					if (!Target.isPlayer())
					{
						continue;
					}
					
					ProcessPlayer(LPlayer, Target, entitylist, i);

					int entity_team = Target.getTeamId();
					if (entity_team == team_player && !config.onevone)
					{
						continue;
					}

					if (config.player_glow && !Target.isGlowing())
					{
						Target.enableGlow();
					}
					else if (!config.player_glow && Target.isGlowing())
					{
						Target.enableGlow();
						//Target.disableGlow();
					}
				}
			}

			if (!lock){
				aimentity = tmp_aimentity;
			}else{
				aimentity = lastaimentity;
			}
		}
	}

	actions_t = false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////

static void AimbotLoop()
{
	static uintptr_t last_locked_entity = 0;
	static std::chrono::steady_clock::time_point lock_start_time;

	aim_t = true;
	while (aim_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while (g_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (config.aim > 0)
			{
				if (aimentity == 0 || !config.aiming)
				{
					lock = false;
					lastaimentity = 0;
					last_locked_entity = 0;
					continue;
				}

				Entity Target = getEntity(aimentity);
				if (!Target.isAlive() || (Target.isKnocked() && !config.firing_range))
				{
					lock = false;
					lastaimentity = 0;
					last_locked_entity = 0;
					aimentity = 0;
					continue;
				}

				lock = true;
				lastaimentity = aimentity;

				if (aimentity != last_locked_entity) {
					last_locked_entity = aimentity;
					lock_start_time = std::chrono::steady_clock::now();
				}

				float current_smooth = config.smooth;
				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lock_start_time).count();
				if (elapsed < 500) {
					current_smooth = config.smooth * 2.0f;
				}

				uint64_t LocalPlayer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

				if (LocalPlayer == 0)
					continue;

				Entity LPlayer = getEntity(LocalPlayer);

				float fov = CalculateFov(LPlayer, Target);
				if (fov > config.max_fov)
				{
					if (!shooting) {
						lock = false;
						lastaimentity = 0;
						last_locked_entity = 0;
						aimentity = 0;
					}
					continue;
				}

				if (config.aim == 2 && !is_aimentity_visible)
				{
					continue;
				}

				QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, config.max_fov, current_smooth);
				if (Angles.x == 0 && Angles.y == 0)
				{
					continue;
				}

				LPlayer.SetViewAngles(Angles);
			}
		}
	}
	aim_t = false;
}

int main(int argc, char *argv[])
{
	mf_log_init(LevelFilter::LevelFilter_Info);
	config.load("settings.ini");

	if(geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}

	const char* ap_proc = "r5apex_dx12.ex";
	//const char* ap_proc = "EasyAntiCheat_launcher.exe";

	std::thread aimbot_thr;
	std::thread actions_thr;

	bool proc_not_found = false;
	while (active)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (aim_t)
			{
				aim_t = false;
				actions_t = false;
				g_Base = 0;

				aimbot_thr.~thread();
				actions_thr.~thread();
			}

			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for apex process...\n");
			proc_not_found = apex_mem.get_proc_status() == process_status::NOT_FOUND;
			if (proc_not_found)
			{
				std::this_thread::sleep_for(std::chrono::seconds(1));
				printf("Searching for apex process...\n");
			}

			apex_mem.open_proc(ap_proc);

			if (apex_mem.get_proc_status() == process_status::FOUND_READY)
			{
				g_Base = apex_mem.get_proc_baseaddr();
				if (proc_not_found)
				{
					printf("\nApex process found\n");
					printf("Base: %lx\n", g_Base);
				}

				aimbot_thr = std::thread(AimbotLoop);
				actions_thr = std::thread(DoActions);
				aimbot_thr.detach();
				actions_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}
