#ifndef OFFSETS_H
#define OFFSETS_H

#include "offsets_dynamic.h"

#define GameVersion v3.0.3.24 //[Miscellaneous].GameVersion updated 2026/02/18
#define OFFSET_ENTITYLIST 0x644bf38 //[.Miscellaneous]cl_entitylist updated 2026/02/11
#define OFFSET_LOCAL_ENT 0x282c198 //[.Miscellaneous]LocalPlayer updated 2026/02/11
#define OFFSET_LOCAL_ENTITY_HANDLE 0x2017c68 //[.Miscellaneous]LocalEntityHandle updated 2026/02/07
#define OFFSET_NAME_LIST 0x8e46440 //[.Miscellaneous]NameList updated 2026/02/11
#define OFFSET_GLOBAL_VARS 0x1f7a5b0 //[.Miscellaneous]GlobalVars updated 2026/02/11

#define OFFSET_LEVELNAME 0x1f7aa34 //[.Miscellaneous]LevelName updated 2026/02/11
#define OFFSET_CLIENTSTATE (OFFSET_LEVELNAME - 0x1c0) //updated 2026/02/11
#define OFFSET_SIGNONSTATE (OFFSET_CLIENTSTATE + 0xAC) //updated 2026/02/11
#define OFFSET_HOST_MAP 0x1e94444  //[ConVars]host_map + 0x58 updated 2026/02/18

#define OFFSET_TEAM 0x335  //[RecvTable.DT_BaseEntity]m_iTeamNum updated 2026/02/18
#define OFFSET_HEALTH 0x0324 //[RecvTable.DT_Player]m_iHealth updated 2026/02/11
#define OFFSET_MAXHEALTH 0x0468 //[RecvTable.DT_Player]m_iMaxHealth updated 2026/02/11

#define OFFSET_SHIELD 0x01a0 //[RecvTable.DT_BaseEntity]m_shieldHealth updated 2024/09/30
#define OFFSET_MAXSHIELD 0x1A4 //[RecvTable.DT_BaseEntity]m_shieldHealthMax updated 2024/09/30
#define OFFSET_ARMORTYPE 0x4814 //[RecvTable.DT_Player]m_armorType updated 2026/02/11
#define OFFSET_NAME 0x0479 //[RecvTable.DT_BaseEntity]m_iName updated 2025/02/25
#define OFFSET_SIGN_NAME 0x0470 //[RecvTable.DT_BaseEntity]m_iSignifierName updated 2025/02/25
#define OFFSET_ABS_VELOCITY 0x0170 //[DataMap.DT_BaseEntity]m_vecAbsVelocity updated 2024/09/18
#define OFFSET_VISIBLE_TIME 0x1a70  //[.Miscellaneous]CPlayer!lastVisibleTime updated 2026/02/18
#define OFFSET_LAST_AIMEDAT_TIME (OFFSET_VISIBLE_TIME + 0x8) //updated 2025/04/18
#define OFFSET_ZOOMING 0x1cb1 //[RecvTable.DT_Player]m_bZooming updated 2026/02/11

#define OFFSET_VIEW_OFFSET 0x00e8 //[DataMap.DT_BaseEntity]m_currentFrame.viewOffset updated 2024/09/18
#define OFFSET_ACTIVE_WEAPON 0x1958 + 0x0058 //[RecvTable.DT_Player]m_inventory + 0x58 updated 2026/02/11

#define OFFSET_SPECTATOR_LIST OFFSET_OBSERVER_LIST
#define OFFSET_OBSERVER_LIST 0x644df58 + 0x20C8 //[ConVars]gamepad_aim_assist_melee + 0x20C8 updated 2026/02/11
#define OFFSET_OBSERVER_ARRAY 0x954 //[DataMapTypes.DT_GlobalNonRewinding]m_playerObserver updated 2024/09/20

#define OFFSET_IN_DUCKSTATE 0x2abc //[RecvTable.DT_Player]m_duckState updated 2026/02/11

