import threading
import time
import ctypes

# Assuming the package structure allows these imports
from py_apex_dma.memory import Memory, ProcessStatus
from py_apex_dma import game
from py_apex_dma.offsets import GAME_OFFSETS
from py_apex_dma import structs
from .server_ipc import IPCServer # Import IPCServer
from . import actions # Import actions module


class GameContext:
    def __init__(self):
        # Process-related
        self.g_base: int = 0
        self.active: bool = True

        # Feature toggles
        self.aim_enabled: bool = True
        self.esp_enabled: bool = True
        self.player_glow_enabled: bool = True
        self.item_glow_enabled: bool = False
        self.trigger_bot_enabled: bool = False
        self.aim_visibility_check: bool = True
        self.aim_no_recoil: bool = True
        self.wall_jump_enabled: bool = False
        self.super_glide_enabled: bool = False
        self.grapple_boost_enabled: bool = False # New

        # Aimbot specific
        self.aim_entity_ptr: int = 0
        self.max_dist: float = 200.0 * 40.0
        self.max_fov: float = 15.0
        self.smooth_factor: float = 0.2
        self.bone_target: int = 0
        self.aim_key_pressed: bool = False
        self.is_jump_key_pressed: bool = False

        # DDS Settings
        self.dds_enabled: bool = False
        self.dds_distance: float = 100.0 * 40.0
        self.dds_min_fov: float = 5.0
        self.dds_max_fov: float = 20.0
        self.dds_min_smooth: float = 0.1
        self.dds_max_smooth: float = 0.3

        self.firing_range: bool = False
        self.onevone_mode: bool = False
        self.team_player_id: int = 0

        self.spectators_count: int = 0
        self.allied_spectators_count: int = 0

        self.last_vis_aim_times: dict[int, float] = {}
        self.esp_players_data: list[dict] = []
        self.esp_valid_data: bool = False
        self.esp_next_update: bool = True
        self.status_message: str = "Server Initializing"

        self.glow_visible_ui: list[float] = [1.0, 0.0, 0.0, 1.0]
        self.glow_not_visible_ui: list[float] = [0.0, 1.0, 0.0, 1.0]
        self.glow_knocked_ui: list[float] = [0.0, 0.0, 1.0, 1.0]

        self.visuals: dict = {
            "box_esp": True, "name_esp": True, "circle_fov_enabled": True,
            "line_esp": True, "distance_esp": True, "health_bar_esp": True,
            "shield_bar_esp": True, "circle_fov_size": self.max_fov
        }

        self.main_sleep: float = 0.001
        self.map_name: str = ""
        self.is_in_game: bool = False

        self.aimbot_thread_handle = None
        self.esp_thread_handle = None
        self.do_actions_thread_handle = None
        self.item_glow_thread_handle = None
        self.ipc_server_handle = None

        self.MAX_ENTITY_COUNT = 100
        self.SCREEN_WIDTH = 1920
        self.SCREEN_HEIGHT = 1080

        self.aim_target_entity_ptr: int = 0

        self.action_states: dict = {
            'wall_jump': {},
            'super_glide': {},
            'grapple_boost': {} # New
        }

    def update_settings(self, settings_data: dict):
        self.aim_enabled = settings_data.get("aim_enabled", self.aim_enabled)
        self.esp_enabled = settings_data.get("esp_enabled", self.esp_enabled)
        self.player_glow_enabled = settings_data.get("player_glow_enabled", self.player_glow_enabled)
        self.item_glow_enabled = settings_data.get("item_glow_enabled", self.item_glow_enabled)
        self.aim_visibility_check = settings_data.get("aim_visibility_check", self.aim_visibility_check)
        self.aim_no_recoil = settings_data.get("aim_no_recoil", self.aim_no_recoil)
        self.aim_key_pressed = settings_data.get("aim_key_pressed", self.aim_key_pressed)
        self.is_jump_key_pressed = settings_data.get("is_jump_key_pressed", self.is_jump_key_pressed)

        self.max_dist = settings_data.get("max_dist", self.max_dist)
        self.max_fov = settings_data.get("max_fov", self.max_fov)
        self.smooth_factor = settings_data.get("smooth_aim_value", self.smooth_factor)
        self.bone_target = settings_data.get("aim_bone", self.bone_target)

        self.dds_enabled = settings_data.get("dds_enabled", self.dds_enabled)
        self.dds_distance = settings_data.get("dds_distance", self.dds_distance)
        self.dds_min_fov = settings_data.get("dds_min_fov", self.dds_min_fov)
        self.dds_max_fov = settings_data.get("dds_max_fov", self.dds_max_fov)
        self.dds_min_smooth = settings_data.get("dds_min_smooth", self.dds_min_smooth)
        self.dds_max_smooth = settings_data.get("dds_max_smooth", self.dds_max_smooth)

        self.firing_range = settings_data.get("firing_range", self.firing_range)
        self.onevone_mode = settings_data.get("onevone_mode", self.onevone_mode)

        self.wall_jump_enabled = settings_data.get("wall_jump_enabled", self.wall_jump_enabled)
        self.super_glide_enabled = settings_data.get("super_glide_enabled", self.super_glide_enabled)
        self.grapple_boost_enabled = settings_data.get("grapple_boost_enabled", self.grapple_boost_enabled) # New

        self.glow_visible_ui = settings_data.get("glow_visible_ui", self.glow_visible_ui)
        self.glow_not_visible_ui = settings_data.get("glow_not_visible_ui", self.glow_not_visible_ui)
        self.glow_knocked_ui = settings_data.get("glow_knocked_ui", self.glow_knocked_ui)

        client_visuals = settings_data.get("visuals", {})
        if isinstance(client_visuals, dict):
            self.visuals.update(client_visuals)

    def get_serializable_state(self) -> dict:
        esp_players_list_of_dicts = []
        if self.esp_valid_data:
            esp_players_list_of_dicts = self.esp_players_data
        return {
            "status": self.status_message,
            "map_name": self.map_name,
            "spectators": {
                "enemy": self.spectators_count,
                "allied": self.allied_spectators_count
            },
            "esp_players": esp_players_list_of_dicts,
            "firing_range": self.firing_range,
        }

    def to_dict_for_processing(self) -> dict:
        return {
            "firing_range": self.firing_range,
            "onevone_mode": self.onevone_mode,
            "team_player_id": self.team_player_id,
            "max_dist": self.max_dist,
            "player_glow_enabled": self.player_glow_enabled,
            "last_vis_aim_times": self.last_vis_aim_times,
            "glowr": self.glow_not_visible_ui[0],
            "glowg": self.glow_not_visible_ui[1],
            "glowb": self.glow_not_visible_ui[2],
            "glowrviz": self.glow_visible_ui[0],
            "glowgviz": self.glow_visible_ui[1],
            "glowbviz": self.glow_visible_ui[2],
            "glowrknc": self.glow_knocked_ui[0],
            "glowgknc": self.glow_knocked_ui[1],
            "glowbknc": self.glow_knocked_ui[2],
            "aim_enabled": self.aim_enabled,
        }

