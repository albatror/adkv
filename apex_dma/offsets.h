#include "offsets_dynamic.h"

#define GameVersion v3.0.3.24
#define OFFSET_ENTITYLIST offsets.EntityList
#define OFFSET_LOCAL_ENT offsets.LocalEnt
#define OFFSET_LOCAL_ENTITY_HANDLE offsets.LocalEntityHandle
#define OFFSET_NAME_LIST offsets.NameList
#define OFFSET_GLOBAL_VARS offsets.GlobalVars
 
#define OFFSET_LEVELNAME offsets.LevelName
#define OFFSET_CLIENTSTATE offsets.ClientState
#define OFFSET_SIGNONSTATE offsets.SignonState
#define OFFSET_HOST_MAP offsets.HostMap
 
#define OFFSET_TEAM offsets.Team
#define OFFSET_HEALTH offsets.Health
#define OFFSET_MAXHEALTH offsets.MaxHealth

#define OFFSET_SHIELD offsets.Shield
#define OFFSET_MAXSHIELD offsets.MaxShield
#define OFFSET_ARMORTYPE offsets.Armortype
#define OFFSET_NAME offsets.Name
#define OFFSET_SIGN_NAME offsets.SignName
#define OFFSET_ABS_VELOCITY offsets.AbsVelocity
#define OFFSET_VISIBLE_TIME offsets.VisibleTime
#define OFFSET_LAST_AIMEDAT_TIME offsets.LastAimedAtTime
#define OFFSET_ZOOMING offsets.Zooming

#define OFFSET_VIEW_OFFSET offsets.ViewOffset
#define OFFSET_ACTIVE_WEAPON offsets.ActiveWeapon

#define OFFSET_SPECTATOR_LIST OFFSET_OBSERVER_LIST
#define OFFSET_OBSERVER_LIST offsets.ObserverList
#define OFFSET_OBSERVER_ARRAY offsets.ObserverArray

#define OFFSET_IN_DUCKSTATE offsets.InDuckState

#define OFFSET_IN_DUCK offsets.InDuck
#define OFFSET_TRAVERSAL_PROGRESS offsets.TraversalProgress
#define OFFSET_TRAVERSAL_STARTTIME offsets.TraversalStartTime
#define OFFSET_TRAVERSAL_RELEASE_TIME offsets.TraversalReleaseTime

#define OFFSET_IN_JUMP offsets.InJump
#define OFFSET_IN_TOGGLE_DUCK offsets.InToggleDuck
 
#define OFFSET_WEAPON_NAME offsets.WeaponName
#define OFFSET_OFF_WEAPON offsets.OffWeapon
#define OFFSET_WALL_RUN_START_TIME offsets.WallRunStartTime
#define OFFSET_WALL_RUN_CLEAR_TIME offsets.WallRunClearTime

#define OFFSET_FLAGS offsets.Flags
#define OFFSET_IN_ATTACK offsets.InAttack
#define OFFSET_IN_ZOOM offsets.InZoom
#define OFFSET_IN_FORWARD offsets.InForward
#define OFFSET_IN_BACKWARD offsets.InBackward
 
#define OFFSET_LIFE_STATE offsets.LifeState
#define OFFSET_BLEED_OUT_STATE offsets.BleedOutState
 
#define OFFSET_ORIGIN offsets.Origin
#define OFFSET_BONES offsets.Bones
#define OFFSET_STUDIOHDR offsets.StudioHdr
#define OFFSET_AIMPUNCH offsets.AimPunch
#define OFFSET_CAMERAPOS offsets.CameraPos
#define OFFSET_VIEWANGLES offsets.ViewAngles
#define OFFSET_BREATH_ANGLES offsets.BreathAngles
#define OFFSET_OBSERVER_MODE offsets.ObserverMode
#define OFFSET_OBSERVING_TARGET offsets.ObservingTarget
#define OFFSET_IN_USE offsets.InUse
 
#define OFFSET_MATRIX offsets.Matrix
#define OFFSET_RENDER offsets.Render
 
#define OFFSET_WEAPON offsets.Weapon
#define OFFSET_BULLET_SPEED offsets.BulletSpeed
#define OFFSET_BULLET_SCALE offsets.BulletScale
#define OFFSET_ZOOM_FOV offsets.ZoomFov
#define OFFSET_AMMO offsets.Ammo
 
#define OFFSET_ITEM_ID offsets.ItemId
#define OFFSET_MODELNAME offsets.ModelName
#define OFFSET_M_CUSTOMSCRIPTINT OFFSET_ITEM_ID
#define OFFSET_YAW offsets.Yaw
 
#define OFFSET_GLOW_ENABLE offsets.GlowEnable
#define OFFSET_GLOW_THROUGH_WALLS offsets.GlowThroughWalls
 
#define OFFSET_TIME_BASE offsets.TimeBase
#define GLOW_START_TIME (offsets.TimeBase + 0x6C) // Approximate fallback
#define OFFSET_HIGHLIGHTSERVERACTIVESTATES 0x29C // Hardcoded as it is usually stable

#define OFFSET_GLOW_FIX 0x278
#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT OFFSET_GLOW_ENABLE
#define OFFSET_GLOW_CONTEXT_ID 0x29c
#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS
#define GLOW_LIFE_TIME 0x3A4+ 0x30
#define GLOW_DISTANCE 0x294
#define GLOW_TYPE 0x29c
#define GLOW_COLOR 0x1D0+ 0x30
#define GLOW_FADE 0x388+ 0x30
#define HIGHLIGHT_SETTINGS offsets.HighlightSettings
#define HIGHLIGHT_TYPE_SIZE 0x34
#define OFFSET_CROSSHAIR_LAST offsets.CrosshairLastTargetTime
#define OFFSET_INPUT_SYSTEM offsets.InputSystem

#define OFFSET_SKYDIVE_STATE offsets.SkydiveState
#define OFFSET_GRAPPLEACTIVED offsets.GrappleActive
#define OFFSET_GRAPPLE offsets.Grapple
#define OFFSET_GRAPPLEATTACHED offsets.GrappleAttached
#define OFFSET_m_xp offsets.m_xp
#define OFFSET_GRADE offsets.Grade
