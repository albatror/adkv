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
#include "StuffBot.h"
#include "Weapon.h"
#include "ItemManager.h"
#include <thread>
#include <array>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <cmath>
////////////////////////
////////////////////////

Memory apex_mem;
Memory client_mem;

bool firing_range = false;
bool active = true;
uintptr_t aimentity = 0;
uintptr_t tmp_aimentity = 0;
uintptr_t lastaimentity = 0;
float max = 999.0f;
float max_dist = 200.0f * 40.0f;
float aim_dist = 200.0f * 40.0f;
int team_player = 0;
float ads_fov = 3.0f;
float ads_smooth = 15.0f;
float hip_fov = 8.0f;
float hip_smooth = 25.0f;

const int toRead = 100;

int aim = false;
bool esp = false;
bool skeleton = false;
float skeleton_thickness = 1.0f;
bool item_glow = false;
bool player_glow = false;
bool aim_no_recoil = true;
bool lock_target = false;
bool aiming = false;

int bone = 2;
bool shooting = false;

int SuperKey = false;

bool isGrappling;
int grappleAttached;

//Firing Range 1v1 toggle
bool onevone = false;

bool aassist = false;
float aassist_dist = 50.0f * 40.0f;
bool aassist_aiming = false;

bool triggerbot = false;
int triggerbot_key = 0xA0; // VK_LSHIFT
bool triggerbot_aiming = false;
float triggerbot_fov = 10.0f;

bool superglide = false;
bool bhop = false;
bool walljump = false;

///////////////////////////
//Player Glow Color and Brightness.
//inside fill
unsigned char insidevalue = 6;  //0 = no fill, 14 = full fill
//Outline size
unsigned char outlinesize = 32; // 0-255
//Not Visable 
float glowr = 1; //Red 0-255, higher is brighter color.
float glowg = 0; //Green 0-255, higher is brighter color.
float glowb = 0; //Blue 0-255, higher is brighter color.
//Visable
float glowrviz = 0; //Red 0-255, higher is brighter color.
float glowgviz = 1; //Green 0-255, higher is brighter color.
float glowbviz = 0; //Blue 0-255, higher is brighter color.
//Knocked
float glowrknocked = 0; //Red 0-255, higher is brighter color.
float glowgknocked = 0; //Green 0-255, higher is brighter color.
float glowbknocked = 1; //Blue 0-255, higher is brighter color.
//Item Configs
//loot Fill
//unsigned char lootfilled = 5;  //0 no fill, 14 100% fill
//loot outline siez
//unsigned char lootoutline = 0;
///////////////////////////

bool actions_t = false;
bool esp_t = false;
bool aim_t = false;
bool vars_t = false;
bool item_t = false;
uint64_t g_Base;
uint64_t c_Base;
bool next = false;
bool valid = false;
bool lock = false;
bool is_aimentity_visible = false;

int screen_width = 2560;
int screen_height = 1440;

typedef struct player
{
	float dist = 0;
	int entity_team = 0;
	float boxMiddle = 0;
	float h_y = 0;
	float width = 0;
	float height = 0;
	float b_x = 0;
	float b_y = 0;
	bool knocked = false;
	bool visible = false;
	int health = 0;
	int shield = 0;
	int maxshield = 0;
	int armortype = 0;
	int player_xp_level = 0;
	int platform = 0;
	char name[33] = { 0 };
	char weapon[33] = { 0 };
	float bones[17][2] = { 0 };
}player;

typedef struct spectator{
	bool is_spec = false;
	char name[33] = { 0 };
}spectator;

struct Matrix
{
	float matrix[16];
};

spectator spectator_list[toRead];

float lastvis_esp[toRead];
float lastvis_aim[toRead];

int tmp_spec = 0, spectators = 0;
int tmp_all_spec = 0, allied_spectators = 0;

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
    	if (player_glow >= 1)
    	{
    			if (!Target.isGlowing() || (int)Target.buffer[OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE] != 1) {
    				float currentEntityTime = 5000.f;
    				if (!isnan(currentEntityTime) && currentEntityTime > 0.f) {
    					if (!(firing_range) && (Target.isKnocked() || !Target.isAlive()))
    					{
    						contextId = 5;
    						settingIndex = 80;
    						highlightParameter = { glowrknocked, glowgknocked, glowbknocked };
    					}
    					else if (Target.lastVisTime() > lastvis_aim[index] || (Target.lastVisTime() < 0.f && lastvis_aim[index] > 0.f))
    					{
    						contextId = 6;
    						settingIndex = 81;
    						highlightParameter = { glowrviz, glowgviz, glowbviz };
    					}
    					else 
    					{
    						contextId = 7;
    						settingIndex = 82;
    						highlightParameter = { glowr, glowg, glowb };
    					}
    					Target.enableGlow();
    				}
    			}
    	}
    	//////////////////////////////////////////////////////////////////////////////////////////////////
		else if((player_glow == 0) && Target.isGlowing())
		{
			Target.disableGlow();
		}
}
//////////////////////////////////////////////////////////////////////////////////////////////////

