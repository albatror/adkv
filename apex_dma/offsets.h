#pragma once
#include "offsets_dynamic.h"

#define GameVersion v3.0.2.49 //[Miscellaneous].GameVersion updated 2026/04/14
#define OFFSET_ENTITYLIST 0x6266928 //[Miscellaneous].cl_entitylist updated 2026/05/05
#define OFFSET_LOCAL_ENT 0x268fa08 //[Miscellaneous].LocalPlayer updated 2026/05/05
#define OFFSET_LOCAL_ENTITY_HANDLE 0x251E5D0 //[Miscellaneous].localplayerHandle updated 2026/04/14
#define OFFSET_NAME_LIST 0x8c62ea0 //[Miscellaneous].NameList updated 2026/05/05
#define OFFSET_GLOBAL_VARS 0x1e3abf0 //[Miscellaneous].GlobalVars updated 2026/05/05
 
#define OFFSET_LEVELNAME 0x1e3b074 //[Miscellaneous].LevelName updated 2026/05/05
#define OFFSET_CLIENTSTATE 0x3090a0 //OFFSET_LEVELNAME - 0x1c0 //ClientState  ClientState = LevelName - 0x1C0  //updated 1/10/2024
//#define OFFSET_CLIENTSTATE 0x2faa40 //[Miscellaneous].ClientState updated 2026/05/05
#define OFFSET_SIGNONSTATE 0x1d0bc1c //OFFSET_CLIENTSTATE + 0xAC //SignonState (ClientState + 0xAC)  //updated 1/10/2024
//#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE 0x16d699c
#define OFFSET_HOST_MAP 0x1b7c450 + 0x58 //[ConVars].host_map + 0x58 updated 2026/05/05
//#define OFFSET_GAME_MODE  0x26ca698 #[ConVars]mp_gamemode + 0x48
 
#define OFFSET_TEAM 0x334 //[RecvTable.DT_BaseEntity].m_iTeamNum updated 2026/05/05
#define OFFSET_HEALTH 0x324 //0x950 //[RecvTable.DT_Player].m_iHealth updated updated 2026/05/05
#define OFFSET_MAXHEALTH 0x468 //[RecvTable.DT_Player].m_iMaxHealth updated updated 2026/05/05

#define OFFSET_SHIELD 0x1a0 //[RecvTable.DT_BaseEntity].m_shieldHealth updated 2026/05/05
#define OFFSET_MAXSHIELD 0x1a4 //[RecvTable.DT_BaseEntity].m_shieldHealthMax updated 2026/05/05
#define OFFSET_ARMORTYPE 0x487c //[RecvTable.DT_Player].m_armorType updated updated 2026/05/05
#define OFFSET_NAME 0x479 //[RecvTable.DT_BaseEntity].m_iName updated 2026/05/05
#define OFFSET_SIGN_NAME 0x470 //[RecvTable.DT_BaseEntity].m_iSignifierName updated 2026/05/05
#define OFFSET_ABS_VELOCITY 0x170 //[DataMap.DT_BaseEntity].m_vecAbsVelocity updated 2026/05/05
#define OFFSET_VISIBLE_TIME 0x1a72 + 0x02 //[DataMap.C_BaseCombatCharacter]m_hudInfo_visibilityTestAlwaysPasses + 0x02
#define OFFSET_LAST_AIMEDAT_TIME  OFFSET_VISIBLE_TIME + 0x8 //[Miscellaneous]CWeaponX!lastCrosshairTargetTime (CPlayer!lastVisibleTime relative)
#define OFFSET_ZOOMING 0x1cc1 //[RecvTable.DT_Player].m_bZooming updated 2026/05/05

#define OFFSET_VIEW_OFFSET 0xe8 //[DataMap.DT_BaseEntity].m_currentFrame.viewOffset updated 2026/05/05

#define OFFSET_SPECTATOR_LIST OFFSET_OBSERVER_LIST //? updated 2024/11/15
#define OFFSET_OBSERVER_LIST 0x6268948 //[Miscellaneous].observerList updated 2026/05/05
#define OFFSET_OBSERVER_ARRAY 0x964 //[RecvTable.DT_GlobalNonRewinding].m_gameTimescale updated 2026/05/05

#define OFFSET_IN_DUCKSTATE 0x2ae4 //[RecvTable.DT_Player].m_duckState updated 2026/05/05

