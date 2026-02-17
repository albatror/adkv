#include "offsets_dynamic.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

DynamicOffsets offsets = {0};

void initialize_offsets() {
    offsets.EntityList = 0x644bf38;
    offsets.LocalEnt = 0x282c198;
    offsets.LocalEntityHandle = 0x2017c68;
    offsets.NameList = 0x8e46440;
    offsets.GlobalVars = 0x1f7a5b0;
    offsets.LevelName = 0x1f7aa34;
    offsets.ClientState = offsets.LevelName - 0x1c0;
    offsets.SignonState = offsets.ClientState + 0xac;
    offsets.HostMap = 0x01e93dd0 + 0x58;
    offsets.Team = 0x0334;
    offsets.Health = 0x0324;
    offsets.MaxHealth = 0x0468;
    offsets.Shield = 0x01a0;
    offsets.MaxShield = 0x1A4;
    offsets.Armortype = 0x4814;
    offsets.Name = 0x0479;
    offsets.SignName = 0x0470;
    offsets.AbsVelocity = 0x0170;
    offsets.VisibleTime = 0x1a64;
    offsets.LastAimedAtTime = 0x1a5c;
    offsets.Zooming = 0x1cb1;
    offsets.ViewOffset = 0x00e8;
    offsets.ActiveWeapon = 0x1958 + 0x0058;
    offsets.ObserverList = 0x644df58 + 0x20C8;
    offsets.ObserverArray = 0x954;
    offsets.InDuckState = 0x2abc;
    offsets.InDuck = 0x03f39378;
    offsets.TraversalProgress = 0x2bcc;
    offsets.TraversalStartTime = 0x2bd4;
    offsets.TraversalReleaseTime = 0x2bdc;
    offsets.InJump = 0x3f39270;
    offsets.InToggleDuck = 0x03f391a8;
    offsets.WeaponName = 0x15f0;
    offsets.OffWeapon = 0x19d4;
    offsets.WallRunStartTime = 0x370c;
    offsets.WallRunClearTime = 0x3710;
    offsets.Flags = 0x00c8;
    offsets.InAttack = 0x03f389e8;
    offsets.InZoom = 0x03f39300;
    offsets.InForward = 0x03f393b8;
    offsets.InBackward = 0x03f393e0;
    offsets.LifeState = 0x0690;
    offsets.BleedOutState = 0x27c0;
    offsets.Origin = 0x017c;
    offsets.Bones = 0x0db8 + 0x48;
    offsets.StudioHdr = 0x1000;
    offsets.AimPunch = 0x2438;
    offsets.CameraPos = 0x1fc4;
    offsets.ViewAngles = 0x2614 - 0x14;
    offsets.BreathAngles = offsets.ViewAngles - 0x10;
    offsets.ObserverMode = 0x35e4;
    offsets.ObservingTarget = 0x35f0;
    offsets.InUse = 0x03f392f0;
    offsets.Matrix = 0x11a350;
    offsets.Render = 0x3f360f0;
    offsets.Weapon = 0x19c4;
    offsets.BulletSpeed = 0x2860;
    offsets.BulletScale = offsets.BulletSpeed + 0x8;
    offsets.ZoomFov = 0x1650 + 0xb8;
    offsets.Ammo = 0x15e0;
    offsets.ItemId = 0x15e4;
    offsets.ModelName = 0x0030;
    offsets.Yaw = 0x231c - 0x8;
    offsets.GlowEnable = 0x0299 - 0x1;
    offsets.GlowThroughWalls = 0x26c - 0x21;
    offsets.TimeBase = 0x2158;
    offsets.HighlightSettings = 0x6b95c40;
    offsets.InputSystem = 0x2034400;
    offsets.SkydiveState = 0x4878;
    offsets.GrappleActive = 0x2da8;
    offsets.Grapple = 0x2d20;
    offsets.GrappleAttached = 0x48;
    offsets.m_xp = 0x3824;
    offsets.Grade = 0x0348;
    offsets.CrosshairLastTargetTime = offsets.VisibleTime + 0x19f8;
}