def aimbot_loop(ctx: GameContext, apex_mem: Memory):
    while ctx.active:
        if not (ctx.g_base and ctx.aim_enabled and ctx.aim_key_pressed):
            ctx.aim_target_entity_ptr = 0
            time.sleep(0.01)
            continue
        try:
            local_player_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_LOCAL_ENT"], local_player_ptr_val) or local_player_ptr_val.value == 0:
                ctx.aim_target_entity_ptr = 0; time.sleep(0.1); continue
            local_player = game.get_entity(local_player_ptr_val.value, apex_mem, ctx.g_base)
            if not local_player or not local_player.get_ptr() or not local_player.is_alive():
                ctx.aim_target_entity_ptr = 0; time.sleep(0.1); continue
            current_view_angles = local_player.get_view_angles()
            if not current_view_angles:
                 ctx.aim_target_entity_ptr = 0; time.sleep(0.1); continue
            best_target_ptr = 0
            min_fov_from_crosshair = ctx.max_fov
            esp_data_copy = list(ctx.esp_players_data)
            for player_dict in esp_data_copy:
                if not player_dict.get("visible", False) or player_dict.get("knocked", True):
                    continue
                target_entity_temp_ptr = player_dict.get("ptr", 0)
                if target_entity_temp_ptr == 0 or target_entity_temp_ptr == local_player_ptr_val.value :
                    continue
                target_entity_obj = game.get_entity(target_entity_temp_ptr, apex_mem, ctx.g_base)
                if not target_entity_obj or not target_entity_obj.get_ptr() or not target_entity_obj.is_alive():
                    continue
                potential_aim_angles = game.calculate_best_bone_aim(local_player, target_entity_obj, ctx.max_fov * 1.5, ctx.bone_target, GAME_OFFSETS)
                if potential_aim_angles:
                    fov_to_this_target = game.calculate_fov_distance(current_view_angles, potential_aim_angles)
                    if fov_to_this_target < min_fov_from_crosshair:
                        min_fov_from_crosshair = fov_to_this_target
                        best_target_ptr = target_entity_temp_ptr
            ctx.aim_target_entity_ptr = best_target_ptr
            if ctx.aim_target_entity_ptr != 0:
                aim_target = game.get_entity(ctx.aim_target_entity_ptr, apex_mem, ctx.g_base)
                if aim_target and aim_target.is_alive():
                    final_aim_angles = game.calculate_best_bone_aim(local_player, aim_target, ctx.max_fov, ctx.bone_target, GAME_OFFSETS)
                    if final_aim_angles:
                        if ctx.aim_no_recoil:
                            recoil = local_player.get_recoil(GAME_OFFSETS)
                            if recoil:
                                final_aim_angles.x -= recoil.x
                                final_aim_angles.y -= recoil.y
                                final_aim_angles = game.normalize_angles(final_aim_angles)
                        if ctx.smooth_factor > 0.0 and ctx.smooth_factor < 1.0:
                            delta_x = game.normalize_angle(final_aim_angles.x - current_view_angles.x)
                            delta_y = game.normalize_angle(final_aim_angles.y - current_view_angles.y)
                            final_aim_angles.x = game.normalize_angle(current_view_angles.x + delta_x * (1.0 - ctx.smooth_factor))
                            final_aim_angles.y = game.normalize_angle(current_view_angles.y + delta_y * (1.0 - ctx.smooth_factor))
                            final_aim_angles = game.normalize_angles(final_aim_angles)
                        local_player.set_view_angles(final_aim_angles, GAME_OFFSETS)
        except Exception as e:
            ctx.aim_target_entity_ptr = 0
        time.sleep(0.001)

