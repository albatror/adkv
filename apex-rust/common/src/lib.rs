use serde::{Deserialize, Serialize};

#[repr(C)]
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct Player {
    pub dist: f32,
    pub entity_team: i32,
    pub box_middle: f32,
    pub h_y: f32,
    pub width: f32,
    pub height: f32,
    pub b_x: f32,
    pub b_y: f32,
    pub knocked: bool,
    pub visible: bool,
    pub health: i32,
    pub shield: i32,
    pub max_shield: i32,
    pub armor_type: i32,
    pub xp_level: i32,
    pub platform: i32,
    pub name: [u8; 32],
    pub weapon: [u8; 32],
    pub bones: [[f32; 2]; 17],
}

impl Default for Player {
    fn default() -> Self {
        unsafe { std::mem::zeroed() }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct Spectator {
    pub is_spec: bool,
    pub name: [u8; 32],
}

impl Default for Spectator {
    fn default() -> Self {
        unsafe { std::mem::zeroed() }
    }
}

#[repr(C)]
#[derive(Copy, Clone, Debug, Serialize, Deserialize)]
pub struct Matrix {
    pub matrix: [f32; 16],
}

pub const OFFSET_ENTITYLIST: u64 = 0x612c5e8;
pub const OFFSET_LOCAL_ENT: u64 = 0x2563888;
pub const OFFSET_LOCAL_ENTITY_HANDLE: u64 = 0x23f12f0;
pub const OFFSET_NAME_LIST: u64 = 0x8b26c80;
pub const OFFSET_GLOBAL_VARS: u64 = 0x1d0b8e0;
pub const OFFSET_LEVELNAME: u64 = 0x1d0bd64;
pub const OFFSET_HOST_MAP: u64 = 0x1b66090 + 0x58;
pub const OFFSET_TEAM: u64 = 0x334;
pub const OFFSET_HEALTH: u64 = 0x324;
pub const OFFSET_MAXHEALTH: u64 = 0x468;
pub const OFFSET_SHIELD: u64 = 0x1a0;
pub const OFFSET_MAXSHIELD: u64 = 0x1a4;
pub const OFFSET_ARMORTYPE: u64 = 0x16e4;
pub const OFFSET_NAME: u64 = 0x479;
pub const OFFSET_SIGN_NAME: u64 = 0x470;
pub const OFFSET_ABS_VELOCITY: u64 = 0x170;
pub const OFFSET_VISIBLE_TIME: u64 = 0x1A64;
pub const OFFSET_LAST_AIMEDAT_TIME: u64 = 0x1a6c;
pub const OFFSET_ZOOMING: u64 = 0x1cb1;
pub const OFFSET_VIEW_OFFSET: u64 = 0xe8;
pub const OFFSET_ACTIVE_WEAPON: u64 = 0x1958 + 0x0058;
pub const OFFSET_OBSERVER_LIST: u64 = 0x612e608;
pub const OFFSET_OBSERVER_ARRAY: u64 = 0x964;
pub const OFFSET_IN_DUCKSTATE: u64 = 0x2abc;
pub const OFFSET_IN_DUCK: u64 = 0x3c75538;
pub const OFFSET_TRAVERSAL_PROGRESS: u64 = 0x2bcc;
pub const OFFSET_TRAVERSAL_STARTTIME: u64 = 0x2bd4;
pub const OFFSET_TRAVERSAL_RELEASE_TIME: u64 = 0x2bdc;
pub const OFFSET_IN_JUMP: u64 = 0x3c75440;
pub const OFFSET_IN_TOGGLE_DUCK: u64 = 0x3c75368;
pub const OFFSET_WEAPON_NAME: u64 = 0x19c4;
pub const OFFSET_OFF_WEAPON: u64 = 0x19d4;
pub const OFFSET_WEAPON: u64 = 0x15f0;
pub const OFFSET_WALL_RUN_START_TIME: u64 = 0x370c;
pub const OFFSET_WALL_RUN_CLEAR_TIME: u64 = 0x3710;
pub const OFFSET_FLAGS: u64 = 0xc8;
pub const OFFSET_IN_ATTACK: u64 = 0x3c74b88;
pub const OFFSET_IN_ZOOM: u64 = 0x3c754c0;
pub const OFFSET_IN_FORWARD: u64 = 0x3c75578;
pub const OFFSET_IN_BACKWARD: u64 = 0x3c74b00;
pub const OFFSET_LIFE_STATE: u64 = 0x690;
pub const OFFSET_BLEED_OUT_STATE: u64 = 0x27c0;
pub const OFFSET_ORIGIN: u64 = 0x17c;
pub const OFFSET_BONES: u64 = 0xdb8 + 0x48;
pub const OFFSET_STUDIOHDR: u64 = 0x1000;
pub const OFFSET_AIMPUNCH: u64 = 0x2518;
pub const OFFSET_CAMERAPOS: u64 = 0x1fc4;
pub const OFFSET_VIEWANGLES: u64 = 0x2614 - 0x104;
pub const OFFSET_BREATH_ANGLES: u64 = OFFSET_VIEWANGLES - 0x10;
pub const OFFSET_OBSERVER_MODE: u64 = 0x35f4;
pub const OFFSET_OBSERVING_TARGET: u64 = 0x3600;
pub const OFFSET_IN_USE: u64 = 0x3c754b0;
pub const OFFSET_MATRIX: u64 = 0x11a350;
pub const OFFSET_RENDER: u64 = 0x3c73920;
pub const OFFSET_BULLET_SPEED: u64 = 0x28A8;
pub const OFFSET_BULLET_SCALE: u64 = 0x28A8 + 0x8;
pub const OFFSET_ZOOM_FOV: u64 = 0x1724;
pub const OFFSET_AMMO: u64 = 0x15e0;
pub const OFFSET_ITEM_ID: u64 = 0x15e4;
pub const OFFSET_MODELNAME: u64 = 0x30;
pub const OFFSET_M_CUSTOMSCRIPTINT: u64 = OFFSET_ITEM_ID;
pub const OFFSET_YAW: u64 = 0x231c - 0x8;
pub const OFFSET_GLOW_ENABLE: u64 = 0x299 - 0x1;
pub const OFFSET_GLOW_THROUGH_WALLS: u64 = 0x26c;
pub const OFFSET_TIME_BASE: u64 = 0x2168;
pub const HIGHLIGHT_SETTINGS: u64 = 0x68762f0;
pub const HIGHLIGHT_TYPE_SIZE: u64 = 0x34;
pub const OFFSET_CROSSHAIR_LAST: u64 = 0x1a6c;
pub const OFFSET_INPUT_SYSTEM: u64 = 0x1dc5e00;
pub const OFFSET_SKYDIVE_STATE: u64 = 0x48b8;
pub const OFFSET_GRAPPLEACTIVED: u64 = 0x2da8;
pub const OFFSET_GRAPPLE: u64 = 0x2d20;
pub const OFFSET_GRAPPLEATTACHED: u64 = 0x48;
pub const OFFSET_m_xp: u64 = 0x3824;
pub const OFFSET_GRADE: u64 = 0x344;
pub const OFFSET_PLATFORM: u64 = 0x2620;
