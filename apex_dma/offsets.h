#pragma once
#include "offsets_dynamic.h"

#define GameVersion v0.3.24 //[Miscellaneous].GameVersion updated 2026/02/11
#define OFFSET_ENTITYLIST 0x644C0F8 //[Miscellaneous].cl_entitylist updated 2026/02/18
#define OFFSET_LOCAL_ENT 0x282C358 //[Miscellaneous].LocalPlayer updated 2026/02/18
#define OFFSET_LOCAL_ENTITY_HANDLE 0x2664F10 //[Miscellaneous].LocalEntityHandle updated 2026/02/18
#define OFFSET_NAME_LIST 0x8E46600 //[Miscellaneous].NameList updated 2026/02/18
#define OFFSET_GLOBAL_VARS 0x1F7A5B0 //[Miscellaneous].GlobalVars updated 2026/02/18
 
#define OFFSET_LEVELNAME 0x1F7AA34 //[Miscellaneous].LevelName updated 2026/02/18
#define OFFSET_CLIENTSTATE OFFSET_LEVELNAME - 0x1c0 //ClientState  ClientState = LevelName - 0x1C0  //updated 1/10/2024
//#define OFFSET_CLIENTSTATE 0x8901280 //[Miscellaneous].ClientState updated 2026/02/11
#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE + 0xAC //SignonState (ClientState + 0xAC)  //updated 1/10/2024
//#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE 0x16d699c
#define OFFSET_HOST_MAP 0x01e93dd0 + 0x58 //[ConVars].host_map + 0x58 updated 2026/02/11
//#define OFFSET_GAME_MODE 0x2867148 //[ConVars]mp_gamemode + 0x48
 
#define OFFSET_TEAM 0x0334 //[RecvTable.DT_BaseEntity].m_iTeamNum updated 2026/02/07
#define OFFSET_HEALTH 0x0324 //0x950 //[RecvTable.DT_Player].m_iHealth updated updated 2026/02/11
#define OFFSET_MAXHEALTH 0x0468 //[RecvTable.DT_Player].m_iMaxHealth

#define OFFSET_SHIELD 0x01a0 //[RecvTable.DT_BaseEntity].m_shieldHealth updated 2024/09/30
#define OFFSET_MAXSHIELD 0x1A4 //[RecvTable.DT_BaseEntity].m_shieldHealthMax updated 2024/09/30
#define OFFSET_ARMORTYPE 0x4814 //[RecvTable.DT_Player].m_armorType updated updated 2026/02/11
#define OFFSET_NAME 0x0479 //[RecvTable.DT_BaseEntity].m_iName updated 2025/02/25
#define OFFSET_SIGN_NAME 0x0470 //[RecvTable.DT_BaseEntity].m_iSignifierName updated 2025/02/25
#define OFFSET_ABS_VELOCITY 0x0170 //[DataMap.DT_BaseEntity].m_vecAbsVelocity updated 2024/09/18
//#define OFFSET_VISIBLE_TIME 0x1A64 //[Miscellaneous].CPlayer!lastVisibleTime updated 2026/02/18
//#define OFFSET_LAST_AIMEDAT_TIME 0x1A6C //0x1a5c   //[Miscellaneous].CPlayer!lastVisibleTime + 0x8
#define OFFSET_VISIBLE_TIME     0x1a64 //[DataMap.C_BaseCombatCharacter]m_hudInfo_visibilityTestAlwaysPasses + 0x02 updated 2026/02/18
#define OFFSET_LAST_AIMEDAT_TIME       0x8 //[Miscellaneous]CWeaponX!lastCrosshairTargetTime (CPlayer!lastVisibleTime relative)
#define OFFSET_ZOOMING 0x1cb1 //[RecvTable.DT_Player].m_bZooming updated 2026/02/11

#define OFFSET_VIEW_OFFSET 0x00e8 //[DataMap.DT_BaseEntity].m_currentFrame.viewOffset updated 2024/09/18
#define OFFSET_ACTIVE_WEAPON 0x1958 + 0x0058 //[RecvTable.DT_Player].m_inventory + WeaponInventory_Client>activeWeapons updated 2026/02/11

#define OFFSET_SPECTATOR_LIST OFFSET_OBSERVER_LIST //? updated 2024/11/15
#define OFFSET_OBSERVER_LIST 0x644e118 //[ConVars].gamepad_aim_assist_melee + 0x20C8 updated 2026/02/18
#define OFFSET_OBSERVER_ARRAY 0x964 //[DataMapTypes.DT_GlobalNonRewinding].m_playerObserver - [DataMap.C_ObserverMode].m_observerTarget updated 2024/09/20

