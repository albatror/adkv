#include "offsets_dynamic.h"
#include <cstdio>
#include <fstream>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

DynamicOffsets offsets = {0};

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
    if (off_data.count("[.Miscellaneous]CPlayer!lastVisibleTime")) offsets.VisibleTime = off_data["[.Miscellaneous]CPlayer!lastVisibleTime"];
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

    // ViewAngles is tricky, often derived
    // if (off_data.count("[.Miscellaneous]ViewAngles")) offsets.ViewAngles = off_data["[.Miscellaneous]ViewAngles"];

    if (but_data.count("[Buttons]in_duck")) offsets.InDuck = but_data["[Buttons]in_duck"];
    if (but_data.count("[Buttons]in_jump")) offsets.InJump = but_data["[Buttons]in_jump"];
    if (but_data.count("[Buttons]in_attack")) offsets.InAttack = but_data["[Buttons]in_attack"];
    if (but_data.count("[Buttons]in_zoom")) offsets.InZoom = but_data["[Buttons]in_zoom"];
    if (but_data.count("[Buttons]in_forward")) offsets.InForward = but_data["[Buttons]in_forward"];
    if (but_data.count("[Buttons]in_backward")) offsets.InBackward = but_data["[Buttons]in_backward"];
    if (but_data.count("[Buttons]in_use")) offsets.InUse = but_data["[Buttons]in_use"];
    if (but_data.count("[Buttons]in_toggle_duck")) offsets.InToggleDuck = but_data["[Buttons]in_toggle_duck"];

    if (con_data.count("[ConVars]host_map")) offsets.HostMap = con_data["[ConVars]host_map"] + 0x58;

    // Load Weapon IDs
    if (off_data.count("[WeaponIds]mp_weapon_kraber")) offsets.ID_KRABER = off_data["[WeaponIds]mp_weapon_kraber"]; else offsets.ID_KRABER = 98;
    if (off_data.count("[WeaponIds]mp_weapon_sentinel")) offsets.ID_SENTINEL = off_data["[WeaponIds]mp_weapon_sentinel"]; else offsets.ID_SENTINEL = 1;
    if (off_data.count("[WeaponIds]mp_weapon_wingman")) offsets.ID_WINGMAN = off_data["[WeaponIds]mp_weapon_wingman"]; else offsets.ID_WINGMAN = 117;
    if (off_data.count("[WeaponIds]mp_weapon_sniper")) offsets.ID_LONGBOW = off_data["[WeaponIds]mp_weapon_sniper"];
    else if (off_data.count("[WeaponIds]mp_weapon_dmr")) offsets.ID_LONGBOW = off_data["[WeaponIds]mp_weapon_dmr"];
    else offsets.ID_LONGBOW = 89;
    if (off_data.count("[WeaponIds]mp_weapon_g7")) offsets.ID_G7 = off_data["[WeaponIds]mp_weapon_g7"]; else offsets.ID_G7 = 95;
    if (off_data.count("[WeaponIds]mp_weapon_hemlok")) offsets.ID_HEMLOCK = off_data["[WeaponIds]mp_weapon_hemlok"]; else offsets.ID_HEMLOCK = 96;
    if (off_data.count("[WeaponIds]mp_weapon_3030")) offsets.ID_3030 = off_data["[WeaponIds]mp_weapon_3030"]; else offsets.ID_3030 = 120;
    if (off_data.count("[WeaponIds]mp_weapon_triple_take")) offsets.ID_TRIPLE_TAKE = off_data["[WeaponIds]mp_weapon_triple_take"]; else offsets.ID_TRIPLE_TAKE = 116;
    if (off_data.count("[WeaponIds]mp_weapon_bow")) offsets.ID_BOCEK = off_data["[WeaponIds]mp_weapon_bow"]; else offsets.ID_BOCEK = 182;
    if (off_data.count("[WeaponIds]mp_weapon_throwing_knife")) offsets.ID_THROWING_KNIFE = off_data["[WeaponIds]mp_weapon_throwing_knife"]; else offsets.ID_THROWING_KNIFE = 2;
    if (off_data.count("[WeaponIds]mp_weapon_p2020")) offsets.ID_P2020 = off_data["[WeaponIds]mp_weapon_p2020"]; else offsets.ID_P2020 = 114;
    if (off_data.count("[WeaponIds]mp_weapon_mozambique")) offsets.ID_MOZAMBIQUE = off_data["[WeaponIds]mp_weapon_mozambique"]; else offsets.ID_MOZAMBIQUE = 103;
    if (off_data.count("[WeaponIds]mp_weapon_eva8")) offsets.ID_EVA8 = off_data["[WeaponIds]mp_weapon_eva8"]; else offsets.ID_EVA8 = 92;
    if (off_data.count("[WeaponIds]mp_weapon_peacekeeper")) offsets.ID_PEACEKEEPER = off_data["[WeaponIds]mp_weapon_peacekeeper"]; else offsets.ID_PEACEKEEPER = 111;
    if (off_data.count("[WeaponIds]mp_weapon_mastiff")) offsets.ID_MASTIFF = off_data["[WeaponIds]mp_weapon_mastiff"]; else offsets.ID_MASTIFF = 101;
    if (off_data.count("[WeaponIds]mp_weapon_nemesis")) offsets.ID_NEMESIS = off_data["[WeaponIds]mp_weapon_nemesis"]; else offsets.ID_NEMESIS = 122;

    printf("Offsets updated from INI files.\n");
    return true;
}
