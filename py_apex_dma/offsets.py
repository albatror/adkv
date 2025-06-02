# Parsed from apex_dma/offsets.h
# GameVersion v3.0.1.29

GAME_OFFSETS = {
    "OFFSET_ENTITYLIST": 0x6b6ce88, # //[Miscellaneous].cl_entitylist updated 2025/04/18
    "OFFSET_LOCAL_ENT": 0x26c8128, # //[Miscellaneous].LocalPlayer updated 2025/03/25
    "OFFSET_LOCAL_ENTITY_HANDLE": 0x1ae6c0c, # //[Miscellaneous].LocalEntityHandle updated 2025/04/18
    "OFFSET_NAME_LIST": 0x95a7e90, # //[Miscellaneous].NameList updated 2025/04/18
    "OFFSET_GLOBAL_VARS": 0x01BB40D0, # //[Miscellaneous].GlobalVars updated 2025/03/25
    "OFFSET_LEVELNAME": 0x1BB5694, # //[Miscellaneous].LevelName updated 2025/04/18
    # OFFSET_CLIENTSTATE = OFFSET_LEVELNAME - 0x1c0 => 0x1BB5694 - 0x1c0 = 0x1BB54D4
    "OFFSET_CLIENTSTATE": 0x1BB54D4, # //ClientState  ClientState = LevelName - 0x1C0  //updated 1/10/2024
    # OFFSET_SIGNONSTATE = OFFSET_CLIENTSTATE + 0xAC => 0x1BB54D4 + 0xAC = 0x1BB5580
    "OFFSET_SIGNONSTATE": 0x1BB5580, # //SignonState (ClientState + 0xAC)  //updated 1/10/2024
    # OFFSET_HOST_MAP = 0x01ad0680 + 0x58 => 0x01ad06D8
    "OFFSET_HOST_MAP": 0x01ad06D8, # //[ConVars].host_map + 0x58 updated 2025/04/18
    "OFFSET_TEAM": 0x0338, # //[RecvTable.DT_BaseEntity].m_iTeamNum updated 2025/03/25
    "OFFSET_HEALTH": 0x0328, # //[RecvTable.DT_Player].m_iHealth updated updated 2025/04/18
    "OFFSET_MAXHEALTH": 0x0470, # //[RecvTable.DT_Player].m_iMaxHealth
    "OFFSET_SHIELD": 0x01a0, # //[RecvTable.DT_BaseEntity].m_shieldHealth updated 2024/09/30
    "OFFSET_MAXSHIELD": 0x1A4, # //[RecvTable.DT_BaseEntity].m_shieldHealthMax updated 2024/09/30
    "OFFSET_ARMORTYPE": 0x47ec, # //[RecvTable.DT_Player].m_armorType updated updated 2025/04/18
    "OFFSET_NAME": 0x0481, # //[RecvTable.DT_BaseEntity].m_iName updated 2025/02/25
    "OFFSET_SIGN_NAME": 0x0478, # //[RecvTable.DT_BaseEntity].m_iSignifierName updated 2025/02/25
    "OFFSET_ABS_VELOCITY": 0x0170, # //[DataMap.DT_BaseEntity].m_vecAbsVelocity updated 2024/09/18
    "OFFSET_VISIBLE_TIME": 0x19f0, # //[Miscellaneous].CPlayer!lastVisibleTime updated 2025/04/18
    "OFFSET_LAST_AIMEDAT_TIME": 0x19f0, #//[Miscellaneous].CPlayer!lastVisibleTime + 0x8 (Note: value is same as OFFSET_VISIBLE_TIME, likely needs +0x8 applied if used, 0x19f8)
    "OFFSET_ZOOMING": 0x1c41, # //[RecvTable.DT_Player].m_bZooming updated 2025/04/18
    "OFFSET_VIEW_OFFSET": 0x00e8, # //[DataMap.DT_BaseEntity].m_currentFrame.viewOffset updated 2024/09/18
    # OFFSET_ACTIVE_WEAPON = 0x1928 + 0x0058 => 0x1980
    "OFFSET_ACTIVE_WEAPON": 0x1980, # //[RecvTable.DT_Player].m_inventory + WeaponInventory_Client>activeWeapons updated 2025/04/18
    "OFFSET_OBSERVER_LIST": 0x0256ebe0, # //[ConVars].gamepad_aim_assist_melee + 0x20C8 updated 2025/04/18
    "OFFSET_SPECTATOR_LIST": 0x0256ebe0, # //? updated 2024/11/15 (assigned OFFSET_OBSERVER_LIST)
    "OFFSET_OBSERVER_ARRAY": 0x954, # //[DataMapTypes.DT_GlobalNonRewinding].m_playerObserver - [DataMap.C_ObserverMode].m_observerTarget updated 2024/09/20
    "OFFSET_IN_DUCKSTATE": 0x2ab8, # //[RecvTable.DT_Player].m_duckState updated 2025/04/18
    "OFFSET_IN_DUCK": 0x03371200, # //[Buttons].in_duck updated 2025/04/18
    "OFFSET_TRAVERSAL_PROGRESS": 0x2bc4, # //[DataMap.DT_Player].m_traversalProgress updated 2025/02/12
    "OFFSET_TRAVERSAL_STARTTIME": 0x2bc8, # //[DataMap.DT_Player].m_traversalStartTime updated 2025/02/12
    "OFFSET_TRAVERSAL_RELEASE_TIME": 0x2bd0, # //[RecvTable.DT_LocalPlayerExclusive].m_traversalReleaseTime updated 2025/02/12
    "OFFSET_IN_JUMP": 0x03371110, # //[Buttons].in_jump updated 2025/04/18
    "OFFSET_IN_TOGGLE_DUCK": 0x03371030, # //[Buttons].in_toggle_duck updated 2025/04/18
    "OFFSET_WEAPON_NAME": 0x17e8, # //[RecvTable.DT_WeaponX].m_weaponNameIndex updated 2025/02/25
    "OFFSET_OFF_WEAPON": 0x19a4, # //[DataMap.DT_BaseCombatCharacter].m_latestNonOffhandWeapons updated 2025/02/25
    "OFFSET_WALL_RUN_START_TIME": 0x36fc, # //[RecvTable.DT_LocalPlayerExclusive]->m_wallRunStartTime updated 2025/03/25
    "OFFSET_WALL_RUN_CLEAR_TIME": 0x3700, # //[RecvTable.DT_LocalPlayerExclusive]->m_wallRunClearTime updated 2025/03/25
    "OFFSET_FLAGS": 0x00c8, # //[DataMap.DT_Player].m_fFlags updated 2025/03/25
    "OFFSET_IN_ATTACK": 0x03370ff0, # //[Buttons].in_attack updated 2025/04/18
    "OFFSET_IN_ZOOM": 0x03371188, # //[Buttons].in_zoom updated 2025/04/18
    "OFFSET_IN_FORWARD": 0x03370f40, # //[Buttons].in_forward updated 2025/04/18
    "OFFSET_IN_BACKWARD": 0x03370f68, # //[Buttons].in_backward updated 2025/04/18
    "OFFSET_LIFE_STATE": 0x0690, # //[RecvTable.DT_Player].m_lifeState updated 2025/04/18
    "OFFSET_BLEED_OUT_STATE": 0x27b8, # //[RecvTable.DT_Player].m_bleedoutState updated 2025/04/18
    "OFFSET_ORIGIN": 0x017c, # //[DataMap.DT_BaseEntity].m_vecAbsOrigin updated 2025/01/08
    # OFFSET_BONES = 0x0d88 + 0x48 => 0x0DD0
    "OFFSET_BONES": 0x0DD0, # //[RecvTable.DT_BaseAnimating].m_nForceBone + 0x48 updated 2025/04/18
    "OFFSET_STUDIOHDR": 0xfd0, # //[Miscellaneous].CBaseAnimating!m_pStudioHdr updated 2025/04/18
    "OFFSET_AIMPUNCH": 0x2438, # //[DataMap.DT_Player].m_currentFrameLocalPlayer.m_vecPunchWeapon_Angle updated 2024/09/18
    "OFFSET_CAMERAPOS": 0x1f40, # //[Miscellaneous].CPlayer!camera_origin updated 2025/04/18
    # OFFSET_VIEWANGLES = 0x2594 - 0x14 => 0x2580
    "OFFSET_VIEWANGLES": 0x2580, # //[RecvTable.DT_Player].m_ammoPoolCapacity - 0x14 updated 2025/04/18
    # OFFSET_BREATH_ANGLES = OFFSET_VIEWANGLES - 0x10 => 0x2580 - 0x10 = 0x2570
    "OFFSET_BREATH_ANGLES": 0x2570, # //updated 01/10/2024
    "OFFSET_OBSERVER_MODE": 0x35e4, # //[RecvTable.DT_LocalPlayerExclusive].m_iObserverMode updated 2025/02/25
    "OFFSET_OBSERVING_TARGET": 0x35f0, # //[RecvTable.DT_LocalPlayerExclusive].m_hObserverTarget updated 2025/02/25
    "OFFSET_IN_USE": 0x03371178, # //[Buttons].in_use updated 2025/04/18
    "OFFSET_MATRIX": 0x11a350, # //[Miscellaneous].ViewMatrix updated 2025/04/18
    "OFFSET_RENDER": 0x3370ee0, # //[Miscellaneous].ViewRender updated 2025/04/18
    "OFFSET_WEAPON": 0x1994, # //[RecvTable.DT_BaseCombatCharacter].m_latestPrimaryWeapons updated 2025/04/18
    "OFFSET_BULLET_SPEED": 0x2790, # //0x19d8 + 0x04ec //0x1eb4 + 0x19d0 //[Miscellaneous].CWeaponX!m_flProjectileSpeed updated 2025/04/18
    "OFFSET_BULLET_SCALE": 0x2798, # //0x19d8 + 0x04f4 //0x1ebc + 0x19d0 //[Miscellaneous].CWeaponX!m_flProjectileScale updated 2025/04/18
    "OFFSET_ZOOM_FOV": 0x16ec, # //[RecvTable.DT_WeaponX].m_playerData + m_curZoomFOV updated 2025/02/25
    "OFFSET_AMMO": 0x15b0, # //[RecvTable.DT_PropSurvival].m_ammoInClip updated 2025/04/18
    "OFFSET_ITEM_ID": 0x15b4, # //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2025/04/18
    "OFFSET_MODELNAME": 0x0030, # //[DataMap.DT_BaseEntity].m_ModelName updated 2024/09/18
    "OFFSET_M_CUSTOMSCRIPTINT": 0x15b4, # //[RecvTable.DT_PropSurvival].m_customScriptInt updated 2025/04/18 (assigned OFFSET_ITEM_ID)
    # OFFSET_YAW = 0x2294 - 0x8 => 0x228C
    "OFFSET_YAW": 0x228C, # //[DataMap.DT_Player].m_currentFramePlayer.m_ammoPoolCount - 0x8 updated 2025/02/12
    "OFFSET_GLOW_ENABLE": 0x29c, # //0x27c //0x28c ?//[RecvTable.DT_HighlightSettings].m_highlightGenericContexts - 0x1 updated 2024/08/31
    "OFFSET_GLOW_THROUGH_WALLS": 0x26c, # ////[RecvTable.DT_HighlightSettings.m_highlightGenericContexts - 0x21 updated 2024/07/23
    "OFFSET_TIME_BASE": 0x20E8, # //[DataMap.DT_Player].m_currentFramePlayer.timeBase updated 2025/02/12
    # GLOW_START_TIME = 0x02c8 + 0x30 => 0x2F8
    "GLOW_START_TIME": 0x02F8, # //m_playerFloatLookStartTime=0x02c4 //updated 01/10/2024 DataMap.C_PlayerLocalData
    "OFFSET_HIGHLIGHTSERVERACTIVESTATES": 0x29C, # //[DT_HighlightSettings].m_highlightServerActiveStates updated 2024/06/26
    "OFFSET_GLOW_FIX": 0x278, # //0x268 //updated 2025/02/25
    "OFFSET_GLOW_ENABLE_GLOW_CONTEXT": 0x29c, # // Script_Highlight_SetCurrentContext updated 01/10/2024 (assigned OFFSET_GLOW_ENABLE)
    "OFFSET_GLOW_CONTEXT_ID": 0x29c, # //updated 6/26/2024
    "OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE": 0x26c, # // Script_Highlight_SetVisibilityType 5th mov updated 01/10/2024 (assigned OFFSET_GLOW_THROUGH_WALLS)
    # GLOW_LIFE_TIME = 0x3A4 + 0x30 => 0x3D4
    "GLOW_LIFE_TIME": 0x3D4, # // Script_Highlight_SetLifeTime + 4 updated 01/10/2024
    "GLOW_DISTANCE": 0x264, # //0x294 //0x26c // Script_Highlight_SetFarFadeDist updated 01/10/2024
    "GLOW_TYPE": 0x29c, # // Script_Highlight_GetState + 4 updated 01/10/2024
    # GLOW_COLOR = 0x1D0 + 0x30 => 0x200
    "GLOW_COLOR": 0x200, # // Script_CopyHighlightState 15th mov updated 01/10/2024
    # GLOW_FADE = 0x388 + 0x30 => 0x3B8
    "GLOW_FADE": 0x3B8, # // ats 3rd result of 3 offsets consecutive or first + 8 updated 01/10/2024
    "HIGHLIGHT_SETTINGS": 0x72b6430, # //[Miscellaneous].HighlightSettings updated 2025/04/18
    "HIGHLIGHT_TYPE_SIZE": 0x34, # //? updated 01/10/2024
    # OFFSET_CROSSHAIR_LAST = OFFSET_VISIBLE_TIME + 0x19f8 => 0x19f0 + 0x19f8 = 0x33E8
    "OFFSET_CROSSHAIR_LAST": 0x33E8, # //[Miscellaneous].CWeaponX!lastCrosshairTargetTime updated 2025/04/18
    "OFFSET_INPUT_SYSTEM": 0x1c64440, # //[Miscellaneous].InputSystem updated 2025/04/18
    "OFFSET_SKYDIVE_STATE": 0x484c, # //[RecvTable.DT_Player].m_skydiveState updated 2025/04/18
    "OFFSET_GRAPPLEACTIVED": 0x2d90, # //[RecvTable.DT_Player].m_grappleActive updated 2025/04/18
    "OFFSET_GRAPPLE": 0x2d08, # //[RecvTable.DT_Player].m_grapple updated 2025/04/18
    "OFFSET_GRAPPLEATTACHED": 0x0048, # //[RecvTable.DT_GrappleData].m_grappleAttached updated 2025/02/25
    "OFFSET_m_xp": 0x380c, # //[RecvTable.DT_Player].m_xp updated 2025/04/18
    "OFFSET_GRADE": 0x0348, # //[RecvTable.DT_BaseEntity].m_grade updated 2025/02/25
}