void ProcessPlayer(Entity &LPlayer, Entity &target, uint64_t entitylist, int index, uint64_t spectated_ptr)
{
	int entity_team = target.getTeamId();
	
	if (!target.isAlive())
	{
		if (target.Observing(spectated_ptr))
		{
			if (LPlayer.getTeamId() == entity_team)
				tmp_all_spec++;
			else
				tmp_spec++;

			if (index >= 0 && index < toRead) {
				target.get_name(g_Base, spectator_list[index].name);
				spectator_list[index].is_spec = true;
			}
		}
		return;
	}

	Vector EntityPosition = target.getPosition();
	Vector LocalPlayerPosition = LPlayer.getPosition();
	float dist = LocalPlayerPosition.DistTo(EntityPosition);

	float active_max_dist = max_dist;

	if (dist > active_max_dist)
		return;

	if(!firing_range && !onevone)
		if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
			return;
	
	bool visible = (target.lastVisTime() > lastvis_aim[index]);

	// Use head position for FOV calculation to be more accurate at close range
	Vector HeadPos = target.getBonePositionByHitbox(0);
	float fov = Math::GetFov(LPlayer.GetViewAngles(), Math::CalcAngle(LPlayer.GetCamPos(), HeadPos));

	if (target.ptr == aimentity)
	{
		is_aimentity_visible = visible;
	}

	float active_fov = LPlayer.isZooming() ? ads_fov : hip_fov;
	float active_dist = (aassist && aassist_aiming) ? aassist_dist : aim_dist;

	if (triggerbot) {
		if (triggerbot_fov > active_fov) active_fov = triggerbot_fov;
	}

	float priority = fov + logf(fmaxf(dist / 40.0f, 1.0f));

	if (aimentity != 0 && lock)
	{
		// Stick to target
	}
	else if (aim == 2)
	{
		if (visible && fov <= active_fov && dist <= active_dist)
		{
			if (priority < max)
			{
				max = priority;
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
		if (fov <= active_fov && dist <= active_dist)
		{
			if (priority < max)
			{
				max = priority;
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

		while (g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(30));

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);

// Ensure the local player entity is valid
			if (LocalPlayer == 0)
				continue;

// Retrieve the local player entity object
Entity LPlayer = getEntity(LocalPlayer);

			uint64_t spectated_ptr = 0;
			uint32_t spec_handle = 0;
			if (LPlayer.isAlive()) {
				apex_mem.Read<uint32_t>(g_Base + OFFSET_LOCAL_ENTITY_HANDLE, spec_handle);
			} else {
				apex_mem.Read<uint32_t>(LocalPlayer + OFFSET_OBSERVING_TARGET, spec_handle);
			}

			if (spec_handle != 0xFFFFFFFF && spec_handle != 0) {
				apex_mem.Read<uint64_t>(g_Base + OFFSET_ENTITYLIST + ((spec_handle & 0xFFFF) << 5), spectated_ptr);
			}

			if (spectated_ptr == 0)
				spectated_ptr = LPlayer.ptr;

			team_player = LPlayer.getTeamId();
			if (team_player < 0 || team_player > 50 && !onevone)
			{
				continue;
			}

			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;

//////////////////////////////////

//walljump ++/////////////////////////////////////
if (walljump) {
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
       if (superglide && SuperKey) {
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
if (bhop && SuperKey) {
    uint32_t flags;
    apex_mem.Read<uint32_t>(LocalPlayer + OFFSET_FLAGS, flags);
    if (flags & 0x1) {
        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 5);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        apex_mem.Write<int>(g_Base + OFFSET_IN_JUMP + 0x8, 4);
    }
}
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

			tmp_spec = 0;
			tmp_all_spec = 0;

			// Clear spectator list every frame to prevent ghosting/stale data
			memset(spectator_list, 0, sizeof(spectator_list));

			if (firing_range)
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

					if (player_glow && !Target.isGlowing())
					{
						Target.enableGlow();
					}
					else if (!player_glow && Target.isGlowing())
					{
						Target.disableGlow();
					}

					ProcessPlayer(LPlayer, Target, entitylist, c, spectated_ptr);
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
					
					ProcessPlayer(LPlayer, Target, entitylist, i, spectated_ptr);
				}
			}

			spectators = tmp_spec;
			allied_spectators = tmp_all_spec;

			if (!lock){
				aimentity = tmp_aimentity;
			}else{
				aimentity = lastaimentity;
			}
		}
	}

	actions_t = false;
}
player players[toRead];

static void EspLoop()
{
	esp_t = true;
	while (esp_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		while(g_Base != 0 && c_Base != 0)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if (esp)
			{
				valid = false;

			uint64_t LocalPlayer = 0;
			apex_mem.Read<uint64_t>(g_Base + OFFSET_LOCAL_ENT, LocalPlayer);
				if (LocalPlayer == 0)
{
    next = true;
    while(next && g_Base != 0 && c_Base != 0 && esp)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    continue;
}

// Ensure the local player entity is valid
if (LocalPlayer == 0)
{
    next = true;
    while(next && g_Base != 0 && c_Base != 0 && esp)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    continue;
}

// Retrieve the local player entity object
Entity LPlayer = getEntity(LocalPlayer);

				int team_player = LPlayer.getTeamId();
				if (team_player < 0 || team_player > 50)
				{
					next = true;
					while(next && g_Base != 0 && c_Base != 0 && esp)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1));
					}
					continue;
				}
				Vector LocalPlayerPosition = LPlayer.getPosition();

				uint64_t viewRenderer = 0;
				apex_mem.Read<uint64_t>(g_Base + OFFSET_RENDER, viewRenderer);
				uint64_t viewMatrix = 0;
				apex_mem.Read<uint64_t>(viewRenderer + OFFSET_MATRIX, viewMatrix);
				Matrix m = {};
				apex_mem.Read<Matrix>(viewMatrix, m);

				uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
				
				memset(players, 0, sizeof(players));
				if (firing_range)
				{
					int c = 0;
					for (int i = 0; i < 10000; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}

						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);

						if (!Target.isDummy())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}
						int entity_team = Target.getTeamId();


						if (!onevone)
						{
							if (entity_team < 0 || entity_team>50 || entity_team == team_player)
							{
								continue;
							}
						}
						else
						{
							if (entity_team < 0 || entity_team>50)
							{
                              continue;
                            }
						}

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{	
							continue;
						}
						
						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, screen_width, screen_height, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							WorldToScreen(HeadPosition, m.matrix, screen_width, screen_height, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxShield();
							int armortype = Target.getArmortype();
							int platform = 0;
							apex_mem.Read<int>(Target.ptr + OFFSET_PLATFORM, platform);
							players[c] = 
							{
								dist,
								entity_team,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								0,
								(Target.lastVisTime() > lastvis_esp[c]),
								health,
								shield,
								maxshield,
								armortype,
								0, // xp_level
								platform
							};

							if (skeleton)
							{
								// 0:Head, 1:Neck, 2:UpperChest, 3:LowerChest, 4:Pelvis, 5:L_Shoulder, 6:L_Elbow, 7:L_Hand, 8:R_Shoulder, 9:R_Elbow, 10:R_Hand, 11:L_Hip, 12:L_Knee, 13:L_Foot, 14:R_Hip, 15:R_Knee, 16:R_Foot
								int boneIDs[] = { 0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18 };
								for (int b = 0; b < 17; b++)
								{
									Vector bonePos = Target.getBonePositionByHitbox(boneIDs[b]);
									Vector boneScreen = Vector();
									if (WorldToScreen(bonePos, m.matrix, screen_width, screen_height, boneScreen))
									{
										players[c].bones[b][0] = boneScreen.x;
										players[c].bones[b][1] = boneScreen.y;
									}
								}
							}

							Target.get_name(g_Base, &players[c].name[0]);

							char weaponModel[256] = { 0 };
							Target.getWeaponModelName(weaponModel, 256);
							std::string weaponName = get_weapon_name_by_model(weaponModel);
							if (weaponName == "Unknown") {
								int weaponId = Target.getCurrentWeaponId();
								weaponName = get_weapon_name(weaponId);
							}
							strncpy(players[c].weapon, weaponName.c_str(), 32);

							lastvis_esp[c] = Target.lastVisTime();
							valid = true;
							c++;
						}
					}
				}	
				else
				{
					for (int i = 0; i < toRead; i++)
					{
						uint64_t centity = 0;
						apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
						if (centity == 0)
						{
							continue;
						}
						
						if (LocalPlayer == centity)
						{
							continue;
						}

						Entity Target = getEntity(centity);
						
						if (!Target.isPlayer())
						{
							continue;
						}

						if (!Target.isAlive())
						{
							continue;
						}

						int entity_team = Target.getTeamId();
						if (!onevone)
						{
							if (entity_team < 0 || entity_team > 50 || entity_team == team_player)
							{
								continue;
							}
						}
						else
						{
							if (entity_team < 0 || entity_team>50)
							{
                              continue;
                            }						}

						Vector EntityPosition = Target.getPosition();
						float dist = LocalPlayerPosition.DistTo(EntityPosition);
						if (dist > max_dist || dist < 50.0f)
						{	
							continue;
						}

						Vector bs = Vector();
						WorldToScreen(EntityPosition, m.matrix, screen_width, screen_height, bs);
						if (bs.x > 0 && bs.y > 0)
						{
							Vector hs = Vector();
							Vector HeadPosition = Target.getBonePositionByHitbox(0);
							WorldToScreen(HeadPosition, m.matrix, screen_width, screen_height, hs);
							float height = abs(abs(hs.y) - abs(bs.y));
							float width = height / 2.0f;
							float boxMiddle = bs.x - (width / 2.0f);
							int health = Target.getHealth();
							int shield = Target.getShield();
							int maxshield = Target.getMaxShield();
							int armortype = Target.getArmortype();
							int platform = 0;
							apex_mem.Read<int>(Target.ptr + OFFSET_PLATFORM, platform);
							players[i] = 
							{
								dist,
								entity_team,
								boxMiddle,
								hs.y,
								width,
								height,
								bs.x,
								bs.y,
								Target.isKnocked(),
								(Target.lastVisTime() > lastvis_esp[i]),
								health,
								shield,
								maxshield,
								armortype,
								0, // xp_level
								platform
							};

							if (skeleton)
							{
								// 0:Head, 1:Neck, 2:UpperChest, 3:LowerChest, 4:Pelvis, 5:L_Shoulder, 6:L_Elbow, 7:L_Hand, 8:R_Shoulder, 9:R_Elbow, 10:R_Hand, 11:L_Hip, 12:L_Knee, 13:L_Foot, 14:R_Hip, 15:R_Knee, 16:R_Foot
								int boneIDs[] = { 0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 18 };
								for (int b = 0; b < 17; b++)
								{
									Vector bonePos = Target.getBonePositionByHitbox(boneIDs[b]);
									Vector boneScreen = Vector();
									if (WorldToScreen(bonePos, m.matrix, screen_width, screen_height, boneScreen))
									{
										players[i].bones[b][0] = boneScreen.x;
										players[i].bones[b][1] = boneScreen.y;
									}
								}
							}

							Target.get_name(g_Base, &players[i].name[0]);

							char weaponModel[256] = { 0 };
							Target.getWeaponModelName(weaponModel, 256);
							std::string weaponName = get_weapon_name_by_model(weaponModel);
							if (weaponName == "Unknown") {
								int weaponId = Target.getCurrentWeaponId();
								weaponName = get_weapon_name(weaponId);
							}
							strncpy(players[i].weapon, weaponName.c_str(), 32);

							lastvis_esp[i] = Target.lastVisTime();
							valid = true;
						}
					}
				}

				next = true;
				while(next && g_Base != 0 && c_Base != 0 && esp)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(1));
				}
			}
		}
	}
	esp_t = false;
}

