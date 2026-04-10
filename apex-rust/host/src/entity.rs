use crate::memory::Memory;
use common::*;

pub struct Entity {
    pub ptr: u64,
    pub buffer: [u8; 0x3FF0],
}

impl Entity {
    pub fn new(ptr: u64, mem: &mut Memory) -> Self {
        let mut buffer = [0u8; 0x3FF0];
        if let Ok(data) = mem.read_array::<u8>(ptr, 0x3FF0) {
            buffer.copy_from_slice(&data);
        }
        Self { ptr, buffer }
    }

    pub fn get_team_id(&self) -> i32 {
        i32::from_le_bytes(self.buffer[OFFSET_TEAM as usize..OFFSET_TEAM as usize + 4].try_into().unwrap())
    }

    pub fn get_health(&self) -> i32 {
        i32::from_le_bytes(self.buffer[OFFSET_HEALTH as usize..OFFSET_HEALTH as usize + 4].try_into().unwrap())
    }

    pub fn is_alive(&self) -> bool {
        i32::from_le_bytes(self.buffer[OFFSET_LIFE_STATE as usize..OFFSET_LIFE_STATE as usize + 4].try_into().unwrap()) == 0
    }

    pub fn get_origin(&self) -> [f32; 3] {
        let x = f32::from_le_bytes(self.buffer[OFFSET_ORIGIN as usize..OFFSET_ORIGIN as usize + 4].try_into().unwrap());
        let y = f32::from_le_bytes(self.buffer[OFFSET_ORIGIN as usize + 4..OFFSET_ORIGIN as usize + 8].try_into().unwrap());
        let z = f32::from_le_bytes(self.buffer[OFFSET_ORIGIN as usize + 8..OFFSET_ORIGIN as usize + 12].try_into().unwrap());
        [x, y, z]
    }

    pub fn is_player(&self) -> bool {
        let name_val = u64::from_le_bytes(self.buffer[OFFSET_NAME as usize..OFFSET_NAME as usize + 8].try_into().unwrap());
        name_val == 125780153691248
    }
}
