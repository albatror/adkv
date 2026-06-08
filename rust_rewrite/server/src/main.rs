mod memory;
mod offsets;
mod entity;
mod math;

use crate::memory::Memory;
use crate::entity::Entity;
use crate::offsets::*;
use std::thread;
use std::time::Duration;

#[repr(C)]
#[derive(Copy, Clone)]
struct PlayerShared {
    dist: f32,
    entity_team: i32,
    box_middle: f32,
    h_y: f32,
    width: f32,
    height: f32,
    b_x: f32,
    b_y: f32,
    knocked: bool,
    visible: bool,
    health: i32,
    shield: i32,
    maxshield: i32,
    armortype: i32,
    xp_level: i32,
    platform: i32,
    name: [u8; 33],
    weapon: [u8; 33],
    bones: [[f32; 2]; 17],
}

impl Default for PlayerShared {
    fn default() -> Self {
        unsafe { std::mem::zeroed() }
    }
}

unsafe impl memflow::prelude::v1::Pod for PlayerShared {}

fn main() -> anyhow::Result<()> {
    env_logger::init();
    println!("Starting Rust Apex DMA Server...");

    let mut apex_mem = loop {
        match Memory::new("kvm", "r5apex_dx12.exe") {
            Ok(m) => break m,
            Err(_) => {
                println!("Searching for apex process...");
                thread::sleep(Duration::from_secs(1));
            }
        }
    };

    println!("Apex process found! Base: {:x}", apex_mem.base_addr.to_umem());

    let mut client_mem = loop {
        match Memory::new("kvm", "Client.exe") {
            Ok(m) => break m,
            Err(_) => {
                println!("Searching for client process...");
                thread::sleep(Duration::from_secs(1));
            }
        }
    };

    println!("Client process found! Base: {:x}", client_mem.base_addr.to_umem());

    // CRITICAL: Update this with the 'add offset' printed by the client.exe
    let add_array_offset: u64 = 0x0;

    let add_array_ptr = client_mem.base_addr.to_umem() + add_array_offset;
    let players_array_ptr = if add_array_offset != 0 { add_array_ptr + (6 * 8) } else { 0 }; // index 6 in the 'add' array

    loop {
        let local_player_ptr = match apex_mem.read::<u64>(apex_mem.base_addr.to_umem() + OFFSET_LOCAL_ENT) {
            Ok(ptr) => ptr,
            Err(_) => {
                thread::sleep(Duration::from_millis(100));
                continue;
            }
        };

        if local_player_ptr == 0 {
            thread::sleep(Duration::from_millis(100));
            continue;
        }

        let local_player = Entity::new(local_player_ptr);
        let _team = local_player.get_team_id(&mut apex_mem);
        let local_pos = local_player.get_origin(&mut apex_mem);

        let mut players_shared = [PlayerShared::default(); 100];

        let entity_list = apex_mem.base_addr.to_umem() + OFFSET_ENTITYLIST;
        for i in 0..100 {
            let entity_ptr = match apex_mem.read::<u64>(entity_list + (i as u64 * 32)) {
                Ok(ptr) if ptr != 0 && ptr != local_player_ptr => ptr,
                _ => continue,
            };

            let target = Entity::new(entity_ptr);
            if !target.is_alive(&mut apex_mem) {
                continue;
            }

            let pos = target.get_origin(&mut apex_mem);
            let dist = local_pos.distance(pos);

            if dist > 10000.0 { // Configurable max dist
                continue;
            }

            let mut p = PlayerShared::default();
            p.dist = dist;
            p.entity_team = target.get_team_id(&mut apex_mem);
            p.health = target.get_health(&mut apex_mem);
            p.shield = target.get_shield(&mut apex_mem);
            p.maxshield = target.get_max_shield(&mut apex_mem);
            p.armortype = target.get_armor_type(&mut apex_mem);
            p.knocked = target.is_knocked(&mut apex_mem);
            p.visible = target.last_vis_time(&mut apex_mem) > 0.0; // Simple check for now

            // Bone processing and W2S would happen here if we had the matrix

            players_shared[i as usize] = p;
        }

        // Write the processed player data back to the client's shared memory
        if players_array_ptr != 0 {
            if let Ok(actual_players_ptr) = client_mem.read::<u64>(players_array_ptr) {
                if actual_players_ptr != 0 {
                    for (i, p) in players_shared.iter().enumerate() {
                        let addr = actual_players_ptr + (i as u64 * std::mem::size_of::<PlayerShared>() as u64);
                        let _ = client_mem.write(addr, *p);
                    }
                }
            }
        }

        thread::sleep(Duration::from_millis(5));
    }
}