static void AimbotLoop()
{
	static uintptr_t last_locked_entity = 0;
	static std::chrono::steady_clock::time_point lock_start_time;

	// Zoom tracking
	static auto zoomStartTime = std::chrono::steady_clock::now();
	static bool wasZooming = false;

	aim_t = true;
	while (aim_t)
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


			if (aim > 0)
			{
				bool is_aiming = aiming || (aassist && aassist_aiming);
				if (aimentity == 0 || !is_aiming)
				{
					lock = false;
					lastaimentity = 0;
					last_locked_entity = 0;
					continue;
				}

				Entity Target = getEntity(aimentity);
				if (!Target.isAlive() || (Target.isKnocked() && !firing_range))
				{
					lock = false;
					lastaimentity = 0;
					last_locked_entity = 0;
					aimentity = 0;
					continue;
				}

				if (lock_target)
					lock = true;
				else
					lock = false;

				lastaimentity = aimentity;

				if (aimentity != last_locked_entity) {
					last_locked_entity = aimentity;
					lock_start_time = std::chrono::steady_clock::now();
				}

				bool is_zooming = LPlayer.isZooming();
				float current_smooth = is_zooming ? ads_smooth : hip_smooth;
				float current_max_fov = is_zooming ? ads_fov : hip_fov;

				auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - lock_start_time).count();
				if (elapsed < 500) {
					current_smooth *= 2.0f;
				}


				float fov = CalculateFov(LPlayer, Target);
				float dist = LPlayer.getPosition().DistTo(Target.getPosition());
				float active_dist = (aassist && aassist_aiming) ? aassist_dist : aim_dist;

				if (fov > current_max_fov || dist > active_dist)
				{
					if (!shooting) {
						lock = false;
						lastaimentity = 0;
						last_locked_entity = 0;
						aimentity = 0;
					}
					continue;
				}

				if (aim == 2 && !is_aimentity_visible)
				{
					continue;
				}

				QAngle Angles = CalculateBestBoneAim(LPlayer, aimentity, current_max_fov, current_smooth);
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

static void set_vars(uint64_t add_addr)
{
	printf("Reading client vars...\n");
	std::this_thread::sleep_for(std::chrono::milliseconds(50));
	//Get addresses of client vars
uint64_t check_addr = 0;
client_mem.Read<uint64_t>(add_addr, check_addr);

uint64_t aim_addr = 0;  
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 1, aim_addr);

uint64_t esp_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 2, esp_addr);