#define OFFSET_IN_DUCK 0x03f39378 //[Buttons]+duck updated 2026/02/11
#define OFFSET_TRAVERSAL_PROGRESS 0x2bcc //[DataMap.DT_Player]m_traversalProgress updated 2026/02/07
#define OFFSET_TRAVERSAL_STARTTIME 0x2bd4 //[DataMap.DT_Player]m_traversalStartTime updated 2026/02/07
#define OFFSET_TRAVERSAL_RELEASE_TIME 0x2bdc //[RecvTable.DT_LocalPlayerExclusive]m_traversalReleaseTime updated 2026/02/07

#define OFFSET_IN_JUMP 0x3f39270 //[Buttons]+jump updated 2026/02/11
#define OFFSET_IN_TOGGLE_DUCK 0x03f391a8 //[Buttons]+toggle_duck updated 2026/02/11

#define OFFSET_WEAPON_NAME 0x15f0 //[RecvTable.DT_WeaponX]m_weaponNameIndex updated 2026/02/11
#define OFFSET_OFF_WEAPON 0x19d4 //[DataMap.DT_BaseCombatCharacter]m_latestNonOffhandWeapons updated 2026/02/11
#define OFFSET_WALL_RUN_START_TIME 0x370c //[RecvTable.DT_LocalPlayerExclusive]m_wallRunStartTime updated 2026/02/11
#define OFFSET_WALL_RUN_CLEAR_TIME 0x3710 //[RecvTable.DT_LocalPlayerExclusive]m_wallRunClearTime updated 2026/02/11

#define OFFSET_FLAGS 0x00c8 //[DataMap.DT_Player]m_fFlags updated 2026/02/07
#define OFFSET_IN_ATTACK 0x3f39999  //[Buttons]+attack updated 2026/02/18
#define OFFSET_IN_ZOOM 0x03f39300 //[Buttons]+zoom updated 2026/02/11
#define OFFSET_IN_FORWARD 0x03f393b8 //[Buttons]+forward updated 2026/02/11
#define OFFSET_IN_BACKWARD 0x03f393e0 //[Buttons]+backward updated 2026/02/11

#define OFFSET_LIFE_STATE 0x0690 //[RecvTable.DT_Player]m_lifeState updated 2026/02/11
#define OFFSET_BLEED_OUT_STATE 0x27c0 //[RecvTable.DT_Player]m_bleedoutState updated 2026/02/11

#define OFFSET_ORIGIN 0x017c //[DataMap.DT_BaseEntity]m_vecAbsOrigin updated 2026/02/07
#define OFFSET_BONES 0x0db8 + 0x48 //[RecvTable.DT_BaseAnimating]m_nForceBone + 0x48 updated 2026/02/11
#define OFFSET_STUDIOHDR 0x1000 //[Miscellaneous].CBaseAnimating!m_pStudioHdr updated 2026/02/11
#define OFFSET_AIMPUNCH 0x2438 //[DataMap.DT_Player]m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2024/09/18
#define OFFSET_CAMERAPOS 0x1fc4 //[Miscellaneous].CPlayer!camera_origin updated 2026/02/11
#define OFFSET_VIEWANGLES 0x2600 //[RecvTable.DT_Player]m_ammoPoolCapacity - 0x14 updated 2026/02/11
#define OFFSET_BREATH_ANGLES (OFFSET_VIEWANGLES - 0x10) //updated 2024/01/10
#define OFFSET_OBSERVER_MODE 0x35e4 //[RecvTable.DT_LocalPlayerExclusive]m_iObserverMode updated 2025/02/25
#define OFFSET_OBSERVING_TARGET 0x35f0 //[RecvTable.DT_LocalPlayerExclusive]m_hObserverTarget updated 2025/02/25
#define OFFSET_IN_USE 0x03f392f0 //[Buttons]+use updated 2026/02/11

#define OFFSET_MATRIX 0x11a350 //[Miscellaneous].ViewMatrix updated 2026/02/07
#define OFFSET_RENDER 0x3f360f0 //[Miscellaneous].ViewRender updated 2026/02/07

