#pragma once
#include <string>

namespace ItemID {
    enum Enum : int {
        blank = 0,
        mp_weapon_sniper = 1,
        mp_weapon_mastiff = 2,
        mp_weapon_mastiff_whiteset = 3,
        mp_weapon_mastiff_blueset = 4,
        mp_weapon_mastiff_purpleset = 5,
        mp_weapon_mastiff_gold = 6,
        mp_weapon_mastiff_crate = 7,
        mp_weapon_lstar = 8,
        mp_weapon_lstar_crate = 9,
        mp_weapon_lstar_whiteset = 10,
        mp_weapon_lstar_blueset = 11,
        mp_weapon_lstar_purpleset = 12,
        mp_weapon_lstar_gold = 13,
        mp_weapon_energy_ar = 14,
        mp_weapon_energy_ar_whiteset = 15,
        mp_weapon_energy_ar_blueset = 16,
        mp_weapon_energy_ar_purpleset = 17,
        mp_weapon_energy_ar_gold = 18,
        mp_weapon_energy_ar_crate = 19,
        mp_weapon_esaw = 20,
        mp_weapon_esaw_crate = 21,
        mp_weapon_esaw_whiteset = 22,
        mp_weapon_esaw_blueset = 23,
        mp_weapon_esaw_purpleset = 24,
        mp_weapon_esaw_gold = 25,
        mp_weapon_doubletake = 26,
        mp_weapon_doubletake_crate = 27,
        mp_weapon_doubletake_whiteset = 28,
        mp_weapon_doubletake_blueset = 29,
        mp_weapon_doubletake_purpleset = 30,
        mp_weapon_doubletake_gold = 31,
        mp_weapon_vinson = 32,
        mp_weapon_vinson_whiteset = 33,
        mp_weapon_vinson_blueset = 34,
        mp_weapon_vinson_purpleset = 35,
        mp_weapon_vinson_gold = 36,
        mp_weapon_hemlok = 37,
        mp_weapon_hemlok_crate = 38,
        mp_weapon_hemlok_whiteset = 39,
        mp_weapon_hemlok_blueset = 40,
        mp_weapon_hemlok_purpleset = 41,
        mp_weapon_hemlok_gold = 42,
        mp_weapon_auto_hemlok = 43,
        mp_weapon_auto_hemlok_whiteset = 44,
        mp_weapon_auto_hemlok_blueset = 45,
        mp_weapon_auto_hemlok_purpleset = 46,
        mp_weapon_auto_hemlok_gold = 47,
        mp_weapon_g2 = 48,
        mp_weapon_g2_crate = 49,
        mp_weapon_g2_whiteset = 50,
        mp_weapon_g2_blueset = 51,
        mp_weapon_g2_purpleset = 52,
        mp_weapon_g2_gold = 53,
        mp_weapon_alternator_smg = 54,
        mp_weapon_alternator_smg_crate = 55,
        mp_weapon_alternator_smg_whiteset = 56,
        mp_weapon_alternator_smg_blueset = 57,
        mp_weapon_alternator_smg_purpleset = 58,
        mp_weapon_alternator_smg_gold = 59,
        mp_weapon_r97 = 60,
        mp_weapon_r97_whiteset = 61,
        mp_weapon_r97_blueset = 62,
        mp_weapon_r97_purpleset = 63,
        mp_weapon_r97_gold = 64,
        mp_weapon_r97_crate = 65,
        mp_weapon_pdw = 66,
        mp_weapon_pdw_crate = 67,
        mp_weapon_pdw_whiteset = 68,
        mp_weapon_pdw_blueset = 69,
        mp_weapon_pdw_purpleset = 70,
        mp_weapon_pdw_gold = 71,
        mp_weapon_volt_smg = 72,
        mp_weapon_volt_smg_whiteset = 73,
        mp_weapon_volt_smg_blueset = 74,
        mp_weapon_volt_smg_purpleset = 75,
        mp_weapon_volt_smg_gold = 76,
        mp_weapon_dmr = 77,
        mp_weapon_dmr_whiteset = 78,
        mp_weapon_dmr_blueset = 79,
        mp_weapon_dmr_purpleset = 80,
        mp_weapon_dmr_gold = 81,
        mp_weapon_defender = 82,
        mp_weapon_defender_whiteset = 83,
        mp_weapon_defender_blueset = 84,
        mp_weapon_defender_purpleset = 85,
        mp_weapon_defender_gold = 86,
        mp_weapon_lmg = 87,
        mp_weapon_lmg_whiteset = 88,
        mp_weapon_lmg_blueset = 89,
        mp_weapon_lmg_purpleset = 90,
        mp_weapon_lmg_gold = 91,
        mp_weapon_rspn101 = 92,
        mp_weapon_rspn101_whiteset = 93,
        mp_weapon_rspn101_blueset = 94,
        mp_weapon_rspn101_purpleset = 95,
        mp_weapon_rspn101_gold = 96,
        mp_weapon_shotgun = 97,
        mp_weapon_shotgun_crate = 98,
        mp_weapon_shotgun_whiteset = 99,
        mp_weapon_shotgun_blueset = 100,
        mp_weapon_shotgun_purpleset = 101,
        mp_weapon_shotgun_gold = 102,
        mp_weapon_energy_shotgun = 103,
        mp_weapon_energy_shotgun_crate = 104,
        mp_weapon_energy_shotgun_whiteset = 105,
        mp_weapon_energy_shotgun_blueset = 106,
        mp_weapon_energy_shotgun_purpleset = 107,
        mp_weapon_energy_shotgun_gold = 108,
        mp_weapon_shotgun_pistol = 109,
        mp_weapon_shotgun_pistol_whiteset = 110,
        mp_weapon_shotgun_pistol_blueset = 111,
        mp_weapon_shotgun_pistol_purpleset = 112,
        mp_weapon_shotgun_pistol_gold = 113,
        mp_weapon_shotgun_pistol_akimbo_whiteset = 114,
        mp_weapon_shotgun_pistol_akimbo_blueset = 115,
        mp_weapon_shotgun_pistol_akimbo_purpleset = 116,
        mp_weapon_shotgun_pistol_akimbo_gold = 117,
        mp_weapon_shotgun_pistol_light = 118,
        mp_weapon_shotgun_pistol_heavy = 119,
        mp_weapon_shotgun_pistol_sniper = 120,
        mp_weapon_shotgun_pistol_energy = 121,
        mp_weapon_wingman = 122,
        mp_weapon_wingman_crate = 123,
        mp_weapon_wingman_whiteset = 124,
        mp_weapon_wingman_blueset = 125,
        mp_weapon_wingman_purpleset = 126,
        mp_weapon_wingman_gold = 127,
        mp_weapon_semipistol = 128,
        mp_weapon_semipistol_whiteset = 129,
        mp_weapon_semipistol_blueset = 130,
        mp_weapon_semipistol_purpleset = 131,
        mp_weapon_semipistol_gold = 132,
        mp_weapon_semipistol_crate = 133,
        mp_weapon_semipistol_crate_akimbo = 134,
        mp_weapon_semipistol_akimbo_whiteset = 135,
        mp_weapon_semipistol_akimbo_blueset = 136,
        mp_weapon_semipistol_akimbo_purpleset = 137,
        mp_weapon_semipistol_akimbo_gold = 138,
        mp_weapon_autopistol = 139,
        mp_weapon_autopistol_crate = 140,
        mp_weapon_autopistol_fusion = 141,
        mp_weapon_autopistol_fusion_whiteset = 142,
        mp_weapon_autopistol_fusion_blueset = 143,
        mp_weapon_autopistol_fusion_purpleset = 144,
        mp_weapon_autopistol_fusion_gold = 145,
        mp_weapon_autopistol_whiteset = 146,
        mp_weapon_autopistol_blueset = 147,
        mp_weapon_autopistol_purpleset = 148,
        mp_weapon_autopistol_gold = 149,
        mp_weapon_sentinel = 150,
        mp_weapon_sentinel_whiteset = 151,
        mp_weapon_sentinel_blueset = 152,
        mp_weapon_sentinel_purpleset = 153,
        mp_weapon_sentinel_gold = 154,
        mp_weapon_epg = 155,
        mp_weapon_epg_tethered = 156,
        mp_weapon_bow = 157,
        mp_weapon_bow_crate = 158,
        mp_weapon_bow_cupid = 159,
        mp_weapon_bow_whiteset = 160,
        mp_weapon_bow_blueset = 161,
        mp_weapon_bow_purpleset = 162,
        mp_weapon_bow_gold = 163,
        mp_weapon_3030 = 164,
        mp_weapon_3030_whiteset = 165,
        mp_weapon_3030_blueset = 166,
        mp_weapon_3030_purpleset = 167,
        mp_weapon_3030_gold = 168,
        mp_weapon_nemesis = 169,
        mp_weapon_nemesis_whiteset = 170,
        mp_weapon_nemesis_blueset = 171,
        mp_weapon_nemesis_purpleset = 172,
        mp_weapon_nemesis_gold = 173,
        bullet = 174,
        special = 175,
        shotgun = 176,
        highcal = 177,
        sniper = 178,
        arrows = 179,
        arrowsBundle = 180,
        mp_weapon_dragon_lmg = 181,
        mp_weapon_dragon_lmg_crate = 182,
        mp_weapon_dragon_lmg_whiteset = 183,
        mp_weapon_dragon_lmg_blueset = 184,
        mp_weapon_dragon_lmg_purpleset = 185,
        mp_weapon_dragon_lmg_gold = 186,
        mp_weapon_car = 187,
        mp_weapon_car_crate = 188,
        mp_weapon_car_whiteset = 189,
        mp_weapon_car_blueset = 190,
        mp_weapon_car_purpleset = 191,
        mp_weapon_car_gold = 192,
        mp_weapon_semipistol_paintball_akimbo_blueset = 193,
        mp_weapon_shotgun_pistol_paintball_akimbo_blueset = 194,
        mp_weapon_alternator_smg_paintball_blueset = 195,
        mp_weapon_g2_paintball_blueset = 196,
        mp_weapon_hemlok_paintball_blueset = 197,
        mp_weapon_r97_paintball_blueset = 198,
        mp_weapon_mastiff_paintball_blueset = 199,
        mp_weapon_3030_paintball_blueset = 200,
        mp_weapon_autopistol_paintball_purpleset = 201,
        mp_weapon_3030_paintball_purpleset = 202,
        mp_weapon_wingman_paintball_purpleset = 203,
        mp_weapon_shotgun_paintball_purpleset = 204,
        mp_weapon_lmg_paintball_purpleset = 205,
        mp_weapon_dragon_lmg_paintball_purpleset = 206,
        mp_weapon_vinson_paintball_purpleset = 207,
        mp_weapon_pdw_paintball_purpleset = 208,
        mp_weapon_dmr_paintball_purpleset = 209,
        mp_weapon_3030_paintball_gold = 210,
        mp_weapon_dragon_lmg_paintball_gold = 211,
        mp_weapon_autopistol_paintball_gold = 212,
        mp_weapon_semipistol_paintball_gold = 213,
        mp_weapon_shotgun_pistol_paintball_gold = 214,
        mp_weapon_wingman_paintball_gold = 215,
        mp_weapon_shotgun_paintball_gold = 216,
        mp_weapon_mastiff_paintball_gold = 217,
        mp_weapon_pdw_paintball_gold = 218,
        mp_weapon_r97_paintball_gold = 219,
        mp_weapon_vinson_paintball_gold = 220,
        mp_weapon_hemlok_paintball_gold = 221,
        mp_weapon_rspn101_paintball_gold = 222,
        mp_weapon_g2_paintball_gold = 223,
        mp_weapon_dmr_paintball_gold = 224,
        health_pickup_ultimate = 225,
        health_pickup_combo_full = 226,
        health_pickup_health_large = 227,
        health_pickup_health_small = 228,
        health_pickup_combo_large = 229,
        health_pickup_combo_small = 230,
        helmet_pickup_lv1 = 231,
        helmet_pickup_lv2 = 232,
        helmet_pickup_lv3 = 233,
        helmet_pickup_lv4_abilities = 234,
        helmet_pickup_lv4_armor_upgrade = 235,
        helmet_pickup_lv5_armor_upgrade = 236,
        helmet_pickup_lv5_sunglasses = 237,
        armor_pickup_lv0 = 238,
        armor_pickup_lv1 = 239,
        armor_pickup_lv2 = 240,
        armor_pickup_lv3 = 241,
        armor_pickup_lv4_all_fast = 242,
        armor_pickup_lv0_evolving = 243,
        armor_pickup_lv1_evolving = 244,
        armor_pickup_lv2_evolving = 245,
        armor_pickup_lv3_evolving = 246,
        armor_pickup_lv5_evolving = 247,
        armor_core_pickup_lv1 = 248,
        armor_core_pickup_lv2 = 249,
        armor_core_pickup_lv3 = 250,
        armor_core_pickup_lv5 = 251,
        incapshield_pickup_lv0 = 252,
        incapshield_pickup_lv1 = 253,
        incapshield_pickup_lv2 = 254,
        incapshield_pickup_lv3 = 255,
        incapshield_pickup_lv4_selfrevive = 256,
        backpack_pickup_lv1 = 257,
        backpack_pickup_lv2 = 258,
        backpack_pickup_lv3 = 259,
        backpack_pickup_lv4_revive_boost = 260,
        mp_weapon_throwingknife = 261,
        mp_weapon_thermite_grenade = 262,
        mp_weapon_frag_grenade = 263,
        mp_weapon_grenade_emp = 264,
        optic_cq_hcog_classic = 265,
        optic_cq_hcog_bruiser = 266,
        optic_cq_holosight = 267,
        optic_cq_holosight_variable = 268,
        optic_cq_threat = 269,
        optic_ranged_hcog = 270,
        optic_ranged_aog_variable = 271,
        optic_sniper = 272,
        optic_sniper_variable = 273,
        optic_sniper_threat = 274,
        barrel_stabilizer_l1 = 275,
        barrel_stabilizer_l2 = 276,
        barrel_stabilizer_l3 = 277,
        barrel_hemlok_elite = 278,
        laser_sight_l1 = 279,
        laser_sight_l2 = 280,
        laser_sight_l3 = 281,
        laser_sight_l4 = 282,
        bullets_mag_l1 = 283,
        bullets_mag_l2 = 284,
        bullets_mag_l3 = 285,
        bullets_mag_l4 = 286,
        highcal_mag_l1 = 287,
        highcal_mag_l2 = 288,
        highcal_mag_l3 = 289,
        highcal_mag_l4 = 290,
        energy_mag_l1 = 291,
        energy_mag_l2 = 292,
        energy_mag_l3 = 293,
        energy_mag_l4 = 294,
        sniper_mag_l1 = 295,
        sniper_mag_l2 = 296,
        sniper_mag_l3 = 297,
        sniper_mag_l4 = 298,
        shotgun_bolt_l1 = 299,
        shotgun_bolt_l2 = 300,
        shotgun_bolt_l3 = 301,
        shotgun_bolt_l4 = 302,
        stock_tactical_l1 = 303,
        stock_tactical_l2 = 304,
        stock_tactical_l3 = 305,
        stock_sniper_l1 = 306,
        stock_sniper_l2 = 307,
        stock_sniper_l3 = 308,
        stock_sniper_l4 = 309,
        hopup_turbocharger = 310,
        hopup_selectfire = 311,
        hopup_dual_loader = 312,
        hopup_headshot_dmg = 313,
        hopup_headshot_dmg_elite = 314,
        hopup_headshot_dmg_elite_ghorse = 315,
        hopup_shield_breaker = 316,
        hopup_unshielded_dmg = 317,
        hopup_double_tap = 318,
        hopup_paintball = 319,
        paintball = 320,
        hopup_quickdraw_holster = 321,
        hopup_shatter_rounds = 322,
        hopup_kinetic_loader = 323,
        hopup_smart_reload = 324,
        hopup_gunshield = 325,
        hopup_ultimate_accelerator = 326,
        hopup_brand_weapon_takeover = 327,
        hopup_paintball_elite = 328,
        dormant_hopup_gunshield = 329,
        dormant_hopup_selectfire = 330,
        dormant_hopup_highcal_rounds = 331,
        dormant_hopup_headshot_dmg = 332,
        dormant_hopup_ultimate_accelerator = 333,
        dormant_hopup_quickdraw_holster = 334,
        dormant_hopup_dual_loader = 335,
        dormant_hopup_unshielded_dmg = 336,
        dormant_hopup_shield_breaker = 337,
        dormant_hopup_double_tap = 338,
        dormant_hopup_turbocharger = 339,
        dormant_hopup_paintball_elite = 340,
        hopup_points_pickup = 341,
        mp_ability_mobile_respawn_beacon = 342,
        data_knife = 343,
        s4t_item0 = 344,
        treasure_box = 345,
        doubler_flag = 346,
        quadrupler_flag = 347,
        mp_ability_void_ring = 348,
        loot_marvin_arm = 349,
        ship_keycard = 350,
        mp_ability_redeploy_balloon = 351,
        expired_banners = 352,
        black_market_banner = 353,
        mp_weapon_titan_sword = 354,
        fr_nessie = 355,
        fr_nessie_yellow = 356,
        fr_nessie_pink = 357,
        fr_nessie_turquoise = 358,
        fr_nessie_blue = 359,
        fr_nessie_red = 360,
        fr_nessie_moy = 361,
        fr_nessie_large = 362,
        fr_nessie_lavender = 363,
        fr_nessie_white = 364,
        fr_nessie_orange = 365,
        fr_nessie_black = 366,
        fr_nessie_gray = 367,
        mp_weapon_grenade_rev_shell = 368,
        mp_weapon_grenade_pathy_shell = 369,
        hopup_golden_horse_blue = 370,
        hopup_golden_horse_green = 371,
        hopup_golden_horse_yellow = 372,
        hopup_golden_horse_purple = 373,
        hopup_golden_horse_red = 374,
        mp_pickup_xp_cache = 375,
        mp_pickup_xp_cache_dynamic = 376,
        mp_pickup_xp_cache_large = 377,
        mp_pickup_xp_cache_tiny = 378,
        mp_pickup_xp_cache_tiny_small = 379,
        mp_pickup_xp_cache_tiny_medium = 380,
        mp_pickup_xp_cache_tiny_large = 381,
        mp_pickup_xp_cache_tiny_xlarge = 382,
        sh_bounty_hunt = 383,
        hopup_magic_pixel = 384,
        mythic_bin_key = 385,
        mp_ability_boost_kit_magic_pixel = 386,
        mp_ability_boost_kit_pilot_hover = 387,
        mp_ability_boost_kit_threat_smoke = 388,
        mp_ability_boost_kit_phase_rewind = 389,
        mp_ability_boost_kit_lifesteal_kit = 390,
        mp_ability_boost_kit_nessie_summon = 391,
        mp_ability_boost_kit_heal_radius = 392,
        mp_ability_boost_kit_gold_armor = 393,
        mp_ability_boost_kit_map_hack = 394,
        mp_ability_second_chance_token = 395,
        mp_ability_pilot_mode = 396,
        mp_weapon_vortex_shield = 397,
        augment_infinite_ammo = 398,
        augment_battery_printer = 399,
        augment_heal_overflow = 400,
        augment_legend_expert = 401,
        augment_armor_overflow = 402,
        mp_weapon_vinson_mythicset = 403,
        hopup_mythicset_vinson = 404,
        mp_ability_beamstick_stave = 405,
        hopup_car_crate = 406,
        mp_weapon_buster_rifle = 407,
    };