def esp_loop(ctx: GameContext, apex_mem: Memory):
    local_player_obj = None
    while ctx.active:
        if not ctx.g_base or not ctx.esp_enabled or not ctx.esp_next_update:
            time.sleep(0.01)
            if not ctx.g_base or not ctx.esp_enabled:
                ctx.esp_players_data.clear()
                ctx.esp_valid_data = False
            continue
        current_esp_data_dicts = []
        ctx.esp_valid_data = False
        try:
            local_player_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_LOCAL_ENT"], local_player_ptr_val) or local_player_ptr_val.value == 0:
                raise ValueError("Local player pointer not found")
            local_player_obj = game.get_entity(local_player_ptr_val.value, apex_mem, ctx.g_base)
            if not local_player_obj or not local_player_obj.get_ptr() or not local_player_obj.is_alive():
                raise ValueError("Local player object invalid or not alive")
            ctx.team_player_id = local_player_obj.get_team_id()
            local_player_pos = local_player_obj.get_position()
            view_renderer_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_RENDER"], view_renderer_ptr_val) or view_renderer_ptr_val.value == 0:
                raise ValueError("View renderer pointer not found")
            view_matrix_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(view_renderer_ptr_val.value + GAME_OFFSETS["OFFSET_MATRIX"], view_matrix_ptr_val) or view_matrix_ptr_val.value == 0:
                raise ValueError("View matrix pointer not found")
            view_matrix = structs.MatrixData()
            if not apex_mem.Read(view_matrix_ptr_val.value, view_matrix):
                raise ValueError("Failed to read view matrix")
            entity_list_base_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_ENTITYLIST"], entity_list_base_val) or entity_list_base_val.value == 0:
                raise ValueError("Entity list base pointer not found")
            game_settings_for_player_processing = ctx.to_dict_for_processing()
            for i in range(ctx.MAX_ENTITY_COUNT):
                target_entity_ptr_val = ctypes.c_uint64()
                if not apex_mem.Read(entity_list_base_val.value + (i << 5), target_entity_ptr_val): continue
                if target_entity_ptr_val.value == 0 or target_entity_ptr_val.value == local_player_ptr_val.value:
                    continue
                target_entity = game.get_entity(target_entity_ptr_val.value, apex_mem, ctx.g_base)
                if not target_entity or not target_entity.get_ptr():
                    continue

                # Apply is_dummy / is_player filtering before expensive operations
                if ctx.firing_range:
                    if not target_entity.is_dummy(GAME_OFFSETS):
                        continue
                else: # Not in firing range, expect players
                    if not target_entity.is_player(GAME_OFFSETS):
                        continue

                game.process_player(apex_mem, ctx.g_base, local_player_obj, target_entity, i, game_settings_for_player_processing)

                if not target_entity.is_alive(): continue # Already checked by is_player, but good as a safeguard

                entity_team = target_entity.get_team_id()
                # This team check is specific for ESP list population. process_player handles glow/aim team logic.
                if not ctx.firing_range and not ctx.onevone_mode and \
                   (entity_team < 0 or entity_team > 50 or entity_team == ctx.team_player_id):
                    continue

                target_pos = target_entity.get_position()
                dist = local_player_pos.dist_to(target_pos)
                if dist > ctx.max_dist: continue # Filter by distance for ESP

                screen_pos_base = game.world_to_screen(target_pos, view_matrix, ctx.SCREEN_WIDTH, ctx.SCREEN_HEIGHT)
                head_pos_offset = structs.Vector(x=0, y=0, z=75.0)
                world_head_pos = structs.Vector(x=target_pos.x + head_pos_offset.x,
                                                y=target_pos.y + head_pos_offset.y,
                                                z=target_pos.z + head_pos_offset.z)
                screen_pos_head = game.world_to_screen(world_head_pos, view_matrix, ctx.SCREEN_WIDTH, ctx.SCREEN_HEIGHT)
                if screen_pos_base and screen_pos_head:
                    height = abs(screen_pos_head.y - screen_pos_base.y)
                    width = height / 2.0
                    player_data_dict = {
                        "name": (target_entity.get_name_from_sign_name() or f"Entity_{i}"),
                        "health": target_entity.get_health(), "shield": target_entity.get_shield(),
                        "maxshield": target_entity.get_max_shield(), "dist": dist,
                        "entity_team": entity_team, "knocked": target_entity.is_knocked(),
                        "visible": target_entity.last_visible_time() > ctx.last_vis_aim_times.get(i, 0.0),
                        "box_base_x": screen_pos_base.x, "box_base_y": screen_pos_base.y,
                        "box_head_y": screen_pos_head.y, "box_width": width, "box_height": height,
                        "ptr": target_entity.get_ptr()
                    }
                    current_esp_data_dicts.append(player_data_dict)
            if current_esp_data_dicts:
                ctx.esp_players_data = current_esp_data_dicts
                ctx.esp_valid_data = True
        except ValueError as ve:
            ctx.esp_players_data.clear()
            ctx.esp_valid_data = False
        except Exception as e:
            ctx.esp_players_data.clear()
            ctx.esp_valid_data = False
        finally:
            ctx.esp_next_update = False
            time.sleep(0.01)