#define OFFSET_IN_DUCKSTATE 0x2abc //[RecvTable.DT_Player].m_duckState updated 2026/02/11

#define OFFSET_IN_DUCK 0x03f39558 //[Buttons].in_duck updated 2026/02/20
#define OFFSET_TRAVERSAL_PROGRESS 0x2bcc //[DataMap.DT_Player].m_traversalProgress updated 2026/02/07
#define OFFSET_TRAVERSAL_STARTTIME 0x2bd4 //[DataMap.DT_Player].m_traversalStartTime updated 2026/02/07
#define OFFSET_TRAVERSAL_RELEASE_TIME 0x2bdc //[RecvTable.DT_LocalPlayerExclusive].m_traversalReleaseTime updated 2026/02/07

#define OFFSET_IN_JUMP 0x3f39460 //[Buttons].in_jump updated 2026/02/20
#define OFFSET_IN_TOGGLE_DUCK 0x03f39388 //[Buttons].in_toggle_duck updated 2026/02/20
 
#define OFFSET_WEAPON_NAME 0x1850 //?0x19c4 //[RecvTable.DT_WeaponX].m_weaponNameIndex updated 2026/02/18
#define OFFSET_OFF_WEAPON 0x19d4 //[DataMap.DT_BaseCombatCharacter].m_latestNonOffhandWeapons updated 2026/02/18
#define OFFSET_WALL_RUN_START_TIME 0x370c //[RecvTable.DT_LocalPlayerExclusive]->m_wallRunStartTime updated 2026/02/11
#define OFFSET_WALL_RUN_CLEAR_TIME 0x3710 //[RecvTable.DT_LocalPlayerExclusive]->m_wallRunClearTime updated 2026/02/11

#define OFFSET_FLAGS 0x00c8 //[DataMap.DT_Player].m_fFlags updated 2026/02/07
#define OFFSET_IN_ATTACK 0x03f38bd0 //[Buttons].in_attack updated 2026/02/20
#define OFFSET_IN_ZOOM 0x03f394e0 //[Buttons].in_zoom updated 2026/02/20
#define OFFSET_IN_FORWARD 0x03f39598 //[Buttons].in_forward updated 2026/02/20
#define OFFSET_IN_BACKWARD 0x03f395c0 //[Buttons].in_backward updated 2026/02/20
 
#define OFFSET_LIFE_STATE 0x0690 //[RecvTable.DT_Player].m_lifeState updated 2026/02/11
#define OFFSET_BLEED_OUT_STATE 0x27c0 //[RecvTable.DT_Player].m_bleedoutState updated 2026/02/11
 
#define OFFSET_ORIGIN 0x017c //[DataMap.DT_BaseEntity].m_vecAbsOrigin updated 2026/02/07
#define OFFSET_BONES 0x0db8 + 0x48 //[RecvTable.DT_BaseAnimating].m_nForceBone + 0x48 updated 2026/02/11
#define OFFSET_STUDIOHDR 0x1000 //[Miscellaneous].CBaseAnimating!m_pStudioHdr updated 2026/02/11
#define OFFSET_AIMPUNCH 0x2438 //[DataMap.DT_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2024/09/18
#define OFFSET_CAMERAPOS 0x1fc4 //[Miscellaneous].CPlayer!camera_origin updated 2026/02/11
#define OFFSET_VIEWANGLES 0x2614 - 0x104 //[RecvTable.DT_Player]m_ammoPoolCapacity - 0x104 updated 2026/02/11
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10 //updated 01/10/2024
#define OFFSET_OBSERVER_MODE 0x35e4 //[RecvTable.DT_LocalPlayerExclusive].m_iObserverMode updated 2025/02/25
#define OFFSET_OBSERVING_TARGET 0x35f0 //[RecvTable.DT_LocalPlayerExclusive].m_hObserverTarget updated 2025/02/25
#define OFFSET_IN_USE 0x03f392f0 //[Buttons].in_use updated 2026/02/11
 
#define OFFSET_MATRIX 0x11a350 //[Miscellaneous].ViewMatrix updated 2026/02/07
#define OFFSET_RENDER 0x3F362E0 //[Miscellaneous].ViewRender updated 2026/02/18
 
#define OFFSET_WEAPON 0x19c4 //[RecvTable.DT_BaseCombatCharacter].m_latestPrimaryWeapons updated 2026/02/11
#define OFFSET_BULLET_SPEED 0x2860 //[Miscellaneous].CWeaponX!m_flProjectileSpeed updated 2026/02/07
#define OFFSET_BULLET_SCALE OFFSET_BULLET_SPEED + 0x8 //[Miscellaneous].CWeaponX!m_flProjectileScale updated 2026/02/07
#define OFFSET_ZOOM_FOV 0x1724 //[DataMap.CWeaponX]m_playerData + [RecvTable.DT_WeaponPlayerData]m_targetZoomFOV updated 2025/02/25
#define OFFSET_AMMO 0x15e0 //[RecvTable.DT_PropSurvival].m_ammoInClip updated 2026/02/18
 