# Note: Some offsets were defined in terms of others.
# Example: OFFSET_CLIENTSTATE = OFFSET_LEVELNAME - 0x1c0
# These have been pre-calculated.
# OFFSET_LAST_AIMEDAT_TIME was OFFSET_VISIBLE_TIME + 0x8, so it might be 0x19f8. Currently it's 0x19f0.
# This was manually calculated for inclusion. If the base value (OFFSET_VISIBLE_TIME) changes, dependent values need recalculation.
# A more robust parsing script could handle this dynamically.
# For OFFSET_LAST_AIMEDAT_TIME, the comment suggests it's OFFSET_VISIBLE_TIME + 0x8.
# OFFSET_VISIBLE_TIME is 0x19f0. So, 0x19f0 + 0x8 = 0x19f8.
# I will update OFFSET_LAST_AIMEDAT_TIME to be 0x19f8.
GAME_OFFSETS["OFFSET_LAST_AIMEDAT_TIME"] = 0x19f8
GAME_OFFSETS["OFFSET_SPECTATOR_LIST"] = GAME_OFFSETS["OFFSET_OBSERVER_LIST"]
GAME_OFFSETS["OFFSET_M_CUSTOMSCRIPTINT"] = GAME_OFFSETS["OFFSET_ITEM_ID"]
GAME_OFFSETS["OFFSET_GLOW_ENABLE_GLOW_CONTEXT"] = GAME_OFFSETS["OFFSET_GLOW_ENABLE"]
GAME_OFFSETS["OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE"] = GAME_OFFSETS["OFFSET_GLOW_THROUGH_WALLS"]