    inline std::string getItemName(int id) {
        switch (id) {
            case mp_weapon_sniper: return "Kraber";
            case mp_weapon_mastiff:
            case mp_weapon_mastiff_whiteset:
            case mp_weapon_mastiff_blueset:
            case mp_weapon_mastiff_purpleset:
            case mp_weapon_mastiff_gold:
            case mp_weapon_mastiff_crate: return "Mastiff";
            case mp_weapon_lstar:
            case mp_weapon_lstar_crate:
            case mp_weapon_lstar_whiteset:
            case mp_weapon_lstar_blueset:
            case mp_weapon_lstar_purpleset:
            case mp_weapon_lstar_gold: return "L-Star";
            case mp_weapon_energy_ar:
            case mp_weapon_energy_ar_whiteset:
            case mp_weapon_energy_ar_blueset:
            case mp_weapon_energy_ar_purpleset:
            case mp_weapon_energy_ar_gold:
            case mp_weapon_energy_ar_crate: return "Havoc";
            case mp_weapon_esaw:
            case mp_weapon_esaw_crate:
            case mp_weapon_esaw_whiteset:
            case mp_weapon_esaw_blueset:
            case mp_weapon_esaw_purpleset:
            case mp_weapon_esaw_gold: return "Devotion";
            case mp_weapon_doubletake:
            case mp_weapon_doubletake_crate:
            case mp_weapon_doubletake_whiteset:
            case mp_weapon_doubletake_blueset:
            case mp_weapon_doubletake_purpleset:
            case mp_weapon_doubletake_gold: return "TripleTake";
            case mp_weapon_vinson:
            case mp_weapon_vinson_whiteset:
            case mp_weapon_vinson_blueset:
            case mp_weapon_vinson_purpleset:
            case mp_weapon_vinson_gold: return "Flatline";
            case mp_weapon_hemlok:
            case mp_weapon_hemlok_crate:
            case mp_weapon_hemlok_whiteset:
            case mp_weapon_hemlok_blueset:
            case mp_weapon_hemlok_purpleset:
            case mp_weapon_hemlok_gold:
            case mp_weapon_auto_hemlok:
            case mp_weapon_auto_hemlok_whiteset:
            case mp_weapon_auto_hemlok_blueset:
            case mp_weapon_auto_hemlok_purpleset:
            case mp_weapon_auto_hemlok_gold: return "Hemlok";
            case mp_weapon_g2:
            case mp_weapon_g2_crate:
            case mp_weapon_g2_whiteset:
            case mp_weapon_g2_blueset:
            case mp_weapon_g2_purpleset:
            case mp_weapon_g2_gold: return "G7 Scout";
            case mp_weapon_alternator_smg:
            case mp_weapon_alternator_smg_crate:
            case mp_weapon_alternator_smg_whiteset:
            case mp_weapon_alternator_smg_blueset:
            case mp_weapon_alternator_smg_purpleset:
            case mp_weapon_alternator_smg_gold: return "Alternator";
            case mp_weapon_r97:
            case mp_weapon_r97_whiteset:
            case mp_weapon_r97_blueset:
            case mp_weapon_r97_purpleset:
            case mp_weapon_r97_gold:
            case mp_weapon_r97_crate: return "R-99";
            case mp_weapon_pdw:
            case mp_weapon_pdw_crate:
            case mp_weapon_pdw_whiteset:
            case mp_weapon_pdw_blueset:
            case mp_weapon_pdw_purpleset:
            case mp_weapon_pdw_gold: return "Prowler";
            case mp_weapon_volt_smg:
            case mp_weapon_volt_smg_whiteset:
            case mp_weapon_volt_smg_blueset:
            case mp_weapon_volt_smg_purpleset:
            case mp_weapon_volt_smg_gold: return "Volt";
            case mp_weapon_dmr:
            case mp_weapon_dmr_whiteset:
            case mp_weapon_dmr_blueset:
            case mp_weapon_dmr_purpleset:
            case mp_weapon_dmr_gold: return "Longbow";
            case mp_weapon_defender:
            case mp_weapon_defender_whiteset:
            case mp_weapon_defender_blueset:
            case mp_weapon_defender_purpleset:
            case mp_weapon_defender_gold: return "Charge Rifle";
            case mp_weapon_lmg:
            case mp_weapon_lmg_whiteset:
            case mp_weapon_lmg_blueset:
            case mp_weapon_lmg_purpleset:
            case mp_weapon_lmg_gold: return "Spitfire";
            case mp_weapon_rspn101:
            case mp_weapon_rspn101_whiteset:
            case mp_weapon_rspn101_blueset:
            case mp_weapon_rspn101_purpleset:
            case mp_weapon_rspn101_gold: return "R-301";
            case mp_weapon_shotgun:
            case mp_weapon_shotgun_crate:
            case mp_weapon_shotgun_whiteset:
            case mp_weapon_shotgun_blueset:
            case mp_weapon_shotgun_purpleset:
            case mp_weapon_shotgun_gold: return "EVA-8";
            case mp_weapon_energy_shotgun:
            case mp_weapon_energy_shotgun_crate:
            case mp_weapon_energy_shotgun_whiteset:
            case mp_weapon_energy_shotgun_blueset:
            case mp_weapon_energy_shotgun_purpleset:
            case mp_weapon_energy_shotgun_gold: return "Peacekeeper";
            case mp_weapon_shotgun_pistol:
            case mp_weapon_shotgun_pistol_whiteset:
            case mp_weapon_shotgun_pistol_blueset:
            case mp_weapon_shotgun_pistol_purpleset:
            case mp_weapon_shotgun_pistol_gold:
            case mp_weapon_shotgun_pistol_akimbo_whiteset:
            case mp_weapon_shotgun_pistol_akimbo_blueset:
            case mp_weapon_shotgun_pistol_akimbo_purpleset:
            case mp_weapon_shotgun_pistol_akimbo_gold:
            case mp_weapon_shotgun_pistol_light:
            case mp_weapon_shotgun_pistol_heavy:
            case mp_weapon_shotgun_pistol_sniper:
            case mp_weapon_shotgun_pistol_energy: return "Mozambique";
            case mp_weapon_wingman:
            case mp_weapon_wingman_crate:
            case mp_weapon_wingman_whiteset:
            case mp_weapon_wingman_blueset:
            case mp_weapon_wingman_purpleset:
            case mp_weapon_wingman_gold: return "Wingman";
            case mp_weapon_semipistol:
            case mp_weapon_semipistol_whiteset:
            case mp_weapon_semipistol_blueset:
            case mp_weapon_semipistol_purpleset:
            case mp_weapon_semipistol_gold:
            case mp_weapon_semipistol_crate:
            case mp_weapon_semipistol_crate_akimbo:
            case mp_weapon_semipistol_akimbo_whiteset:
            case mp_weapon_semipistol_akimbo_blueset:
            case mp_weapon_semipistol_akimbo_purpleset:
            case mp_weapon_semipistol_akimbo_gold: return "P2020";
            case mp_weapon_autopistol:
            case mp_weapon_autopistol_crate:
            case mp_weapon_autopistol_fusion:
            case mp_weapon_autopistol_fusion_whiteset:
            case mp_weapon_autopistol_fusion_blueset:
            case mp_weapon_autopistol_fusion_purpleset:
            case mp_weapon_autopistol_fusion_gold:
            case mp_weapon_autopistol_whiteset:
            case mp_weapon_autopistol_blueset:
            case mp_weapon_autopistol_purpleset:
            case mp_weapon_autopistol_gold: return "RE-45";
            case mp_weapon_sentinel:
            case mp_weapon_sentinel_whiteset:
            case mp_weapon_sentinel_blueset:
            case mp_weapon_sentinel_purpleset:
            case mp_weapon_sentinel_gold: return "Sentinel";
            case mp_weapon_bow:
            case mp_weapon_bow_crate:
            case mp_weapon_bow_cupid:
            case mp_weapon_bow_whiteset:
            case mp_weapon_bow_blueset:
            case mp_weapon_bow_purpleset:
            case mp_weapon_bow_gold: return "Bocek";
            case mp_weapon_3030:
            case mp_weapon_3030_whiteset:
            case mp_weapon_3030_blueset:
            case mp_weapon_3030_purpleset:
            case mp_weapon_3030_gold: return "30-30";
            case mp_weapon_nemesis:
            case mp_weapon_nemesis_whiteset:
            case mp_weapon_nemesis_blueset:
            case mp_weapon_nemesis_purpleset:
            case mp_weapon_nemesis_gold: return "Nemesis";
            case bullet: return "Light Ammo";
            case special: return "Energy Ammo";
            case shotgun: return "Shotgun Ammo";
            case highcal: return "Heavy Ammo";
            case sniper: return "Sniper Ammo";
            case mp_weapon_dragon_lmg:
            case mp_weapon_dragon_lmg_crate:
            case mp_weapon_dragon_lmg_whiteset:
            case mp_weapon_dragon_lmg_blueset:
            case mp_weapon_dragon_lmg_purpleset:
            case mp_weapon_dragon_lmg_gold: return "Rampage";
            case mp_weapon_car:
            case mp_weapon_car_crate:
            case mp_weapon_car_whiteset:
            case mp_weapon_car_blueset:
            case mp_weapon_car_purpleset:
            case mp_weapon_car_gold: return "C.A.R. SMG";
            case mp_weapon_semipistol_paintball_akimbo_blueset: return "P2020";
            case mp_weapon_shotgun_pistol_paintball_akimbo_blueset: return "Mozambique";
            case mp_weapon_alternator_smg_paintball_blueset: return "Alternator";
            case mp_weapon_g2_paintball_blueset: return "G7 Scout";
            case mp_weapon_hemlok_paintball_blueset: return "Hemlok";
            case mp_weapon_r97_paintball_blueset: return "R-99";
            case mp_weapon_mastiff_paintball_blueset: return "Mastiff";
            case mp_weapon_3030_paintball_blueset: return "30-30";
            case mp_weapon_autopistol_paintball_purpleset: return "RE-45";
            case mp_weapon_3030_paintball_purpleset: return "30-30";
            case mp_weapon_wingman_paintball_purpleset: return "Wingman";
            case mp_weapon_shotgun_paintball_purpleset: return "EVA-8";
            case mp_weapon_lmg_paintball_purpleset: return "Spitfire";
            case mp_weapon_dragon_lmg_paintball_purpleset: return "Rampage";
            case mp_weapon_vinson_paintball_purpleset: return "Flatline";
            case mp_weapon_pdw_paintball_purpleset: return "Prowler";
            case mp_weapon_dmr_paintball_purpleset: return "Longbow";
            case mp_weapon_3030_paintball_gold: return "30-30";
            case mp_weapon_dragon_lmg_paintball_gold: return "Rampage";
            case mp_weapon_autopistol_paintball_gold: return "RE-45";
            case mp_weapon_semipistol_paintball_gold: return "P2020";
            case mp_weapon_shotgun_pistol_paintball_gold: return "Mozambique";
            case mp_weapon_wingman_paintball_gold: return "Wingman";
            case mp_weapon_shotgun_paintball_gold: return "EVA-8";
            case mp_weapon_mastiff_paintball_gold: return "Mastiff";
            case mp_weapon_pdw_paintball_gold: return "Prowler";
            case mp_weapon_r97_paintball_gold: return "R-99";
            case mp_weapon_vinson_paintball_gold: return "Flatline";
            case mp_weapon_hemlok_paintball_gold: return "Hemlok";
            case mp_weapon_rspn101_paintball_gold: return "R-301";
            case mp_weapon_g2_paintball_gold: return "G7 Scout";
            case mp_weapon_dmr_paintball_gold: return "Longbow";
            case health_pickup_ultimate: return "Ult Accelerant";
            case health_pickup_combo_full: return "Phoenix Kit";
            case health_pickup_health_large: return "Med Kit";
            case health_pickup_health_small: return "Syringe";
            case health_pickup_combo_large: return "Shield Battery";
            case health_pickup_combo_small: return "Shield Cell";
            case helmet_pickup_lv1: return "Helmet Lv1";
            case helmet_pickup_lv2: return "Helmet Lv2";
            case helmet_pickup_lv3: return "Helmet Lv3";
            case helmet_pickup_lv4_abilities:
            case helmet_pickup_lv4_armor_upgrade: return "Helmet Lv4";
            case armor_pickup_lv0:
            case armor_pickup_lv1: return "Armor Lv1";
            case armor_pickup_lv2: return "Armor Lv2";
            case armor_pickup_lv3: return "Armor Lv3";
            case armor_pickup_lv4_all_fast: return "Armor Lv4";
            case armor_pickup_lv0_evolving:
            case armor_pickup_lv1_evolving: return "Evo Armor Lv1";
            case armor_pickup_lv2_evolving: return "Evo Armor Lv2";
            case armor_pickup_lv3_evolving: return "Evo Armor Lv3";
            case armor_pickup_lv5_evolving: return "Evo Armor Lv5";
            case armor_core_pickup_lv1: return "Armor Core Lv1";
            case armor_core_pickup_lv2: return "Armor Core Lv2";
            case armor_core_pickup_lv3: return "Armor Core Lv3";
            case armor_core_pickup_lv5: return "Armor Core Lv5";
            case incapshield_pickup_lv1: return "Knockdown Lv1";
            case incapshield_pickup_lv2: return "Knockdown Lv2";
            case incapshield_pickup_lv3: return "Knockdown Lv3";
            case incapshield_pickup_lv4_selfrevive: return "Knockdown Lv4";
            case backpack_pickup_lv1: return "Backpack Lv1";
            case backpack_pickup_lv2: return "Backpack Lv2";
            case backpack_pickup_lv3: return "Backpack Lv3";
            case backpack_pickup_lv4_revive_boost: return "Backpack Lv4";
            case mp_weapon_throwingknife: return "Thermite";
            case mp_weapon_thermite_grenade: return "Thermite";
            case mp_weapon_frag_grenade: return "Frag Grenade";
            case mp_weapon_grenade_emp: return "Arc Star";
            case optic_cq_hcog_classic: return "1x HCOG";
            case optic_cq_hcog_bruiser: return "2x HCOG";
            case optic_cq_holosight:
            case optic_cq_holosight_variable: return "1x Holo";
            case optic_ranged_hcog: return "3x HCOG";
            case optic_ranged_aog_variable: return "2x-4x AOG";
            case optic_sniper:
            case optic_sniper_variable:
            case optic_sniper_threat: return "Sniper Scope";
            case barrel_stabilizer_l1: return "Barrel Lv1";
            case barrel_stabilizer_l2: return "Barrel Lv2";
            case barrel_stabilizer_l3: return "Barrel Lv3";
            case laser_sight_l1: return "Laser Lv1";
            case laser_sight_l2: return "Laser Lv2";
            case laser_sight_l3: return "Laser Lv3";
            case bullets_mag_l1: return "Light Mag Lv1";
            case bullets_mag_l2: return "Light Mag Lv2";
            case bullets_mag_l3: return "Light Mag Lv3";
            case bullets_mag_l4: return "Light Mag Lv4";
            case highcal_mag_l1: return "Heavy Mag Lv1";
            case highcal_mag_l2: return "Heavy Mag Lv2";
            case highcal_mag_l3: return "Heavy Mag Lv3";
            case highcal_mag_l4: return "Heavy Mag Lv4";
            case energy_mag_l1: return "Energy Mag Lv1";
            case energy_mag_l2: return "Energy Mag Lv2";
            case energy_mag_l3: return "Energy Mag Lv3";
            case energy_mag_l4: return "Energy Mag Lv4";
            case sniper_mag_l1: return "Sniper Mag Lv1";
            case sniper_mag_l2: return "Sniper Mag Lv2";
            case sniper_mag_l3: return "Sniper Mag Lv3";
            case sniper_mag_l4: return "Sniper Mag Lv4";
            case shotgun_bolt_l1: return "Bolt Lv1";
            case shotgun_bolt_l2: return "Bolt Lv2";
            case shotgun_bolt_l3: return "Bolt Lv3";
            case shotgun_bolt_l4: return "Bolt Lv4";
            case stock_tactical_l1: return "Stock Lv1";
            case stock_tactical_l2: return "Stock Lv2";
            case stock_tactical_l3: return "Stock Lv3";
            case stock_sniper_l1: return "Sniper Stock Lv1";
            case stock_sniper_l2: return "Sniper Stock Lv2";
            case stock_sniper_l3: return "Sniper Stock Lv3";
            case mp_ability_mobile_respawn_beacon: return "Mobile Respawn";
            case s4t_item0: return "Keycard";
            case mp_ability_void_ring: return "Heat Shield";
            case ship_keycard: return "Keycard";
            case mp_ability_redeploy_balloon: return "Evac Tower";
            case fr_nessie:
            case fr_nessie_yellow:
            case fr_nessie_pink:
            case fr_nessie_turquoise:
            case fr_nessie_blue:
            case fr_nessie_red:
            case fr_nessie_moy:
            case fr_nessie_large:
            case fr_nessie_lavender:
            case fr_nessie_white:
            case fr_nessie_orange:
            case fr_nessie_black:
            case fr_nessie_gray: return "Nessie";
            case mp_ability_boost_kit_nessie_summon: return "Nessie Summon";
            case augment_infinite_ammo: return "Infinite Ammo";
            case augment_battery_printer: return "Battery Printer";
            case augment_heal_overflow: return "Heal Overflow";
            case augment_legend_expert: return "Legend Expert";
            case augment_armor_overflow: return "Armor Overflow";
            case mp_weapon_vinson_mythicset: return "Flatline Mythic";
            case mp_ability_beamstick_stave: return "Stave";
            case hopup_car_crate: return "CAR Crate Hopup";
            case mp_weapon_buster_rifle: return "Buster Rifle";
            default: return "Unknown";
        }
    }
}