uint64_t aiming_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 3, aiming_addr);

uint64_t g_Base_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 4, g_Base_addr);

uint64_t next_addr = 0;  
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 5, next_addr);

uint64_t player_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 6, player_addr);

uint64_t valid_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 7, valid_addr);

uint64_t max_dist_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 8, max_dist_addr);

uint64_t item_glow_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 9, item_glow_addr);

uint64_t aim_no_recoil_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 10, aim_no_recoil_addr);

uint64_t bone_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 11, bone_addr);

uint64_t spectators_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 12, spectators_addr);

uint64_t allied_spectators_addr = 0;  
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 13, allied_spectators_addr);

uint64_t glowr_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 14, glowr_addr);

uint64_t glowg_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 15, glowg_addr);

uint64_t glowb_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 16, glowb_addr);

uint64_t glowrviz_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 17, glowrviz_addr);

uint64_t glowgviz_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 18, glowgviz_addr);

uint64_t glowbviz_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 19, glowbviz_addr);

uint64_t glowrknocked_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 20, glowrknocked_addr);

uint64_t glowgknocked_addr = 0;  
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 21, glowgknocked_addr);

uint64_t glowbknocked_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 22, glowbknocked_addr);

uint64_t firing_range_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 23, firing_range_addr);

uint64_t shooting_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 24, shooting_addr);