# Verification of some calculated values:
# OFFSET_LEVELNAME = 0x1BB5694
# OFFSET_CLIENTSTATE = 0x1BB5694 - 0x1c0 = 0x1BB54D4 (Correct)
# OFFSET_SIGNONSTATE = 0x1BB54D4 + 0xAC = 0x1BB5580 (Correct)
# OFFSET_ACTIVE_WEAPON = 0x1928 + 0x0058 = 0x1980 (Correct)
# OFFSET_BONES = 0x0d88 + 0x48 = 0xDD0 (Correct)
# OFFSET_VIEWANGLES = 0x2594 - 0x14 = 0x2580 (Correct)
# OFFSET_BREATH_ANGLES = 0x2580 - 0x10 = 0x2570 (Correct)
# OFFSET_YAW = 0x2294 - 0x8 = 0x228C (Correct)
# GLOW_START_TIME = 0x02c8 + 0x30 = 0x02F8 (Correct)
# OFFSET_CROSSHAIR_LAST = 0x19f0 + 0x19f8 = 0x33E8 (Correct)
# GLOW_LIFE_TIME = 0x3A4 + 0x30 = 0x3D4 (Correct)
# GLOW_COLOR = 0x1D0 + 0x30 = 0x200 (Correct)
# GLOW_FADE = 0x388 + 0x30 = 0x3B8 (Correct)
# OFFSET_HOST_MAP = 0x01ad0680 + 0x58 = 0x01AD06D8 (Correct)