#define OFFSET_WEAPON 0x19c4 //[RecvTable.DT_BaseCombatCharacter]m_latestPrimaryWeapons updated 2026/02/11
#define OFFSET_BULLET_SPEED 0x2860 //[Miscellaneous].CWeaponX!m_flProjectileSpeed updated 2026/02/07
#define OFFSET_BULLET_SCALE (OFFSET_BULLET_SPEED + 0x8) //updated 2026/02/07
#define OFFSET_ZOOM_FOV 0x1650 + 0xb8 //[RecvTable.DT_WeaponX]m_playerData + 0xb8 updated 2025/02/25
#define OFFSET_AMMO 0x15e0 //[RecvTable.DT_PropSurvival]m_ammoInClip updated 2026/02/11

#define OFFSET_ITEM_ID 0x15e4 //[RecvTable.DT_PropSurvival]m_customScriptInt updated 2026/02/11
#define OFFSET_MODELNAME 0x0030 //[DataMap.DT_BaseEntity]m_ModelName updated 2024/09/18
#define OFFSET_M_CUSTOMSCRIPTINT OFFSET_ITEM_ID
#define OFFSET_YAW 0x2314 //[DataMap.DT_Player]m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2025/02/12

#define OFFSET_GLOW_ENABLE 0x298 //[RecvTable.DT_HighlightSettings]m_highlightGenericContexts - 0x1 updated 2026/02/11
#define OFFSET_GLOW_THROUGH_WALLS 0x24b //[RecvTable.DT_HighlightSettings]m_highlightGenericContexts - 0x21 updated 2024/07/23

#define OFFSET_TIME_BASE 0x2158 //[DataMap.DT_Player]m_currentFramePlayer.timeBase updated 2025/02/12
#define GLOW_START_TIME (OFFSET_TIME_BASE + 0x6C) //updated 2024/01/10
#define OFFSET_HIGHLIGHTSERVERACTIVESTATES 0x29C //[DT_HighlightSettings]m_highlightServerActiveStates updated 2024/06/26

#define OFFSET_GLOW_FIX 0x278 //updated 2025/02/25
#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT OFFSET_GLOW_ENABLE //updated 2024/01/10
#define OFFSET_GLOW_CONTEXT_ID 0x29c //updated 2024/06/26
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS //updated 2024/01/10
#define GLOW_LIFE_TIME (0x3A4 + 0x30) //updated 2024/01/10
#define GLOW_DISTANCE 0x294 //updated 2024/01/10
#define GLOW_TYPE 0x29c //updated 2024/01/10
#define GLOW_COLOR (0x1D0 + 0x30) //updated 2024/01/10
#define GLOW_FADE (0x388 + 0x30) //updated 2024/01/10
#define HIGHLIGHT_SETTINGS 0x6b95c40 //[Miscellaneous].HighlightSettings updated 2026/02/11
#define HIGHLIGHT_TYPE_SIZE 0x34 //updated 2024/01/10
#define OFFSET_CROSSHAIR_LAST 0x1a64 + 0x19f8 //[.Miscellaneous]CWeaponX!lastCrosshairTargetTime updated 2025/04/18
#define OFFSET_INPUT_SYSTEM 0x2034400 //[Miscellaneous].InputSystem updated 2026/02/11

#define OFFSET_SKYDIVE_STATE 0x4878 //[RecvTable.DT_Player]m_skydiveState updated 2026/02/11
#define OFFSET_GRAPPLEACTIVED 0x2da8 //[RecvTable.DT_Player]m_grappleActive updated 2026/02/11
#define OFFSET_GRAPPLE 0x2d20 //[RecvTable.DT_Player]m_grapple updated 2026/02/11
#define OFFSET_GRAPPLEATTACHED 0x48 //[RecvTable.DT_GrappleData]m_grappleAttached updated 2026/02/07
#define OFFSET_m_xp 0x3824 //[RecvTable.DT_Player]m_xp updated 2026/02/11
#define OFFSET_GRADE 0x0348 //[RecvTable.DT_BaseEntity]m_grade updated 2025/02/25