uint64_t onevone_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 25, onevone_addr);

uint64_t spec_list_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 26, spec_list_addr);

uint64_t dump_req_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 27, dump_req_addr);

uint64_t update_req_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 28, update_req_addr);

uint64_t skeleton_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 29, skeleton_addr);

uint64_t screen_width_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 30, screen_width_addr);

uint64_t screen_height_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 31, screen_height_addr);

uint64_t triggerbot_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 32, triggerbot_addr);

uint64_t triggerbot_key_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 33, triggerbot_key_addr);

uint64_t triggerbot_aiming_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 34, triggerbot_aiming_addr);

uint64_t superglide_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 35, superglide_addr);

uint64_t bhop_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 36, bhop_addr);

uint64_t walljump_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 37, walljump_addr);

uint64_t superkey_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 38, superkey_addr);

uint64_t triggerbot_fov_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 39, triggerbot_fov_addr);

uint64_t lock_target_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 40, lock_target_addr);

uint64_t player_glow_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 41, player_glow_addr);

uint64_t aim_dist_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 42, aim_dist_addr);

uint64_t insidevalue_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 43, insidevalue_addr);

uint64_t outlinesize_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 44, outlinesize_addr);

uint64_t ads_fov_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 45, ads_fov_addr);

uint64_t ads_smooth_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 46, ads_smooth_addr);

uint64_t hip_fov_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 47, hip_fov_addr);

uint64_t hip_smooth_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 48, hip_smooth_addr);

uint64_t skeleton_thickness_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 49, skeleton_thickness_addr);

uint64_t aassist_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 50, aassist_addr);

uint64_t aassist_dist_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 51, aassist_dist_addr);

uint64_t aassist_aiming_addr = 0;
client_mem.Read<uint64_t>(add_addr + sizeof(uint64_t) * 52, aassist_aiming_addr);

uint32_t check = 0;
client_mem.Read<uint32_t>(check_addr, check);

if (check != 0xABCD)
{
    printf("Incorrect values read. Check if the add_off is correct. Quitting.\n");
    active = false;
    return;
}