# The following offsets were defined as aliases or had more complex dependencies
# not automatically resolved by simple value replacement.
# They are listed here for completeness or manual review if needed.
# "#define OFFSET_CLIENTSTATE OFFSET_LEVELNAME - 0x1c0" (Calculated)
# "#define OFFSET_SIGNONSTATE OFFSET_CLIENTSTATE + 0xAC" (Calculated)
# "#define OFFSET_HOST_MAP 0x01ad0680 + 0x58" (Calculated)
# "#define OFFSET_SPECTATOR_LIST OFFSET_OBSERVER_LIST" (Aliased)
# "#define OFFSET_ACTIVE_WEAPON 0x1928 + 0x0058" (Calculated)
# "#define OFFSET_BONES 0x0d88 + 0x48" (Calculated)
# "#define OFFSET_VIEWANGLES 0x2594 - 0x14" (Calculated)
# "#define OFFSET_BREATH_ANGLES OFFSET_VIEWANGLES - 0x10" (Calculated)
# "#define OFFSET_M_CUSTOMSCRIPTINT OFFSET_ITEM_ID" (Aliased)
# "#define OFFSET_YAW 0x2294 - 0x8" (Calculated)
# "#define GLOW_START_TIME 0x02c8 + 0x30" (Calculated)
# "#define OFFSET_GLOW_ENABLE_GLOW_CONTEXT OFFSET_GLOW_ENABLE" (Aliased)
# "#define OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE OFFSET_GLOW_THROUGH_WALLS" (Aliased)
# "#define GLOW_LIFE_TIME 0x3A4+ 0x30" (Calculated)
# "#define GLOW_COLOR 0x1D0+ 0x30" (Calculated)
# "#define GLOW_FADE 0x388+ 0x30" (Calculated)
# "#define OFFSET_CROSSHAIR_LAST OFFSET_VISIBLE_TIME + 0x19f8" (Calculated)