#define OFFSET_IN_DUCK 0x3d9b008 //[Buttons].in_duck updated 2026/05/05
#define OFFSET_TRAVERSAL_PROGRESS 0x2bf4 //[DataMap.DT_Player].m_traversalProgress updated 2026/05/05
#define OFFSET_MANTLE_BOOST_STATE 0x2c2c //[RecvTable.DT_Player].m_mantleBoostState
#define OFFSET_TRAVERSAL_STARTTIME 0x2bfc //[DataMap.DT_Player].m_traversalStartTime updated 2026/05/05
#define OFFSET_TRAVERSAL_RELEASE_TIME 0x2c04 //[RecvTable.DT_LocalPlayerExclusive].m_traversalReleaseTime updated 2026/05/05

#define OFFSET_IN_JUMP 0x3d9af10 //[Buttons].in_jump updated 2026/05/05
#define OFFSET_IN_TOGGLE_DUCK 0x3d9ae28 //[Buttons].in_toggle_duck updated 2026/05/05

#define OFFSET_WEAPON 0x1888 //[RecvTable.DT_WeaponX].m_weaponNameIndex updated 2026/05/05
#define OFFSET_ACTIVE_WEAPON 0x1968 + 0x58 //[RecvTable.DT_Player].m_inventory + [DataMap.WeaponInventory_Client]activeWeapons updated 2026/05/05
#define OFFSET_WEAPON_NAME 0x1888 //[Miscellaneous].WeaponNames updated 2026/05/11
#define OFFSET_OFF_WEAPON 0x19e4 //[DataMap.DT_BaseCombatCharacter].m_latestNonOffhandWeapons updated 2026/05/05
#define OFFSET_WALL_RUN_START_TIME 0x3734 //[RecvTable.DT_LocalPlayerExclusive].m_wallRunStartTime updated 2026/05/05
#define OFFSET_WALL_RUN_CLEAR_TIME 0x3738 //[RecvTable.DT_LocalPlayerExclusive].m_wallRunClearTime updated 2026/05/05

//#OFFSET_GRENADE_HANDLE 0x19e4 #[RecvTable.DT_BaseCombatCharacter]m_latestNonOffhandWeapons

#define OFFSET_FLAGS 0xc8 //[DataMap.DT_Player].m_fFlags updated 2026/05/05
#define OFFSET_IN_ATTACK 0x3d9a658 //[Buttons].in_attack updated 2026/05/05
#define OFFSET_IN_ZOOM 0x3d9afa0 //[Buttons].in_zoom updated 2026/05/05
#define OFFSET_IN_FORWARD 0x3d9b048 //[Buttons].in_forward updated 2026/05/05
#define OFFSET_IN_BACKWARD 0x3d9b070 //[Buttons].in_backward updated 2026/05/05
 
#define OFFSET_LIFE_STATE 0x690 //[RecvTable.DT_Player].m_lifeState updated 2026/05/05
#define OFFSET_BLEED_OUT_STATE 0x27e0 //[RecvTable.DT_Player].m_bleedoutState updated 2026/05/05
 
#define OFFSET_ORIGIN 0x17c //[DataMap.DT_BaseEntity].m_vecAbsOrigin updated 2026/05/05
#define OFFSET_BONES 0xdb8 + 0x48 //[RecvTable.DT_BaseAnimating].m_nForceBone + 0x48 updated 2026/05/05
#define OFFSET_STUDIOHDR 0x1000 //[Miscellaneous].CBaseAnimating!m_pStudioHdr updated 2026/05/05
#define OFFSET_AIMPUNCH 0x2528 //[DataMap.DT_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2026/05/05
#define OFFSET_CAMERAPOS 0x1fd4 //[Miscellaneous].CPlayer!camera_origin updated 2026/05/05
#define OFFSET_VIEWANGLES 0x2614 - 0x104 //[RecvTable.DT_Player]m_ammoPoolCapacity - 0x104 updated 2026/05/05
#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10 //updated 01/10/2024
#define OFFSET_OBSERVER_MODE 0x361c //[RecvTable.DT_LocalPlayerExclusive].m_iObserverMode updated 2026/05/05
#define OFFSET_OBSERVING_TARGET 0x3628 //[RecvTable.DT_LocalPlayerExclusive].m_hObserverTarget updated 2026/05/05
#define OFFSET_IN_USE 0x3d9af80 //[Buttons].in_use updated 2026/05/05
 