bool new_client = true;
vars_t = true;

while (vars_t)
{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (new_client && c_Base != 0 && g_Base != 0)
		{
			client_mem.Write<uint32_t>(check_addr, 0);
			new_client = false;
			printf("\nReady\n");
		}

    while (c_Base != 0 && g_Base != 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if (g_Base_addr) client_mem.Write<uint64_t>(g_Base_addr, g_Base);
        if (spectators_addr) client_mem.Write<int>(spectators_addr, spectators);
        if (allied_spectators_addr) client_mem.Write<int>(allied_spectators_addr, allied_spectators);

        if (aim_addr) client_mem.Read<int>(aim_addr, aim);
        if (esp_addr) client_mem.Read<bool>(esp_addr, esp);
        if (aiming_addr) client_mem.Read<bool>(aiming_addr, aiming);
        if (max_dist_addr) client_mem.Read<float>(max_dist_addr, max_dist);
        if (item_glow_addr) client_mem.Read<bool>(item_glow_addr, item_glow);
        if (player_glow_addr) client_mem.Read<bool>(player_glow_addr, player_glow);
        if (aim_no_recoil_addr) client_mem.Read<bool>(aim_no_recoil_addr, aim_no_recoil);
        if (bone_addr) client_mem.Read<int>(bone_addr, bone);
        if (glowr_addr) client_mem.Read<float>(glowr_addr, glowr);
        if (glowg_addr) client_mem.Read<float>(glowg_addr, glowg);
        if (glowb_addr) client_mem.Read<float>(glowb_addr, glowb);
        if (glowrviz_addr) client_mem.Read<float>(glowrviz_addr, glowrviz);
        if (glowgviz_addr) client_mem.Read<float>(glowgviz_addr, glowgviz);
        if (glowbviz_addr) client_mem.Read<float>(glowbviz_addr, glowbviz);
        if (glowrknocked_addr) client_mem.Read<float>(glowrknocked_addr, glowrknocked);
        if (glowgknocked_addr) client_mem.Read<float>(glowgknocked_addr, glowgknocked);
        if (glowbknocked_addr) client_mem.Read<float>(glowbknocked_addr, glowbknocked);
        if (firing_range_addr) client_mem.Read<bool>(firing_range_addr, firing_range);
        if (onevone_addr) client_mem.Read<bool>(onevone_addr, onevone);

        if (skeleton_addr)
            client_mem.Read<bool>(skeleton_addr, skeleton);

        if (dump_req_addr) {
            bool dump_req = false;
            client_mem.Read<bool>(dump_req_addr, dump_req);
            if (dump_req)
            {
                printf("Dump requested\n");
                if (apex_mem.Dump("dump.bin"))
                {
                    printf("Running r5dumper...\n");
                    system("cd r5dumper && ./r5dumper ../dump.bin");
                }
                client_mem.Write<bool>(dump_req_addr, false);
            }
        }

        if (update_req_addr) {
            bool update_req = false;
            client_mem.Read<bool>(update_req_addr, update_req);
            if (update_req)
            {
                printf("Update offsets requested\n");
                load_offsets_from_ini("r5dumper/_offsets.ini", "r5dumper/_convars.ini", "r5dumper/_buttons.ini");
                client_mem.Write<bool>(update_req_addr, false);
            }
        }

        if (spec_list_addr) client_mem.WriteArray<spectator>(spec_list_addr, spectator_list, toRead);

        if (screen_width_addr)
            client_mem.Read<int>(screen_width_addr, screen_width);

        if (screen_height_addr)
            client_mem.Read<int>(screen_height_addr, screen_height);

        if (triggerbot_addr) client_mem.Read<bool>(triggerbot_addr, triggerbot);

        if (triggerbot_aiming_addr) client_mem.Read<bool>(triggerbot_aiming_addr, triggerbot_aiming);

        if (superglide_addr) client_mem.Read<bool>(superglide_addr, superglide);

        if (bhop_addr) client_mem.Read<bool>(bhop_addr, bhop);

        if (walljump_addr) client_mem.Read<bool>(walljump_addr, walljump);

        if (lock_target_addr) client_mem.Read<bool>(lock_target_addr, lock_target);

        if (superkey_addr) client_mem.Read<int>(superkey_addr, SuperKey);

        if (triggerbot_fov_addr) client_mem.Read<float>(triggerbot_fov_addr, triggerbot_fov);

        if (aim_dist_addr) client_mem.Read<float>(aim_dist_addr, aim_dist);

        if (insidevalue_addr) client_mem.Read<unsigned char>(insidevalue_addr, insidevalue);

        if (outlinesize_addr) client_mem.Read<unsigned char>(outlinesize_addr, outlinesize);

        if (ads_fov_addr) client_mem.Read<float>(ads_fov_addr, ads_fov);

        if (ads_smooth_addr) client_mem.Read<float>(ads_smooth_addr, ads_smooth);

        if (hip_fov_addr) client_mem.Read<float>(hip_fov_addr, hip_fov);

        if (hip_smooth_addr) client_mem.Read<float>(hip_smooth_addr, hip_smooth);

        if (skeleton_thickness_addr) client_mem.Read<float>(skeleton_thickness_addr, skeleton_thickness);

        if (aassist_addr) client_mem.Read<bool>(aassist_addr, aassist);
        if (aassist_dist_addr) client_mem.Read<float>(aassist_dist_addr, aassist_dist);
        if (aassist_aiming_addr) client_mem.Read<bool>(aassist_aiming_addr, aassist_aiming);

        if (esp && next)
        {
            if (valid)
                client_mem.WriteArray<player>(player_addr, players, toRead);
            client_mem.Write<bool>(valid_addr, valid);
            client_mem.Write<bool>(next_addr, true); //next

            bool next_val = false;
            do
            {
                client_mem.Read<bool>(next_addr, next_val);
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            } while (next_val && g_Base != 0 && c_Base != 0);

            next = false;
        }
    }
}
vars_t = false;
}