//#define OFFSET_ITEM_GLOW 0x02f0 //m_highlightFunctionBits
#define OFFSET_ITEM_ID 0x15e4 //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2026/02/11
#define OFFSET_MODELNAME 0x0030 //[DataMap.DT_BaseEntity].m_ModelName updated 2024/09/18
#define OFFSET_M_CUSTOMSCRIPTINT OFFSET_ITEM_ID //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2026/02/11
#define OFFSET_YAW 0x231c - 0x8 //[DataMap.DT_Player].m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2025/02/12
 
//#define OFFSET_GLOW_T1 0x292 //16256 = enabled, 0 = disabled
//#define OFFSET_GLOW_T2 0x30c //1193322764 = enabled, 0 = disabled
#define OFFSET_GLOW_ENABLE 0x0298 //0x0299 - 0x1 //0x299 //[RecvTable.DT_HighlightSettings].m_highlightGenericContexts - 0x1 updated 2026/02/11
#define OFFSET_GLOW_THROUGH_WALLS 0x26c //- 0x21 //[RecvTable.DT_HighlightSettings.m_highlightGenericContexts - 0x21 updated 2024/07/23
 
#define OFFSET_TIME_BASE 0x2168 //[DataMap.DT_Player].m_currentFramePlayer.timeBase updated 2025/02/18
#define GLOW_START_TIME 0x02c8 + 0x30 //0x02c4 //m_playerFloatLookStartTime updated 01/10/2024
#define OFFSET_HIGHLIGHTSERVERACTIVESTATES 0x29C //[DT_HighlightSettings].m_highlightServerActiveStates updated 2024/06/26

#define OFFSET_GLOW_FIX 0x278 //0x268 //updated 2025/02/25
//#define GLOW_VISIBLE_TYPE 0x27c      // updated 6/26/2024
#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT OFFSET_GLOW_ENABLE // Script_Highlight_SetCurrentContext updated 01/10/2024
#define OFFSET_GLOW_CONTEXT_ID 0x29c                                            //updated 6/26/2024
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS // Script_Highlight_SetVisibilityType 5th mov updated 01/10/2024
#define GLOW_LIFE_TIME 0x3A4+ 0x30 // Script_Highlight_SetLifeTime + 4 updated 01/10/2024
#define GLOW_DISTANCE 0x264 //0x26c // Script_Highlight_SetFarFadeDist updated 01/10/2024
#define GLOW_TYPE 0x29c // Script_Highlight_GetState + 4 updated 01/10/2024
#define GLOW_COLOR 0x1D0+ 0x30 // Script_CopyHighlightState 15th mov updated 01/10/2024
#define GLOW_FADE 0x388+ 0x30 // ats 3rd result of 3 offsets consecutive or first + 8 updated 01/10/2024
#define HIGHLIGHT_SETTINGS 0x6B95E00 //[Miscellaneous].HighlightSettings updated 2026/02/18
#define HIGHLIGHT_TYPE_SIZE 0x34 //? updated 01/10/2024
#define OFFSET_CROSSHAIR_LAST OFFSET_VISIBLE_TIME + OFFSET_LAST_AIMEDAT_TIME //[Miscellaneous].CWeaponX!lastCrosshairTargetTime updated 2026/02/18
//#define OFFSET_CROSSHAIR_START 0x1958 //CPlayer!crosshairTargetStartTime updated 01/9/2024
#define OFFSET_INPUT_SYSTEM 0x2034500 //[Miscellaneous].InputSystem updated 2026/02/18 

#define OFFSET_SKYDIVE_STATE 0x4878 //[RecvTable.DT_Player].m_skydiveState updated 2026/02/11
#define OFFSET_GRAPPLEACTIVED       0x2da8 //[RecvTable.DT_Player].m_grappleActive updated 2026/02/11
#define OFFSET_GRAPPLE              0x2d20 //[RecvTable.DT_Player].m_grapple updated 2026/02/11
#define OFFSET_GRAPPLEATTACHED      0x48 //[RecvTable.DT_GrappleData].m_grappleAttached updated 2026/02/07
#define OFFSET_m_xp		    0x3824 //[RecvTable.DT_Player].m_xp updated 2026/02/11
#define OFFSET_GRADE 0x0348 //[RecvTable.DT_BaseEntity].m_grade updated 2025/02/25