// REDEFINITIONS - Dynamic at runtime
#undef OFFSET_ENTITYLIST
#define OFFSET_ENTITYLIST offsets.EntityList
#undef OFFSET_LOCAL_ENT
#define OFFSET_LOCAL_ENT offsets.LocalEnt
#undef OFFSET_LOCAL_ENTITY_HANDLE
#define OFFSET_LOCAL_ENTITY_HANDLE offsets.LocalEntityHandle
#undef OFFSET_NAME_LIST
#define OFFSET_NAME_LIST offsets.NameList
#undef OFFSET_GLOBAL_VARS
#define OFFSET_GLOBAL_VARS offsets.GlobalVars
#undef OFFSET_LEVELNAME
#define OFFSET_LEVELNAME offsets.LevelName
#undef OFFSET_HOST_MAP
#define OFFSET_HOST_MAP offsets.HostMap
#undef OFFSET_TEAM
#define OFFSET_TEAM offsets.Team
#undef OFFSET_HEALTH
#define OFFSET_HEALTH offsets.Health
#undef OFFSET_MAXHEALTH
#define OFFSET_MAXHEALTH offsets.MaxHealth
#undef OFFSET_SHIELD
#define OFFSET_SHIELD offsets.Shield
#undef OFFSET_MAXSHIELD
#define OFFSET_MAXSHIELD offsets.MaxShield
#undef OFFSET_ARMORTYPE
#define OFFSET_ARMORTYPE offsets.Armortype
#undef OFFSET_NAME
#define OFFSET_NAME offsets.Name
#undef OFFSET_SIGN_NAME
#define OFFSET_SIGN_NAME offsets.SignName
#undef OFFSET_ABS_VELOCITY
#define OFFSET_ABS_VELOCITY offsets.AbsVelocity
#undef OFFSET_VISIBLE_TIME
#define OFFSET_VISIBLE_TIME offsets.VisibleTime
#undef OFFSET_ZOOMING
#define OFFSET_ZOOMING offsets.Zooming
#undef OFFSET_VIEW_OFFSET
#define OFFSET_VIEW_OFFSET offsets.ViewOffset
#undef OFFSET_IN_DUCKSTATE
#define OFFSET_IN_DUCKSTATE offsets.InDuckState
#undef OFFSET_IN_DUCK
#define OFFSET_IN_DUCK offsets.InDuck
#undef OFFSET_TRAVERSAL_PROGRESS
#define OFFSET_TRAVERSAL_PROGRESS offsets.TraversalProgress
#undef OFFSET_TRAVERSAL_STARTTIME
#define OFFSET_TRAVERSAL_STARTTIME offsets.TraversalStartTime
#undef OFFSET_TRAVERSAL_RELEASE_TIME
#define OFFSET_TRAVERSAL_RELEASE_TIME offsets.TraversalReleaseTime
#undef OFFSET_IN_JUMP
#define OFFSET_IN_JUMP offsets.InJump
#undef OFFSET_IN_TOGGLE_DUCK
#define OFFSET_IN_TOGGLE_DUCK offsets.InToggleDuck
#undef OFFSET_WEAPON_NAME
#define OFFSET_WEAPON_NAME offsets.WeaponName
#undef OFFSET_OFF_WEAPON
#define OFFSET_OFF_WEAPON offsets.OffWeapon
#undef OFFSET_WALL_RUN_START_TIME
#define OFFSET_WALL_RUN_START_TIME offsets.WallRunStartTime
#undef OFFSET_WALL_RUN_CLEAR_TIME
#define OFFSET_WALL_RUN_CLEAR_TIME offsets.WallRunClearTime
#undef OFFSET_FLAGS
#define OFFSET_FLAGS offsets.Flags
#undef OFFSET_IN_ATTACK
#define OFFSET_IN_ATTACK offsets.InAttack
#undef OFFSET_IN_ZOOM
#define OFFSET_IN_ZOOM offsets.InZoom
#undef OFFSET_IN_FORWARD
#define OFFSET_IN_FORWARD offsets.InForward
#undef OFFSET_IN_BACKWARD
#define OFFSET_IN_BACKWARD offsets.InBackward
#undef OFFSET_LIFE_STATE
#define OFFSET_LIFE_STATE offsets.LifeState
#undef OFFSET_BLEED_OUT_STATE
#define OFFSET_BLEED_OUT_STATE offsets.BleedOutState
#undef OFFSET_ORIGIN
#define OFFSET_ORIGIN offsets.Origin
#undef OFFSET_BONES
#define OFFSET_BONES offsets.Bones
#undef OFFSET_STUDIOHDR
#define OFFSET_STUDIOHDR offsets.StudioHdr
#undef OFFSET_AIMPUNCH
#define OFFSET_AIMPUNCH offsets.AimPunch
#undef OFFSET_CAMERAPOS
#define OFFSET_CAMERAPOS offsets.CameraPos
#undef OFFSET_VIEWANGLES
#define OFFSET_VIEWANGLES offsets.ViewAngles
#undef OFFSET_OBSERVER_MODE
#define OFFSET_OBSERVER_MODE offsets.ObserverMode
#undef OFFSET_OBSERVING_TARGET
#define OFFSET_OBSERVING_TARGET offsets.ObservingTarget
#undef OFFSET_IN_USE
#define OFFSET_IN_USE offsets.InUse
#undef OFFSET_MATRIX
#define OFFSET_MATRIX offsets.Matrix
#undef OFFSET_RENDER
#define OFFSET_RENDER offsets.Render
#undef OFFSET_WEAPON
#define OFFSET_WEAPON offsets.Weapon
#undef OFFSET_BULLET_SPEED
#define OFFSET_BULLET_SPEED offsets.BulletSpeed
#undef OFFSET_ZOOM_FOV
#define OFFSET_ZOOM_FOV offsets.ZoomFov
#undef OFFSET_AMMO
#define OFFSET_AMMO offsets.Ammo
#undef OFFSET_ITEM_ID
#define OFFSET_ITEM_ID offsets.ItemId
#undef OFFSET_MODELNAME
#define OFFSET_MODELNAME offsets.ModelName
#undef OFFSET_YAW
#define OFFSET_YAW offsets.Yaw
#undef OFFSET_GLOW_ENABLE
#define OFFSET_GLOW_ENABLE offsets.GlowEnable
#undef OFFSET_GLOW_THROUGH_WALLS
#define OFFSET_GLOW_THROUGH_WALLS offsets.GlowThroughWalls
#undef OFFSET_TIME_BASE
#define OFFSET_TIME_BASE offsets.TimeBase
#undef HIGHLIGHT_SETTINGS
#define HIGHLIGHT_SETTINGS offsets.HighlightSettings
#undef OFFSET_INPUT_SYSTEM
#define OFFSET_INPUT_SYSTEM offsets.InputSystem
#undef OFFSET_SKYDIVE_STATE
#define OFFSET_SKYDIVE_STATE offsets.SkydiveState
#undef OFFSET_GRAPPLEACTIVED
#define OFFSET_GRAPPLEACTIVED offsets.GrappleActive
#undef OFFSET_GRAPPLE
#define OFFSET_GRAPPLE offsets.Grapple
#undef OFFSET_GRAPPLEATTACHED
#define OFFSET_GRAPPLEATTACHED offsets.GrappleAttached
#undef OFFSET_m_xp
#define OFFSET_m_xp offsets.m_xp
#undef OFFSET_GRADE
#define OFFSET_GRADE offsets.Grade
#undef OFFSET_CROSSHAIR_LAST
#define OFFSET_CROSSHAIR_LAST offsets.CrosshairLastTargetTime

#endif