// Item Glow Stuff
void item_glow_t()
{
	item_t = true;
	bool last_item_glow = false;

	while (item_t)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		if (g_Base == 0 || c_Base == 0) continue;

		if (item_glow)
		{
			last_item_glow = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));

			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
			uint64_t highlightSettingsPtr = 0;
			apex_mem.Read<uint64_t>(g_Base + HIGHLIGHT_SETTINGS, highlightSettingsPtr);
			if (highlightSettingsPtr == 0) continue;

			// Define all required highlight IDs
			const std::vector<uint8_t> itemHighlightIDs = {
				COMMON_GLOW_ID, RARE_GLOW_ID, EPIC_GLOW_ID,
				MYTHIC_GLOW_ID, LEGENDARY_GLOW_ID, WEAPON_GLOW_ID, AMMO_GLOW_ID,
				37, 32, 65, 51, 4, 53, 81, 80, 33, 34 // Also include IDs from items.ini just in case
			};
			const GlowMode itemGlowMode = { 137, 138, 64, 64 };

			for (uint8_t id : itemHighlightIDs)
			{
				GlowMode currentGlowMode;
				apex_mem.Read<GlowMode>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * id + 0, currentGlowMode);
				if (currentGlowMode != itemGlowMode)
					apex_mem.Write<GlowMode>(highlightSettingsPtr + HIGHLIGHT_TYPE_SIZE * id + 0, itemGlowMode);
			}

			for (int i = 0; i < 10000; i++)
			{
				uint64_t centity = 0;
				apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
				if (centity == 0) continue;

				Item item = getItem(centity);
				if (item.isItem() || item.isBox())
				{
					if (!item.isGlowing())
					{
						uint8_t highlightID = 65; // Default

						if (item.isBox())
						{
							highlightID = LEGENDARY_GLOW_ID; // Orange for boxes
						}
						else
						{
							char modelName[256] = { 0 };
							item.getModelName(modelName, 256);
							std::string itemName;
							ItemCategory category = ItemCategory::OTHER;
							if (ItemManager::getInstance().GetItemInfo(modelName, itemName, category)) {
								switch (category) {
									case ItemCategory::COMMON: highlightID = COMMON_GLOW_ID; break;
									case ItemCategory::RARE: highlightID = RARE_GLOW_ID; break;
									case ItemCategory::EPIC: highlightID = EPIC_GLOW_ID; break;
									case ItemCategory::LEGENDARY: highlightID = LEGENDARY_GLOW_ID; break;
									case ItemCategory::MYTHIC: highlightID = MYTHIC_GLOW_ID; break;
									case ItemCategory::WEAPON: highlightID = WEAPON_GLOW_ID; break;
									case ItemCategory::AMMO: highlightID = AMMO_GLOW_ID; break;
									default: highlightID = 65; break;
								}
							} else {
								// Fallback to scriptInt if not found by model
								uint32_t scriptInt = 0;
								apex_mem.Read<uint32_t>(centity + OFFSET_M_CUSTOMSCRIPTINT, scriptInt);

								if (ItemManager::getInstance().GetItemInfoByID(scriptInt, itemName, category)) {
									switch (category) {
										case ItemCategory::COMMON: highlightID = COMMON_GLOW_ID; break;
										case ItemCategory::RARE: highlightID = RARE_GLOW_ID; break;
										case ItemCategory::EPIC: highlightID = EPIC_GLOW_ID; break;
										case ItemCategory::LEGENDARY: highlightID = LEGENDARY_GLOW_ID; break;
										case ItemCategory::MYTHIC: highlightID = MYTHIC_GLOW_ID; break;
										case ItemCategory::WEAPON: highlightID = WEAPON_GLOW_ID; break;
										case ItemCategory::AMMO: highlightID = AMMO_GLOW_ID; break;
										default: highlightID = 65; break;
									}
								} else {
									// Hardcoded legacy fallback
									if (scriptInt == 1) highlightID = COMMON_GLOW_ID;
									else if (scriptInt == 2) highlightID = RARE_GLOW_ID;
									else if (scriptInt == 3) highlightID = EPIC_GLOW_ID;
									else if (scriptInt == 4) highlightID = LEGENDARY_GLOW_ID;
									else if (scriptInt >= 5) highlightID = MYTHIC_GLOW_ID;
								}
							}
						}
						item.enableGlow(highlightID);
					}
				}
			}
		}
		else if (last_item_glow)
		{
			// Disable cleanup
			uint64_t entitylist = g_Base + OFFSET_ENTITYLIST;
			for (int i = 0; i < 10000; i++)
			{
				uint64_t centity = 0;
				apex_mem.Read<uint64_t>(entitylist + ((uint64_t)i << 5), centity);
				if (centity == 0) continue;

				Item item = getItem(centity);
				if ((item.isItem() || item.isBox()) && item.isGlowing())
				{
					item.disableGlow();
				}
			}
			last_item_glow = false;
		}
	}
	item_t = false;
}


