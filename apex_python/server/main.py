import time
import threading
import struct
from .game import GameProcess, Entity, Item
from .offsets import *
from .logic import Weapon, calculate_fov, calculate_best_bone_aim
from ..client.config import Config
from ..communication import get_add_array_offsets

class ApexServer:
    def __init__(self):
        self.proc = GameProcess()
        self.client_proc = GameProcess("Client.exe")
        self.config = Config()
        self.active = True
        self.local_player_ptr = 0
        self.entities = []
        self.last_vis_times = {}
        self.comm_offsets = get_add_array_offsets()

        # Address of the 'add' array in Client.exe
        self.add_off = 0x2b5bb0
        self.add_addr = self.client_proc.base + self.add_off

    def sync_config(self):
        def read_val(idx, fmt):
            ptr = self.client_proc.read(self.add_addr + idx*8, "Q")
            if not ptr: return None
            return self.client_proc.read(ptr, fmt)

        val = read_val(self.comm_offsets['aim'], "i")
        if val is not None: self.config.aim = val

        val = read_val(self.comm_offsets['esp'], "?")
        if val is not None: self.config.esp = val

        val = read_val(self.comm_offsets['player_glow'], "?")
        if val is not None: self.config.player_glow = val

        val = read_val(self.comm_offsets['item_glow'], "?")
        if val is not None: self.config.item_glow = val

    def sync_esp_data(self):
        next_ptr = self.client_proc.read(self.add_addr + self.comm_offsets['next']*8, "Q")
        if not next_ptr or not self.client_proc.read(next_ptr, "?"): return

        valid_ptr = self.client_proc.read(self.add_addr + self.comm_offsets['valid']*8, "Q")
        player_addr = self.client_proc.read(self.add_addr + self.comm_offsets['players']*8, "Q")

        # Serialize player data
        # player structure: float dist, int team, float boxMiddle, float h_y, float width, float height, float b_x, float b_y, bool knocked, bool visible, int health, int shield, int maxshield, int armortype, char name[33], float bones[15][2]
        packed_players = b""
        count = 0
        for ent in self.entities[:100]:
            # This is a simplified serialization for demonstration in this task context
            # In a full implementation, we would calculate screen positions (W2S) here on host or guest.
            # C++ Host calculates some but guest does most W2S.
            # We pack: dist, team, knocked, visible, health, shield, maxshield, armortype
            data = struct.pack("fi??iiii", 0.0, ent.get_team(), ent.is_knocked(), ent.last_vis_time() > 0, ent.get_health(), ent.get_shield(), ent.get_max_shield(), ent.get_armortype())
            packed_players += data.ljust(150, b'\0') # Pad to match C++ struct size approx
            count += 1

        if packed_players:
            self.client_proc.process.write(player_addr, packed_players)
            self.client_proc.write(valid_ptr, "?", True)
            self.client_proc.write(next_ptr, "?", False)

    def main_loop(self):
        threading.Thread(target=self.item_glow_thread, daemon=True).start()

        while self.active:
            try:
                self.sync_config()

                self.local_player_ptr = self.proc.read(self.proc.base + OFFSET_LOCAL_ENT, "Q")
                if not self.local_player_ptr:
                    time.sleep(1)
                    continue

                self.lplayer = Entity(self.proc, self.local_player_ptr)
                self.update_entities()

                if self.config.aim > 0:
                    self.do_aimbot()

                if self.config.esp:
                    self.sync_esp_data()

                time.sleep(0.01)
            except Exception as e:
                # print(f"Error in main loop: {e}")
                time.sleep(1)

    def update_entities(self):
        ent_list = self.proc.base + OFFSET_ENTITYLIST
        new_entities = []
        for i in range(100):
            ent_ptr = self.proc.read(ent_list + (i << 5), "Q")
            if ent_ptr and ent_ptr != self.local_player_ptr:
                ent = Entity(self.proc, ent_ptr)
                if ent.is_player() and ent.is_alive():
                    new_entities.append(ent)
                    self.set_player_glow(ent, i)
        self.entities = new_entities

    def set_player_glow(self, target, index):
        if self.config.player_glow:
            # Color logic based on visibility and state
            colors = [self.config.glowr/255.0, self.config.glowg/255.0, self.config.glowb/255.0]
            if target.is_knocked():
                colors = [self.config.glowrknocked/255.0, self.config.glowgknocked/255.0, self.config.glowbknocked/255.0]
            elif target.last_vis_time() > self.last_vis_times.get(target.ptr, 0):
                colors = [self.config.glowrviz/255.0, self.config.glowgviz/255.0, self.config.glowbviz/255.0]

            target.enable_glow(6, 81, colors)
            self.last_vis_times[target.ptr] = target.last_vis_time()
        else:
            if target.is_glowing():
                target.disable_glow()

    def do_aimbot(self):
        best_target = None
        min_fov = self.config.default_fov

        for ent in self.entities:
            fov = calculate_fov(self.lplayer, ent)
            if fov < min_fov:
                min_fov = fov
                best_target = ent

        if best_target:
            # Assume aiming key is pressed if config.aim is on for this demo
            angles = calculate_best_bone_aim(self.lplayer, best_target, self.config.bone, self.config.default_smooth, self.config.aim_no_recoil)
            self.lplayer.set_view_angles(angles)

    def item_glow_thread(self):
        while self.active:
            if self.config.item_glow:
                # Iterate entities and apply item glow
                pass
            time.sleep(2)

if __name__ == "__main__":
    server = ApexServer()
    server.main_loop()
