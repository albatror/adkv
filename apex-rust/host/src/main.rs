mod memory;
mod entity;
mod math;

use crate::memory::Memory;
use crate::entity::Entity;
use crate::math::Vector3;
use common::*;
use std::thread;
use std::time::Duration;

struct Config {
    max_dist: f32,
}

fn main() -> anyhow::Result<()> {
    env_logger::init();
    let mut apex_mem = Memory::new();
    let mut client_mem = Memory::new();

    let config = Config {
        max_dist: 120.0 * 40.0,
    };

    loop {
        if apex_mem.os.is_none() {
            println!("Searching for apex process...");
            if let Err(_) = apex_mem.open_process("r5apex_dx12.ex") {
                thread::sleep(Duration::from_secs(1));
                continue;
            }
            println!("Apex process found at 0x{:x}", apex_mem.base_addr);
        }

        if client_mem.os.is_none() {
            println!("Searching for client process...");
            if let Err(_) = client_mem.open_process("Client.exe") {
                thread::sleep(Duration::from_secs(1));
                continue;
            }
            println!("Client process found at 0x{:x}", client_mem.base_addr);
        }

        process_entities(&mut apex_mem, &mut client_mem, &config);

        thread::sleep(Duration::from_millis(10));
    }
}

fn process_entities(apex_mem: &mut Memory, _client_mem: &mut Memory, config: &Config) {
    let local_player_ptr: u64 = match apex_mem.read(apex_mem.base_addr + OFFSET_LOCAL_ENT) {
        Ok(ptr) => ptr,
        Err(_) => return,
    };

    if local_player_ptr == 0 {
        return;
    }

    let local_player = Entity::new(local_player_ptr, apex_mem);
    let local_origin = local_player.get_origin();
    let local_pos = Vector3::new(local_origin[0], local_origin[1], local_origin[2]);

    let mut players = vec![Player::default(); 100];
    let mut player_count = 0;

    for i in 0..100 {
        let entity_ptr: u64 = match apex_mem.read(apex_mem.base_addr + OFFSET_ENTITYLIST + (i << 5)) {
            Ok(ptr) => ptr,
            Err(_) => continue,
        };

        if entity_ptr == 0 || entity_ptr == local_player_ptr {
            continue;
        }

        let entity = Entity::new(entity_ptr, apex_mem);
        if entity.is_player() && entity.is_alive() {
            let entity_origin = entity.get_origin();
            let entity_pos = Vector3::new(entity_origin[0], entity_origin[1], entity_origin[2]);
            let dist = local_pos.dist_to(&entity_pos);

            if dist < config.max_dist {
                let mut p = Player::default();
                p.dist = dist;
                p.entity_team = entity.get_team_id();
                p.health = entity.get_health();

                if player_count < 100 {
                    players[player_count] = p;
                    player_count += 1;
                }
            }
        }
    }
}
