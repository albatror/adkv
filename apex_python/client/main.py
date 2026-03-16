import glfw
import imgui
from imgui.integrations.glfw import GlfwRenderer
from .config import Config
import OpenGL.GL as gl
import time
import threading
import struct
import math
import ctypes

class Overlay:
    def __init__(self):
        self.config = Config()
        self.config.load("Settings.txt")
        self.window = None
        self.impl = None
        self.show_menu = False
        self.spectators = 0
        self.allied_spectators = 0
        self.players = []
        self.running = True

        # Communication structure
        self.check = 0xABCD
        self.ready_flag = True
        self.valid_flag = False
        self.player_buffer = ctypes.create_string_buffer(100 * 150) # Matching server side

        # This simulates the 'add' array addresses for the Host to find
        self.add_array = (ctypes.c_uint64 * 64)()
        self.setup_comm_buffer()

    def setup_comm_buffer(self):
        # We store the memory addresses of our variables in the add_array
        # Host will read these addresses to sync data
        offsets = {
            'check': 0,
            'aim': 1,
            'esp': 2,
            'aiming': 3,
            'g_base': 4,
            'next': 5, # ready_flag
            'players': 6, # player_buffer
            'valid': 7, # valid_flag
            'max_dist': 8,
            'item_glow': 9,
            'player_glow': 48,
        }

        self.add_array[offsets['check']] = ctypes.addressof(ctypes.c_uint32(self.check))
        self.add_array[offsets['next']] = ctypes.addressof(ctypes.c_bool(self.ready_flag))
        self.add_array[offsets['players']] = ctypes.addressof(self.player_buffer)
        self.add_array[offsets['valid']] = ctypes.addressof(ctypes.c_bool(self.valid_flag))
        # ... map other config variables

    def update_players(self):
        if self.valid_flag:
            new_players = []
            for i in range(100):
                offset = i * 150
                data = self.player_buffer[offset:offset+150]
                if data[0:4] == b'\0\0\0\0': continue # Dist is 0 or empty

                # Unpack: float dist, int team, bool knocked, bool visible, int health, int shield, int maxshield, int armortype
                p_data = struct.unpack("fi??iiii", data[:24])
                new_players.append({
                    'pos': (500, 500), # Placeholder for demo, real would use W2S
                    'width': 50, 'height': 100,
                    'dist': p_data[0],
                    'team': p_data[1],
                    'knocked': p_data[2],
                    'visible': p_data[3],
                    'health': p_data[4],
                    'shield': p_data[5],
                    'max_shield': p_data[6],
                    'armor': p_data[7],
                    'name': f"Player {i}"
                })
            self.players = new_players
            self.ready_flag = True # Signal Host we are ready for next frame

    def init_window(self):
        if not glfw.init():
            return False

        glfw.window_hint(glfw.FLOATING, True)
        glfw.window_hint(glfw.RESIZABLE, False)
        glfw.window_hint(glfw.DECORATED, False)
        glfw.window_hint(glfw.TRANSPARENT_FRAMEBUFFER, True)

        monitor = glfw.get_primary_monitor()
        mode = glfw.get_video_mode(monitor)
        self.width = mode.width
        self.height = mode.height
        self.window = glfw.create_window(self.width, self.height, "Overlay", None, None)

        if not self.window:
            glfw.terminate()
            return False

        glfw.make_context_current(self.window)
        imgui.create_context()
        self.impl = GlfwRenderer(self.window)

        return True

    def toggle_input(self, state):
        glfw.set_window_attrib(self.window, glfw.MOUSE_PASSTHROUGH, not state)

    def render(self):
        last_ins_state = False
        while not glfw.window_should_close(self.window) and self.running:
            glfw.poll_events()
            self.impl.process_inputs()

            ins_state = glfw.get_key(self.window, glfw.KEY_INSERT) == glfw.PRESS
            if ins_state and not last_ins_state:
                self.show_menu = not self.show_menu
                self.toggle_input(self.show_menu)
            last_ins_state = ins_state

            self.update_players()

            imgui.new_frame()

            if self.show_menu:
                self.draw_menu()
            else:
                self.draw_info()

            self.draw_spectators()
            self.draw_esp()

            gl.glClearColor(0, 0, 0, 0)
            gl.glClear(gl.GL_COLOR_BUFFER_BIT)

            imgui.render()
            self.impl.render(imgui.get_draw_data())
            glfw.swap_buffers(self.window)
            time.sleep(0.001)

    def draw_menu(self):
        imgui.set_next_window_size(490, 500)
        imgui.set_next_window_pos(100, 100, imgui.ONCE)
        imgui.begin("Menu", True, imgui.WINDOW_NO_TITLE_BAR | imgui.WINDOW_NO_RESIZE)
        # ... menu items
        imgui.end()

    def draw_info(self):
        # ... info window
        pass

    def draw_spectators(self):
        # ... specs
        pass

    def draw_esp(self):
        if not self.config.esp: return
        draw_list = imgui.get_background_draw_list()

        for player in self.players:
            x, y = player['pos']
            w, h = player['width'], player['height']

            if self.config.box:
                color = imgui.get_color_u32_rgba(0, 1, 0, 1) if player['visible'] else imgui.get_color_u32_rgba(1, 0, 0, 1)
                draw_list.add_rect(x - w/2, y, x + w/2, y + h, color)

            if self.config.healthbar:
                health_h = h * (player['health'] / 100.0)
                draw_list.add_rect_filled(x - w/2 - 5, y + h - health_h, x - w/2 - 2, y + h, imgui.get_color_u32_rgba(0, 1, 0, 1))

if __name__ == "__main__":
    overlay = Overlay()
    if overlay.init_window():
        overlay.render()