int main(int argc, char *argv[])
{
	mf_log_init(LevelFilter::LevelFilter_Info);

	if(geteuid() != 0)
	{
		printf("Error: %s is not running as root\n", argv[0]);
		return 0;
	}

	const char* cl_proc = "Client.exe";
	const char* ap_proc = "r5apex_dx12.exe";
	//const char* ap_proc = "EasyAntiCheat_launcher.exe";

	//Client "add" offset
	uint64_t add_off = 0x000000;
	std::thread aimbot_thr;
	std::thread esp_thr;
	std::thread actions_thr;
	std::thread itemglow_thr;
	std::thread stuffbot_thr;

	std::thread vars_thr;
	bool proc_not_found = false;
	while (active)
	{
		if (apex_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (aim_t)
			{
				aim_t = false;
				esp_t = false;
				actions_t = false;
				item_t = false;
				extern bool stuff_t;
				stuff_t = false;
				g_Base = 0;

				if (aimbot_thr.joinable()) aimbot_thr.detach();
				if (esp_thr.joinable()) esp_thr.detach();
				if (actions_thr.joinable()) actions_thr.detach();
				if (itemglow_thr.joinable()) itemglow_thr.detach();
				if (stuffbot_thr.joinable()) stuffbot_thr.detach();

				aimbot_thr = std::thread();
				esp_thr = std::thread();
				actions_thr = std::thread();
				itemglow_thr = std::thread();
				stuffbot_thr = std::thread();
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
				esp_thr = std::thread(EspLoop);
				actions_thr = std::thread(DoActions);
				stuffbot_thr = std::thread(StuffBotLoop);
				itemglow_thr = std::thread(item_glow_t);
				aimbot_thr.detach();
				esp_thr.detach();
				actions_thr.detach();
				stuffbot_thr.detach();
				itemglow_thr.detach();
			}
		}
		else
		{
			apex_mem.check_proc();
		}

		if (client_mem.get_proc_status() != process_status::FOUND_READY)
		{
			if (vars_t)
			{
				vars_t = false;
				c_Base = 0;

				if (vars_thr.joinable()) vars_thr.detach();
				vars_thr = std::thread();
			}
			
			std::this_thread::sleep_for(std::chrono::seconds(1));
			printf("Searching for client process...\n");

			client_mem.open_proc(cl_proc);

			if (client_mem.get_proc_status() == process_status::FOUND_READY)
			{
				c_Base = client_mem.get_proc_baseaddr();
				printf("\nClient process found\n");
				printf("Base: %lx\n", c_Base);

				vars_thr = std::thread(set_vars, c_Base + add_off);
				vars_thr.detach();
			}
		}
		else
		{
			client_mem.check_proc();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}