# It's important to note that if the base values change in offsets.h, the calculated values here
# will need to be updated manually or by re-running a more sophisticated parsing script.
# The initial parsing only handles direct hex values.
# The arithmetic has been manually calculated and verified for this version.

# Check for any non-integer values that might have slipped through:
for key, value in GAME_OFFSETS.items():
    if not isinstance(value, int):
        print(f"Warning: Offset {key} has a non-integer value: {value}")

if __name__ == "__main__":
    # Print all offsets to verify
    for name, val in GAME_OFFSETS.items():
        print(f'"{name}": {hex(val)},')
    print(f"Total offsets parsed: {len(GAME_OFFSETS)}")

    # Verify a few known calculated values
    # Expected: OFFSET_CLIENTSTATE = OFFSET_LEVELNAME (0x1BB5694) - 0x1c0 = 0x1BB54D4
    if GAME_OFFSETS["OFFSET_LEVELNAME"] - 0x1c0 == GAME_OFFSETS["OFFSET_CLIENTSTATE"]:
        print("OFFSET_CLIENTSTATE calculation verified.")
    else:
        print("OFFSET_CLIENTSTATE calculation MISMATCH.")

    # Expected: OFFSET_VIEWANGLES = 0x2594 - 0x14 = 0x2580
    if 0x2594 - 0x14 == GAME_OFFSETS["OFFSET_VIEWANGLES"]:
        print("OFFSET_VIEWANGLES calculation verified.")
    else:
        print("OFFSET_VIEWANGLES calculation MISMATCH.")

    if GAME_OFFSETS["OFFSET_LAST_AIMEDAT_TIME"] == 0x19f8:
        print("OFFSET_LAST_AIMEDAT_TIME updated correctly.")
    else:
        print("OFFSET_LAST_AIMEDAT_TIME MISMATCH after update.")

    # Check aliases
    if GAME_OFFSETS["OFFSET_SPECTATOR_LIST"] == GAME_OFFSETS["OFFSET_OBSERVER_LIST"]:
        print("OFFSET_SPECTATOR_LIST alias verified.")
    else:
        print("OFFSET_SPECTATOR_LIST alias MISMATCH.")

    print(f"OFFSET_VISIBLE_TIME: {hex(GAME_OFFSETS['OFFSET_VISIBLE_TIME'])}")
    print(f"OFFSET_LAST_AIMEDAT_TIME: {hex(GAME_OFFSETS['OFFSET_LAST_AIMEDAT_TIME'])}")

```
