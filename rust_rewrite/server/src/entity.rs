use crate::memory::Memory;
use crate::offsets::*;
use glam::{Vec3, Vec2};

pub struct Entity {
    pub ptr: u64,
}

impl Entity {
    pub fn new(ptr: u64) -> Self {
        Self { ptr }
    }

    pub fn get_team_id(&self, mem: &mut Memory) -> i32 {
        mem.read::<i32>(self.ptr + OFFSET_TEAM).unwrap_or(0)
    }

    pub fn get_health(&self, mem: &mut Memory) -> i32 {
        mem.read::<i32>(self.ptr + OFFSET_HEALTH).unwrap_or(0)
    }

    pub fn get_shield(&self, mem: &mut Memory) -> i32 {
        mem.read::<i32>(self.ptr + OFFSET_SHIELD).unwrap_or(0)
    }

    pub fn get_max_shield(&self, mem: &mut Memory) -> i32 {
        mem.read::<i32>(self.ptr + OFFSET_MAXSHIELD).unwrap_or(0)
    }

    pub fn get_armor_type(&self, mem: &mut Memory) -> i32 {
        mem.read::<i32>(self.ptr + OFFSET_ARMORTYPE).unwrap_or(0)
    }

    pub fn get_origin(&self, mem: &mut Memory) -> Vec3 {
        let pos = mem.read::<[f32; 3]>(self.ptr + OFFSET_ORIGIN).unwrap_or([0.0, 0.0, 0.0]);
        Vec3::new(pos[0], pos[1], pos[2])
    }

    pub fn is_alive(&self, mem: &mut Memory) -> bool {
        let health = self.get_health(mem);
        let life_state = mem.read::<i32>(self.ptr + OFFSET_LIFE_STATE).unwrap_or(1);
        health > 0 && life_state == 0
    }

    pub fn is_knocked(&self, mem: &mut Memory) -> bool {
        mem.read::<i32>(self.ptr + OFFSET_BLEED_OUT_STATE).unwrap_or(0) != 0
    }

    pub fn last_vis_time(&self, mem: &mut Memory) -> f32 {
        mem.read::<f32>(self.ptr + OFFSET_VISIBLE_TIME).unwrap_or(0.0)
    }

    pub fn get_bone_pos(&self, mem: &mut Memory, id: i32) -> Vec3 {
        let bones_ptr = mem.read::<u64>(self.ptr + OFFSET_BONES).unwrap_or(0);
        if bones_ptr == 0 {
            return Vec3::ZERO;
        }
        let bone_addr = bones_ptr + (id as u64 * 0x30);
        let x = mem.read::<f32>(bone_addr + 0x0C).unwrap_or(0.0);
        let y = mem.read::<f32>(bone_addr + 0x1C).unwrap_or(0.0);
        let z = mem.read::<f32>(bone_addr + 0x2C).unwrap_or(0.0);
        Vec3::new(x, y, z) + self.get_origin(mem)
    }
}