def do_actions_loop(ctx: GameContext, apex_mem: Memory):
    while ctx.active:
        if not (ctx.g_base and ctx.active):
            time.sleep(0.1)
            continue
        try:
            local_player_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_LOCAL_ENT"], local_player_ptr_val) or local_player_ptr_val.value == 0:
                time.sleep(0.1); continue
            local_player = game.get_entity(local_player_ptr_val.value, apex_mem, ctx.g_base)
            if not local_player or not local_player.get_ptr() or not local_player.is_alive():
                time.sleep(0.1); continue

            if ctx.wall_jump_enabled:
                actions.handle_wall_jump(local_player, apex_mem, ctx, GAME_OFFSETS, ctx.action_states['wall_jump'])
            if ctx.super_glide_enabled and ctx.is_jump_key_pressed:
                actions.handle_super_glide(local_player, apex_mem, ctx, GAME_OFFSETS, ctx.action_states['super_glide'])
            if ctx.grapple_boost_enabled:
                actions.handle_grapple_boost(local_player, apex_mem, ctx, GAME_OFFSETS, ctx.action_states['grapple_boost'])
        except Exception as e:
            pass
        time.sleep(0.001)

if __name__ == "__main__":
    print("Starting PyApexDMA...")
    apex_mem = Memory()
    game_ctx = GameContext()
    ipc_server = IPCServer(game_ctx)
    ipc_server_thread = threading.Thread(target=ipc_server.start_server, daemon=True)
    threads = [ipc_server_thread]
    ipc_server_thread.start()
    game_ctx.ipc_server_handle = ipc_server
    try:
        while game_ctx.active:
            if apex_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                if game_ctx.g_base != 0:
                    game_ctx.g_base = 0
                    game_ctx.status_message = "Server Active - Apex Process Not Found"
                    game_ctx.is_in_game = False
                    game_ctx.esp_players_data.clear()
                    game_ctx.esp_valid_data = False
                    ipc_server.broadcast_game_state()
                if apex_mem.open_proc("r5apex_dx12.exe"):
                    print(f"Apex process opened successfully. PID: {apex_mem.get_proc_pid()}")
                    game_ctx.g_base = apex_mem.get_proc_baseaddr()
                    print(f"Apex base address: {hex(game_ctx.g_base)}")
                    game_ctx.status_message = "Server Active - Apex Found, Initializing..."
                    if game_ctx.aimbot_thread_handle is None or not game_ctx.aimbot_thread_handle.is_alive():
                        game_ctx.aimbot_thread_handle = threading.Thread(target=aimbot_loop, args=(game_ctx, apex_mem), daemon=True)
                        threads.append(game_ctx.aimbot_thread_handle)
                        game_ctx.aimbot_thread_handle.start()
                    if game_ctx.esp_thread_handle is None or not game_ctx.esp_thread_handle.is_alive():
                        game_ctx.esp_thread_handle = threading.Thread(target=esp_loop, args=(game_ctx, apex_mem), daemon=True)
                        threads.append(game_ctx.esp_thread_handle)
                        game_ctx.esp_thread_handle.start()
                    if game_ctx.do_actions_thread_handle is None or not game_ctx.do_actions_thread_handle.is_alive():
                        game_ctx.do_actions_thread_handle = threading.Thread(target=do_actions_loop, args=(game_ctx, apex_mem), daemon=True)
                        threads.append(game_ctx.do_actions_thread_handle)
                        game_ctx.do_actions_thread_handle.start()
                else:
                    time.sleep(3)
            else:
                apex_mem.check_proc()
                if apex_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                    game_ctx.g_base = 0
                    game_ctx.status_message = "Server Active - Apex Process Lost"
                    game_ctx.is_in_game = False
                    game_ctx.esp_players_data.clear()
                    game_ctx.esp_valid_data = False
                    ipc_server.broadcast_game_state()
                    continue
                if game_ctx.g_base != 0:
                    signon_state_val = ctypes.c_int()
                    level_name_raw = (ctypes.c_char * 64)()
                    if apex_mem.Read(game_ctx.g_base + GAME_OFFSETS["OFFSET_SIGNONSTATE"], signon_state_val) and \
                       apex_mem.ReadArray(game_ctx.g_base + GAME_OFFSETS["OFFSET_LEVELNAME"], level_name_raw):
                        current_map_name = level_name_raw.value.decode('utf-8', 'ignore').strip()
                        current_is_in_game = (signon_state_val.value == 7)
                        if game_ctx.map_name != current_map_name or game_ctx.is_in_game != current_is_in_game:
                            game_ctx.map_name = current_map_name
                            game_ctx.is_in_game = current_is_in_game
                            game_ctx.status_message = f"Server Active - In-Game: {game_ctx.is_in_game}, Map: {game_ctx.map_name}"
                            game_ctx.esp_players_data.clear()
                            game_ctx.esp_valid_data = False
                        if "mp_lobby" in game_ctx.map_name or not game_ctx.is_in_game:
                            game_ctx.firing_range = False
                            if game_ctx.esp_players_data or game_ctx.esp_valid_data:
                                game_ctx.esp_players_data.clear()
                                game_ctx.esp_valid_data = False
                        else:
                            game_ctx.firing_range = "mp_rr_canyonlands_staging" in game_ctx.map_name
                    else:
                        if game_ctx.is_in_game or game_ctx.map_name != "N/A":
                            game_ctx.is_in_game = False
                            game_ctx.map_name = "N/A"
                            game_ctx.status_message = "Server Active - Game State Unknown"
                            game_ctx.esp_players_data.clear()
                            game_ctx.esp_valid_data = False
                elif game_ctx.g_base == 0 and game_ctx.is_in_game :
                    game_ctx.is_in_game = False
                    game_ctx.map_name = "N/A"
                    game_ctx.status_message = "Server Active - Apex Not Running"
                    game_ctx.esp_players_data.clear()
                    game_ctx.esp_valid_data = False
                    ipc_server.broadcast_game_state()
                if game_ctx.g_base != 0 and game_ctx.is_in_game and game_ctx.esp_enabled:
                    if not game_ctx.esp_next_update and not game_ctx.esp_valid_data:
                        game_ctx.esp_next_update = True
                if game_ctx.esp_valid_data:
                    ipc_server.broadcast_game_state()
                    game_ctx.esp_valid_data = False
            time.sleep(game_ctx.main_sleep)
    except KeyboardInterrupt:
        print("Keyboard interrupt received. Shutting down...")
    finally:
        game_ctx.active = False
        if ipc_server:
            print("Stopping IPC server...")
            ipc_server.stop_server()
        print("Waiting for threads to finish...")
        for t in threads:
           if t.is_alive():
               t.join(timeout=1.0)
        apex_mem.close_proc()
        # client_mem.close_proc() # Removed as client_mem is no longer used
        print("Processes closed. Exiting PyApexDMA.")

```