static std::map<std::string, uint64_t> read_ini(const char* filename) {
    std::map<std::string, uint64_t> data;
    std::ifstream file(filename);
    if (!file.is_open()) return data;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#' || line[0] == ';') continue;

        std::stringstream ss(line);
        std::string key, value;
        if (ss >> key >> value) {
            try {
                data[key] = std::stoull(value, nullptr, 16);
            } catch (...) {}
        }
    }
    return data;
}

bool load_offsets_from_ini(const char* offsets_file, const char* convars_file, const char* buttons_file) {
    auto off_data = read_ini(offsets_file);
    auto con_data = read_ini(convars_file);
    auto but_data = read_ini(buttons_file);

    if (off_data.empty()) return false;

    // Mapping based on r5dumper output
    if (off_data.count("[.Miscellaneous]cl_entitylist")) offsets.EntityList = off_data["[.Miscellaneous]cl_entitylist"];
    if (off_data.count("[.Miscellaneous]LocalPlayer")) offsets.LocalEnt = off_data["[.Miscellaneous]LocalPlayer"];
    if (off_data.count("[.Miscellaneous]LocalEntityHandle")) offsets.LocalEntityHandle = off_data["[.Miscellaneous]LocalEntityHandle"];
    if (off_data.count("[.Miscellaneous]NameList")) offsets.NameList = off_data["[.Miscellaneous]NameList"];
    if (off_data.count("[.Miscellaneous]GlobalVars")) offsets.GlobalVars = off_data["[.Miscellaneous]GlobalVars"];
    if (off_data.count("[.Miscellaneous]LevelName")) offsets.LevelName = off_data["[.Miscellaneous]LevelName"];
    if (off_data.count("[.Miscellaneous]ViewRender")) offsets.Render = off_data["[.Miscellaneous]ViewRender"];
    if (off_data.count("[.Miscellaneous]ViewMatrix")) offsets.Matrix = off_data["[.Miscellaneous]ViewMatrix"];
    if (off_data.count("[.Miscellaneous]CPlayer!camera_origin")) offsets.CameraPos = off_data["[.Miscellaneous]CPlayer!camera_origin"];
    if (off_data.count("[.Miscellaneous]CBaseAnimating!m_pStudioHdr")) offsets.StudioHdr = off_data["[.Miscellaneous]CBaseAnimating!m_pStudioHdr"];
    if (off_data.count("[.Miscellaneous]InputSystem")) offsets.InputSystem = off_data["[.Miscellaneous]InputSystem"];
    if (off_data.count("[.Miscellaneous]HighlightSettings")) offsets.HighlightSettings = off_data["[.Miscellaneous]HighlightSettings"];

    if (off_data.count("[RecvTable.DT_BaseEntity]m_iTeamNum")) offsets.Team = off_data["[RecvTable.DT_BaseEntity]m_iTeamNum"];
    if (off_data.count("[RecvTable.DT_Player]m_iHealth")) offsets.Health = off_data["[RecvTable.DT_Player]m_iHealth"];
    if (off_data.count("[RecvTable.DT_Player]m_iMaxHealth")) offsets.MaxHealth = off_data["[RecvTable.DT_Player]m_iMaxHealth"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_shieldHealth")) offsets.Shield = off_data["[RecvTable.DT_BaseEntity]m_shieldHealth"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_shieldHealthMax")) offsets.MaxShield = off_data["[RecvTable.DT_BaseEntity]m_shieldHealthMax"];
    if (off_data.count("[RecvTable.DT_Player]m_armorType")) offsets.Armortype = off_data["[RecvTable.DT_Player]m_armorType"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_iName")) offsets.Name = off_data["[RecvTable.DT_BaseEntity]m_iName"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_iSignifierName")) offsets.SignName = off_data["[RecvTable.DT_BaseEntity]m_iSignifierName"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_vecAbsVelocity")) offsets.AbsVelocity = off_data["[DataMap.DT_BaseEntity]m_vecAbsVelocity"];

    // Thread Info Mappings
    if (off_data.count("[.Miscellaneous]lastVisibleTime_-2")) {
        offsets.VisibleTime = off_data["[.Miscellaneous]lastVisibleTime_-2"] + 2;
    } else if (off_data.count("[DataMap.C_BaseCombatCharacter]m_hudInfo_visibilityTestAlwaysPasses")) {
        offsets.VisibleTime = off_data["[DataMap.C_BaseCombatCharacter]m_hudInfo_visibilityTestAlwaysPasses"] + 2;
    } else if (off_data.count("[.Miscellaneous]CPlayer!lastVisibleTime")) {
        offsets.VisibleTime = off_data["[.Miscellaneous]CPlayer!lastVisibleTime"];
    }

    if (off_data.count("[DataMap.C_Player]m_currentFramePlayer.timeBase")) {
        offsets.TimeBase = off_data["[DataMap.C_Player]m_currentFramePlayer.timeBase"];
    } else if (off_data.count("[DataMap.C_Player]currentFramePlayer.timeBase")) {
        offsets.TimeBase = off_data["[DataMap.C_Player]currentFramePlayer.timeBase"];
    } else if (off_data.count("[DataMap.DT_Player]m_currentFramePlayer.timeBase")) {
        offsets.TimeBase = off_data["[DataMap.DT_Player]m_currentFramePlayer.timeBase"];
    } else if (off_data.count("[DataMap.DT_Player]currentFramePlayer.timeBase")) {
        offsets.TimeBase = off_data["[DataMap.DT_Player]currentFramePlayer.timeBase"];
    }

    if (off_data.count("[RecvTable.DT_Player]m_bZooming")) offsets.Zooming = off_data["[RecvTable.DT_Player]m_bZooming"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_currentFrame.viewOffset")) offsets.ViewOffset = off_data["[DataMap.DT_BaseEntity]m_currentFrame.viewOffset"];

    if (off_data.count("[RecvTable.DT_Player]m_duckState")) offsets.InDuckState = off_data["[RecvTable.DT_Player]m_duckState"];
    if (off_data.count("[DataMap.DT_Player]m_traversalProgress")) offsets.TraversalProgress = off_data["[DataMap.DT_Player]m_traversalProgress"];
    if (off_data.count("[DataMap.DT_Player]m_traversalStartTime")) offsets.TraversalStartTime = off_data["[DataMap.DT_Player]m_traversalStartTime"];

    if (off_data.count("[RecvTable.DT_Player]m_lifeState")) offsets.LifeState = off_data["[RecvTable.DT_Player]m_lifeState"];
    if (off_data.count("[RecvTable.DT_Player]m_bleedoutState")) offsets.BleedOutState = off_data["[RecvTable.DT_Player]m_bleedoutState"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_vecAbsOrigin")) offsets.Origin = off_data["[DataMap.DT_BaseEntity]m_vecAbsOrigin"];
    if (off_data.count("[RecvTable.DT_BaseAnimating]m_nForceBone")) offsets.Bones = off_data["[RecvTable.DT_BaseAnimating]m_nForceBone"] + 0x48;
    if (off_data.count("[DataMap.DT_Player]m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle")) offsets.AimPunch = off_data["[DataMap.DT_Player]m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle"];

    if (off_data.count("[.Miscellaneous]ViewAngles")) offsets.ViewAngles = off_data["[.Miscellaneous]ViewAngles"];
    if (offsets.ViewAngles) offsets.BreathAngles = offsets.ViewAngles - 0x10;

    if (off_data.count("[RecvTable.DT_LocalPlayerExclusive]m_iObserverMode")) offsets.ObserverMode = off_data["[RecvTable.DT_LocalPlayerExclusive]m_iObserverMode"];
    if (off_data.count("[RecvTable.DT_LocalPlayerExclusive]m_hObserverTarget")) offsets.ObservingTarget = off_data["[RecvTable.DT_LocalPlayerExclusive]m_hObserverTarget"];

    if (off_data.count("[RecvTable.DT_BaseCombatCharacter]m_latestPrimaryWeapons")) offsets.Weapon = off_data["[RecvTable.DT_BaseCombatCharacter]m_latestPrimaryWeapons"];
    if (off_data.count("[.Miscellaneous]CWeaponX!m_flProjectileSpeed")) offsets.BulletSpeed = off_data["[.Miscellaneous]CWeaponX!m_flProjectileSpeed"];
    if (offsets.BulletSpeed) offsets.BulletScale = offsets.BulletSpeed + 0x8;

    if (off_data.count("[RecvTable.DT_WeaponX]m_playerData")) {
        offsets.ZoomFov = off_data["[RecvTable.DT_WeaponX]m_playerData"] + 0xb8;
    }
    if (off_data.count("[RecvTable.DT_PropSurvival]m_ammoInClip")) offsets.Ammo = off_data["[RecvTable.DT_PropSurvival]m_ammoInClip"];
    if (off_data.count("[RecvTable.DT_PropSurvival]m_customScriptInt")) offsets.ItemId = off_data["[RecvTable.DT_PropSurvival]m_customScriptInt"];
    if (off_data.count("[DataMap.DT_BaseEntity]m_ModelName")) offsets.ModelName = off_data["[DataMap.DT_BaseEntity]m_ModelName"];
    if (off_data.count("[DataMap.DT_Player]m_currentFramePlayer.m_ammoPoolCount")) offsets.Yaw = off_data["[DataMap.DT_Player]m_currentFramePlayer.m_ammoPoolCount"] - 0x8;
    else if (off_data.count("[DataMap.DT_Player]currentFramePlayer.m_ammoPoolCount")) offsets.Yaw = off_data["[DataMap.DT_Player]currentFramePlayer.m_ammoPoolCount"] - 0x8;

    if (off_data.count("[RecvTable.DT_HighlightSettings]m_highlightGenericContexts")) {
        offsets.GlowEnable = off_data["[RecvTable.DT_HighlightSettings]m_highlightGenericContexts"] - 0x1;
        offsets.GlowThroughWalls = off_data["[RecvTable.DT_HighlightSettings]m_highlightGenericContexts"] - 0x21;
    }

    if (off_data.count("[RecvTable.DT_Player]m_skydiveState")) offsets.SkydiveState = off_data["[RecvTable.DT_Player]m_skydiveState"];
    if (off_data.count("[RecvTable.DT_Player]m_grappleActive")) offsets.GrappleActive = off_data["[RecvTable.DT_Player]m_grappleActive"];
    if (off_data.count("[RecvTable.DT_Player]m_grapple")) offsets.Grapple = off_data["[RecvTable.DT_Player]m_grapple"];
    if (off_data.count("[RecvTable.DT_GrappleData]m_grappleAttached")) offsets.GrappleAttached = off_data["[RecvTable.DT_GrappleData]m_grappleAttached"];
    if (off_data.count("[RecvTable.DT_Player]m_xp")) offsets.m_xp = off_data["[RecvTable.DT_Player]m_xp"];
    if (off_data.count("[RecvTable.DT_BaseEntity]m_grade")) offsets.Grade = off_data["[RecvTable.DT_BaseEntity]m_grade"];

    if (off_data.count("[.Miscellaneous]CWeaponX!lastCrosshairTargetTime")) {
        offsets.CrosshairLastTargetTime = off_data["[.Miscellaneous]CWeaponX!lastCrosshairTargetTime"];
    } else {
        offsets.CrosshairLastTargetTime = offsets.VisibleTime + 0x19f8;
    }

    if (but_data.count("[Buttons]in_duck")) offsets.InDuck = but_data["[Buttons]in_duck"];
    if (but_data.count("[Buttons]in_jump")) offsets.InJump = but_data["[Buttons]in_jump"];
    if (but_data.count("[Buttons]in_attack")) offsets.InAttack = but_data["[Buttons]in_attack"];
    if (but_data.count("[Buttons]in_zoom")) offsets.InZoom = but_data["[Buttons]in_zoom"];
    if (but_data.count("[Buttons]in_forward")) offsets.InForward = but_data["[Buttons]in_forward"];
    if (but_data.count("[Buttons]in_backward")) offsets.InBackward = but_data["[Buttons]in_backward"];
    if (but_data.count("[Buttons]in_use")) offsets.InUse = but_data["[Buttons]in_use"];
    if (but_data.count("[Buttons]in_toggle_duck")) offsets.InToggleDuck = but_data["[Buttons]in_toggle_duck"];

    if (con_data.count("[ConVars]host_map")) offsets.HostMap = con_data["[ConVars]host_map"] + 0x58;

    printf("Offsets updated from INI files.\n");
    return true;
}

bool save_offsets_to_files(const char* h_file, const char* ini_file) {
    // Save to .h file
    FILE* h = fopen(h_file, "w");
    if (h) {
        fprintf(h, "#include \"offsets_dynamic.h\"\n\n");
        fprintf(h, "#define GameVersion v3.0.3.24\n");
        fprintf(h, "#define OFFSET_ENTITYLIST 0x%lx\n", offsets.EntityList);
        fprintf(h, "#define OFFSET_LOCAL_ENT 0x%lx\n", offsets.LocalEnt);
        fprintf(h, "#define OFFSET_LOCAL_ENTITY_HANDLE 0x%lx\n", offsets.LocalEntityHandle);
        fprintf(h, "#define OFFSET_NAME_LIST 0x%lx\n", offsets.NameList);
        fprintf(h, "#define OFFSET_GLOBAL_VARS 0x%lx\n", offsets.GlobalVars);
        fprintf(h, "#define OFFSET_LEVELNAME 0x%lx\n", offsets.LevelName);
        fprintf(h, "#define OFFSET_CLIENTSTATE 0x%lx\n", offsets.ClientState);
        fprintf(h, "#define OFFSET_SIGNONSTATE 0x%lx\n", offsets.SignonState);
        fprintf(h, "#define OFFSET_HOST_MAP 0x%lx\n", offsets.HostMap);
        fprintf(h, "#define OFFSET_TEAM 0x%lx\n", offsets.Team);
        fprintf(h, "#define OFFSET_HEALTH 0x%lx\n", offsets.Health);
        fprintf(h, "#define OFFSET_MAXHEALTH 0x%lx\n", offsets.MaxHealth);
        fprintf(h, "#define OFFSET_SHIELD 0x%lx\n", offsets.Shield);
        fprintf(h, "#define OFFSET_MAXSHIELD 0x%lx\n", offsets.MaxShield);
        fprintf(h, "#define OFFSET_ARMORTYPE 0x%lx\n", offsets.Armortype);
        fprintf(h, "#define OFFSET_NAME 0x%lx\n", offsets.Name);
        fprintf(h, "#define OFFSET_SIGN_NAME 0x%lx\n", offsets.SignName);
        fprintf(h, "#define OFFSET_ABS_VELOCITY 0x%lx\n", offsets.AbsVelocity);
        fprintf(h, "#define OFFSET_VISIBLE_TIME 0x%lx\n", offsets.VisibleTime);
        fprintf(h, "#define OFFSET_LAST_AIMEDAT_TIME 0x%lx\n", offsets.LastAimedAtTime);
        fprintf(h, "#define OFFSET_ZOOMING 0x%lx\n", offsets.Zooming);
        fprintf(h, "#define OFFSET_VIEW_OFFSET 0x%lx\n", offsets.ViewOffset);
        fprintf(h, "#define OFFSET_ACTIVE_WEAPON 0x%lx\n", offsets.ActiveWeapon);
        fprintf(h, "#define OFFSET_OBSERVER_LIST 0x%lx\n", offsets.ObserverList);
        fprintf(h, "#define OFFSET_OBSERVER_ARRAY 0x%lx\n", offsets.ObserverArray);
        fprintf(h, "#define OFFSET_IN_DUCKSTATE 0x%lx\n", offsets.InDuckState);
        fprintf(h, "#define OFFSET_IN_DUCK 0x%lx\n", offsets.InDuck);
        fprintf(h, "#define OFFSET_TRAVERSAL_PROGRESS 0x%lx\n", offsets.TraversalProgress);
        fprintf(h, "#define OFFSET_TRAVERSAL_STARTTIME 0x%lx\n", offsets.TraversalStartTime);
        fprintf(h, "#define OFFSET_TRAVERSAL_RELEASE_TIME 0x%lx\n", offsets.TraversalReleaseTime);
        fprintf(h, "#define OFFSET_IN_JUMP 0x%lx\n", offsets.InJump);
        fprintf(h, "#define OFFSET_IN_TOGGLE_DUCK 0x%lx\n", offsets.InToggleDuck);
        fprintf(h, "#define OFFSET_WEAPON_NAME 0x%lx\n", offsets.WeaponName);
        fprintf(h, "#define OFFSET_OFF_WEAPON 0x%lx\n", offsets.OffWeapon);
        fprintf(h, "#define OFFSET_WALL_RUN_START_TIME 0x%lx\n", offsets.WallRunStartTime);
        fprintf(h, "#define OFFSET_WALL_RUN_CLEAR_TIME 0x%lx\n", offsets.WallRunClearTime);
        fprintf(h, "#define OFFSET_FLAGS 0x%lx\n", offsets.Flags);
        fprintf(h, "#define OFFSET_IN_ATTACK 0x%lx\n", offsets.InAttack);
        fprintf(h, "#define OFFSET_IN_ZOOM 0x%lx\n", offsets.InZoom);
        fprintf(h, "#define OFFSET_IN_FORWARD 0x%lx\n", offsets.InForward);
        fprintf(h, "#define OFFSET_IN_BACKWARD 0x%lx\n", offsets.InBackward);
        fprintf(h, "#define OFFSET_LIFE_STATE 0x%lx\n", offsets.LifeState);
        fprintf(h, "#define OFFSET_BLEED_OUT_STATE 0x%lx\n", offsets.BleedOutState);
        fprintf(h, "#define OFFSET_ORIGIN 0x%lx\n", offsets.Origin);
        fprintf(h, "#define OFFSET_BONES 0x%lx\n", offsets.Bones);
        fprintf(h, "#define OFFSET_STUDIOHDR 0x%lx\n", offsets.StudioHdr);
        fprintf(h, "#define OFFSET_AIMPUNCH 0x%lx\n", offsets.AimPunch);
        fprintf(h, "#define OFFSET_CAMERAPOS 0x%lx\n", offsets.CameraPos);
        fprintf(h, "#define OFFSET_VIEWANGLES 0x%lx\n", offsets.ViewAngles);
        fprintf(h, "#define OFFSET_BREATH_ANGLES 0x%lx\n", offsets.BreathAngles);
        fprintf(h, "#define OFFSET_OBSERVER_MODE 0x%lx\n", offsets.ObserverMode);
        fprintf(h, "#define OFFSET_OBSERVING_TARGET 0x%lx\n", offsets.ObservingTarget);
        fprintf(h, "#define OFFSET_IN_USE 0x%lx\n", offsets.InUse);
        fprintf(h, "#define OFFSET_MATRIX 0x%lx\n", offsets.Matrix);
        fprintf(h, "#define OFFSET_RENDER 0x%lx\n", offsets.Render);
        fprintf(h, "#define OFFSET_WEAPON 0x%lx\n", offsets.Weapon);
        fprintf(h, "#define OFFSET_BULLET_SPEED 0x%lx\n", offsets.BulletSpeed);
        fprintf(h, "#define OFFSET_BULLET_SCALE 0x%lx\n", offsets.BulletScale);
        fprintf(h, "#define OFFSET_ZOOM_FOV 0x%lx\n", offsets.ZoomFov);
        fprintf(h, "#define OFFSET_AMMO 0x%lx\n", offsets.Ammo);
        fprintf(h, "#define OFFSET_ITEM_ID 0x%lx\n", offsets.ItemId);
        fprintf(h, "#define OFFSET_MODELNAME 0x%lx\n", offsets.ModelName);
        fprintf(h, "#define OFFSET_YAW 0x%lx\n", offsets.Yaw);
        fprintf(h, "#define OFFSET_GLOW_ENABLE 0x%lx\n", offsets.GlowEnable);
        fprintf(h, "#define OFFSET_GLOW_THROUGH_WALLS 0x%lx\n", offsets.GlowThroughWalls);
        fprintf(h, "#define OFFSET_TIME_BASE 0x%lx\n", offsets.TimeBase);
        fprintf(h, "#define HIGHLIGHT_SETTINGS 0x%lx\n", offsets.HighlightSettings);
        fprintf(h, "#define OFFSET_INPUT_SYSTEM 0x%lx\n", offsets.InputSystem);
        fprintf(h, "#define OFFSET_SKYDIVE_STATE 0x%lx\n", offsets.SkydiveState);
        fprintf(h, "#define OFFSET_GRAPPLEACTIVED 0x%lx\n", offsets.GrappleActive);
        fprintf(h, "#define OFFSET_GRAPPLE 0x%lx\n", offsets.Grapple);
        fprintf(h, "#define OFFSET_GRAPPLEATTACHED 0x%lx\n", offsets.GrappleAttached);
        fprintf(h, "#define OFFSET_m_xp 0x%lx\n", offsets.m_xp);
        fprintf(h, "#define OFFSET_GRADE 0x%lx\n", offsets.Grade);
        fprintf(h, "#define OFFSET_CROSSHAIR_LAST 0x%lx\n", offsets.CrosshairLastTargetTime);
        fclose(h);
    }

    // Save to .ini file (Simplified)
    FILE* ini = fopen(ini_file, "w");
    if (ini) {
        fprintf(ini, "[Miscellaneous]\n");
        fprintf(ini, "cl_entitylist 0x%lx\n", offsets.EntityList);
        fprintf(ini, "LocalPlayer 0x%lx\n", offsets.LocalEnt);
        fprintf(ini, "GlobalVars 0x%lx\n", offsets.GlobalVars);
        fprintf(ini, "LevelName 0x%lx\n", offsets.LevelName);
        fprintf(ini, "ViewMatrix 0x%lx\n", offsets.Matrix);
        fprintf(ini, "ViewRender 0x%lx\n", offsets.Render);
        fprintf(ini, "\n[Buttons]\n");
        fprintf(ini, "in_attack 0x%lx\n", offsets.InAttack);
        fprintf(ini, "in_jump 0x%lx\n", offsets.InJump);
        fprintf(ini, "in_duck 0x%lx\n", offsets.InDuck);
        fprintf(ini, "in_forward 0x%lx\n", offsets.InForward);
        fprintf(ini, "in_backward 0x%lx\n", offsets.InBackward);
        fprintf(ini, "in_use 0x%lx\n", offsets.InUse);
        fprintf(ini, "in_zoom 0x%lx\n", offsets.InZoom);
        fclose(ini);
    }

    return true;
}