#define OFFSET_MATRIX 0x11a350 //[Miscellaneous].ViewMatrix updated 2026/05/05
#define OFFSET_RENDER 0x3d97d78 //[Miscellaneous].ViewRender updated 2026/05/05
 
#define OFFSET_BULLET_SPEED 0x1af8 + 0xd80 //[Miscellaneous].WeaponSettingsMeta_base + [weaponSettings].projectile_launch_speed updated 2026/05/05
#define OFFSET_BULLET_SCALE OFFSET_BULLET_SPEED + 0x08 //0x1af8 + 0xd88 //[Miscellaneous].WeaponSettingsMeta_base + [weaponSettings].projectile_gravity_scale updated 2026/05/05

#define OFFSET_ZOOM_FOV 0x1724 //[DataMap.CWeaponX]m_playerData + [RecvTable.DT_WeaponPlayerData]m_targetZoomFOV updated 2025/02/25
#define OFFSET_AMMO 0x15e0 //[RecvTable.DT_PropSurvival].m_ammoInClip updated 2026/05/05
 
//#define OFFSET_ITEM_GLOW 0x02f0 //m_highlightFunctionBits
#define OFFSET_ITEM_ID 0x15e4 //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2026/05/05
#define OFFSET_MODELNAME 0x30 //[DataMap.DT_BaseEntity].m_ModelName updated 2026/05/05
#define OFFSET_M_CUSTOMSCRIPTINT OFFSET_ITEM_ID //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2026/05/05
#define OFFSET_YAW 0x231c - 0x8 //[DataMap.C_Player].m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2026/05/05
 
//#define OFFSET_GLOW_T1 0x292 //16256 = enabled, 0 = disabled
//#define OFFSET_GLOW_T2 0x30c //1193322764 = enabled, 0 = disabled
#define OFFSET_GLOW_ENABLE 0x299 - 0x1 //[RecvTable.DT_HighlightSettings].m_highlightGenericContexts - 0x1 updated 2026/05/05
#define OFFSET_GLOW_THROUGH_WALLS 0x26c //- 0x21 //[RecvTable.DT_HighlightSettings].m_highlightGenericContexts - 0x21 updated 2026/05/05
 
#define OFFSET_TIME_BASE 0x2178 //[DataMap.DT_Player].m_currentFramePlayer.timeBase updated 2026/05/05
#define GLOW_START_TIME 0x2c4 + 0x30 //0x02c8 + 0x30 //0x2c4 + 0x30 //m_playerFloatLookStartTime updated 2026/04/14
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
#define HIGHLIGHT_SETTINGS 0x69b0600 //[Miscellaneous].HighlightSettings updated 2026/04/14
#define HIGHLIGHT_TYPE_SIZE 0x34 //? updated 01/10/2024
#define OFFSET_CROSSHAIR_LAST OFFSET_VISIBLE_TIME + 0x8 //[Miscellaneous].CWeaponX!lastCrosshairTargetTime updated 2025/04/18
//#define OFFSET_CROSSHAIR_START 0x1958 //CPlayer!crosshairTargetStartTime updated 01/9/2024
#define OFFSET_INPUT_SYSTEM 0x1ef3f00 //[Miscellaneous].InputSystem updated 2026/05/05 

#define OFFSET_SKYDIVE_STATE 0x48e0 //[RecvTable.DT_Player].m_skydiveState updated 2026/05/05
#define OFFSET_GRAPPLEACTIVED       0x2dd0 //[RecvTable.DT_Player].m_grappleActive updated 2026/05/05
#define OFFSET_GRAPPLE              0x2d48 //[RecvTable.DT_Player].m_grapple updated 2026/05/05
#define OFFSET_GRAPPLEATTACHED      0x48 //[RecvTable.DT_GrappleData].m_grappleAttached updated 2026/05/05
#define OFFSET_m_xp		    0x384c //[RecvTable.DT_Player].m_xp updated 2026/05/05
#define OFFSET_GRADE 0x344 //[RecvTable.DT_BaseEntity].m_grade updated 2026/05/05
#define OFFSET_PLATFORM 0x2630 //[DT_Player].m_hardware updated 2026/05/05
