import threading
import time
import ctypes

# Assuming the package structure allows these imports
from py_apex_dma.memory import Memory, ProcessStatus
from py_apex_dma import game
from py_apex_dma.offsets import GAME_OFFSETS
from py_apex_dma import structs
from .server_ipc import IPCServer # Import IPCServer

# CLIENT_ADD_OFFSET removed as client interaction is now via IPC server


class GameContext:
    def __init__(self):
        # Process-related
        self.g_base: int = 0  # Game base address (r5apex_dx12.exe)
        # self.c_base: int = 0 # Client base address (Client.exe) - No longer needed for direct mem access

        # Loop control
        self.active: bool = True # Overall main loop control

        # Feature toggles (defaults) - these will be updated by IPC client settings
        self.aim_enabled: bool = True
        self.esp_enabled: bool = True # This might just control if server sends ESP data
        self.player_glow_enabled: bool = True
        self.item_glow_enabled: bool = False
        self.trigger_bot_enabled: bool = False
        self.aim_visibility_check: bool = True # Added from ClientSettings
        self.aim_no_recoil: bool = True      # Added from ClientSettings

        # Aimbot specific
        self.aim_entity_ptr: int = 0
        self.max_dist: float = 200.0 * 40.0
        self.max_fov: float = 15.0
        self.smooth_factor: float = 0.2
        self.bone_target: int = 0
        self.aim_key_pressed: bool = False # This might be set by client or hooks later

        # DDS Settings from ClientSettings
        self.dds_enabled: bool = False
        self.dds_distance: float = 100.0 * 40.0
        self.dds_min_fov: float = 5.0
        self.dds_max_fov: float = 20.0
        self.dds_min_smooth: float = 0.1
        self.dds_max_smooth: float = 0.3

        # Game mode / context
        self.firing_range: bool = False
        self.onevone_mode: bool = False
        self.team_player_id: int = 0

        # Spectator info
        self.spectators_count: int = 0
        self.allied_spectators_count: int = 0

        # ESP / Glow related
        self.last_vis_aim_times: dict[int, float] = {}
        self.esp_players_data: list[dict] = [] # Store as list of dicts now for easier IPC
        self.esp_valid_data: bool = False
        self.esp_next_update: bool = True # Main loop signals ESP to update with this
        self.status_message: str = "Server Initializing" # For server status

        # Glow Colors (will be updated by IPC client settings)
        # Using UI-friendly names from ClientSettings. These are lists [R,G,B,A] 0-1 floats.
        self.glow_visible_ui: list[float] = [1.0, 0.0, 0.0, 1.0]  # Bright Red
        self.glow_not_visible_ui: list[float] = [0.0, 1.0, 0.0, 1.0]  # Bright Green
        self.glow_knocked_ui: list[float] = [0.0, 0.0, 1.0, 1.0]  # Bright Blue

        # Visuals dictionary from ClientSettings
        self.visuals: dict = {
            "box_esp": True, "name_esp": True, "circle_fov_enabled": True,
            "line_esp": True, "distance_esp": True, "health_bar_esp": True,
            "shield_bar_esp": True, "circle_fov_size": self.max_fov
        }

        # Additional settings
        self.main_sleep: float = 0.001
        self.map_name: str = ""
        self.is_in_game: bool = False

        # Thread handles
        self.aimbot_thread_handle = None
        self.esp_thread_handle = None
        self.do_actions_thread_handle = None
        # self.vars_thread_handle = None # Removed, replaced by IPC server
        self.item_glow_thread_handle = None
        self.ipc_server_handle = None # For IPCServer thread

        # Constants for ESP loop / screen dimensions
        self.MAX_ENTITY_COUNT = 100 # Max entities to loop through
        self.SCREEN_WIDTH = 1920    # Default screen width for W2S
        self.SCREEN_HEIGHT = 1080   # Default screen height for W2S


    def update_settings(self, settings_data: dict):
        # print(f"GameContext: Updating settings from IPC: {settings_data}")
        self.aim_enabled = settings_data.get("aim_enabled", self.aim_enabled)
        self.esp_enabled = settings_data.get("esp_enabled", self.esp_enabled) # Client might just toggle its display
        self.player_glow_enabled = settings_data.get("player_glow_enabled", self.player_glow_enabled)
        self.item_glow_enabled = settings_data.get("item_glow_enabled", self.item_glow_enabled) # Example
        self.aim_visibility_check = settings_data.get("aim_visibility_check", self.aim_visibility_check)
        self.aim_no_recoil = settings_data.get("aim_no_recoil", self.aim_no_recoil)

        self.max_dist = settings_data.get("max_dist", self.max_dist)
        self.max_fov = settings_data.get("max_fov", self.max_fov)
        self.smooth_factor = settings_data.get("smooth_aim_value", self.smooth_factor) # Key name from client
        self.bone_target = settings_data.get("aim_bone", self.bone_target) # Key name from client

        self.dds_enabled = settings_data.get("dds_enabled", self.dds_enabled)
        self.dds_distance = settings_data.get("dds_distance", self.dds_distance)
        self.dds_min_fov = settings_data.get("dds_min_fov", self.dds_min_fov)
        self.dds_max_fov = settings_data.get("dds_max_fov", self.dds_max_fov)
        self.dds_min_smooth = settings_data.get("dds_min_smooth", self.dds_min_smooth)
        self.dds_max_smooth = settings_data.get("dds_max_smooth", self.dds_max_smooth)

        self.firing_range = settings_data.get("firing_range", self.firing_range) # Client might not set this
        self.onevone_mode = settings_data.get("onevone_mode", self.onevone_mode) # Client might not set this

        # Update glow colors (expecting [R,G,B,A] list from client's to_dict)
        self.glow_visible_ui = settings_data.get("glow_visible_ui", self.glow_visible_ui)
        self.glow_not_visible_ui = settings_data.get("glow_not_visible_ui", self.glow_not_visible_ui)
        self.glow_knocked_ui = settings_data.get("glow_knocked_ui", self.glow_knocked_ui)

        # Update visuals dictionary
        client_visuals = settings_data.get("visuals", {})
        if isinstance(client_visuals, dict): # Ensure it's a dict before trying to update
            self.visuals.update(client_visuals)

        # Update game_settings for process_player based on new glow colors
        # The set_player_glow function uses game_settings dict with specific keys for colors
        # We need to ensure these keys are correctly populated from self.glow_..._ui
        # This might be better done where game_settings for process_player is constructed.

        # print("GameContext: Settings updated.")


    def get_serializable_state(self) -> dict:
        # Prepare ESP data: convert list of PlayerData objects to list of dicts
        esp_players_list_of_dicts = []
        if self.esp_valid_data: # Only send if data is considered fresh by ESP loop
            for player_obj in self.esp_players_data:
                # This conversion is now done when populating mock data or in real ESP loop
                esp_players_list_of_dicts = self.esp_players_data # Already list of dicts

        return {
            "status": self.status_message, # Renamed from status_message for client consistency
            "map_name": self.map_name,
            # Spectator data structure for client: nested dict
            "spectators": {
                "enemy": self.spectators_count, # Assuming this is total, client can interpret
                "allied": self.allied_spectators_count
            },
            "esp_players": esp_players_list_of_dicts,
            "firing_range": self.firing_range,
            # Add any other state client needs
        }

    def to_dict_for_processing(self) -> dict:
        """Gathers settings needed by process_player and other game logic functions."""
        return {
            "firing_range": self.firing_range,
            "onevone_mode": self.onevone_mode,
            "team_player_id": self.team_player_id,
            "max_dist": self.max_dist, # General max distance from aim settings
            "player_glow_enabled": self.player_glow_enabled,
            "last_vis_aim_times": self.last_vis_aim_times,
            # Glow colors for set_player_glow
            "glowr": self.glow_not_visible_ui[0],
            "glowg": self.glow_not_visible_ui[1],
            "glowb": self.glow_not_visible_ui[2],
            "glowrviz": self.glow_visible_ui[0],
            "glowgviz": self.glow_visible_ui[1],
            "glowbviz": self.glow_visible_ui[2],
            "glowrknc": self.glow_knocked_ui[0],
            "glowgknc": self.glow_knocked_ui[1],
            "glowbknc": self.glow_knocked_ui[2],
            # Pass other relevant settings from GameContext if needed by process_player
            "aim_enabled": self.aim_enabled, # Example if process_player needs it
        }

def aimbot_loop(ctx: GameContext, apex_mem: Memory):
    # print("Aimbot loop started.") # Reduced verbosity
    # print("Aimbot loop started.") # Reduced verbosity
    while ctx.active:
        if ctx.g_base == 0: # Game not ready
            time.sleep(0.1)
            continue
        # Aimbot logic here
        time.sleep(0.001)
    # print("Aimbot loop ended.") # Reduced verbosity

def esp_loop(ctx: GameContext, apex_mem: Memory):
    # print("ESP loop started.") # Reduced verbosity
    local_player_obj = None

    while ctx.active:
        if not ctx.g_base or not ctx.esp_enabled or not ctx.esp_next_update:
            time.sleep(0.01) # Sleep if not ready, ESP disabled, or not time for next update
            if not ctx.g_base or not ctx.esp_enabled: # Clear data if disabled or game not running
                ctx.esp_players_data.clear()
                ctx.esp_valid_data = False
            continue

        current_esp_data_dicts = []
        ctx.esp_valid_data = False # Reset validity for current pass

        try:
            # Get Local Player
            local_player_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_LOCAL_ENT"], local_player_ptr_val) or local_player_ptr_val.value == 0:
                raise ValueError("Local player pointer not found")

            local_player_obj = game.get_entity(local_player_ptr_val.value, apex_mem, ctx.g_base)
            if not local_player_obj or not local_player_obj.get_ptr() or not local_player_obj.is_alive():
                raise ValueError("Local player object invalid or not alive")

            ctx.team_player_id = local_player_obj.get_team_id()
            local_player_pos = local_player_obj.get_position()

            # Read View Matrix
            view_renderer_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_RENDER"], view_renderer_ptr_val) or view_renderer_ptr_val.value == 0:
                raise ValueError("View renderer pointer not found")

            view_matrix_ptr_val = ctypes.c_uint64()
            if not apex_mem.Read(view_renderer_ptr_val.value + GAME_OFFSETS["OFFSET_MATRIX"], view_matrix_ptr_val) or view_matrix_ptr_val.value == 0:
                raise ValueError("View matrix pointer not found")

            view_matrix = structs.MatrixData()
            if not apex_mem.Read(view_matrix_ptr_val.value, view_matrix):
                raise ValueError("Failed to read view matrix")

            # Entity List Iteration
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

                game.process_player(apex_mem, ctx.g_base, local_player_obj, target_entity, i, game_settings_for_player_processing)

                if not target_entity.is_alive(): continue

                entity_team = target_entity.get_team_id()
                if not ctx.firing_range and not ctx.onevone_mode and \
                   (entity_team < 0 or entity_team > 50 or entity_team == ctx.team_player_id):
                    continue

                target_pos = target_entity.get_position()
                dist = local_player_pos.dist_to(target_pos)
                if dist > ctx.max_dist: continue

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
                    }
                    current_esp_data_dicts.append(player_data_dict)

            if current_esp_data_dicts:
                ctx.esp_players_data = current_esp_data_dicts
                ctx.esp_valid_data = True

        except ValueError as ve: # Catch specific errors for failed reads/conditions
            # print(f"ESP Loop data gathering error: {ve}")
            ctx.esp_players_data.clear() # Clear data on error
            ctx.esp_valid_data = False
        except Exception as e: # Catch any other unexpected errors
            # print(f"ESP Loop unexpected error: {e}")
            ctx.esp_players_data.clear()
            ctx.esp_valid_data = False
        finally:
            ctx.esp_next_update = False # Ensure this is always reset
            time.sleep(0.01) # Base sleep for the loop after a pass or if conditions not met early


def do_actions_loop(ctx: GameContext, apex_mem: Memory):
    # print("DoActions loop started.") # Reduced verbosity
    while ctx.active:
        if ctx.g_base == 0:
            time.sleep(0.1)
            continue
        time.sleep(0.001)
    # print("DoActions loop ended.") # Reduced verbosity

# set_vars_loop and its direct Client.exe memory interaction is removed.
# IPCServer will now handle settings updates from the external client.


if __name__ == "__main__":
    print("Starting PyApexDMA...")

    apex_mem = Memory()
    # client_mem = Memory() # Removed, no longer directly accessing Client.exe memory
    game_ctx = GameContext()

    # Instantiate and start IPCServer
    ipc_server = IPCServer(game_ctx) # Pass GameContext reference
    ipc_server_thread = threading.Thread(target=ipc_server.start_server, daemon=True)
    threads = [ipc_server_thread] # Add to threads list for potential join later, though daemon usually suffices
    ipc_server_thread.start()
    game_ctx.ipc_server_handle = ipc_server # Store for potential access, though not used in this snippet

    # Main loop and other thread initializations
    # ... (rest of the main loop from previous version, removing client_mem parts) ...

    try:
        while game_ctx.active:
            if apex_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                # print("Apex process not found or not ready. Attempting to open...") # Reduced verbosity
                if game_ctx.g_base != 0:
                    # print("Apex process lost. Signaling dependent threads to pause.") # Reduced verbosity
                    game_ctx.g_base = 0

                if apex_mem.open_proc("r5apex_dx12.exe"):
                    print(f"Apex process opened successfully. PID: {apex_mem.get_proc_pid()}")
                    game_ctx.g_base = apex_mem.get_proc_baseaddr()
                    print(f"Apex base address: {hex(game_ctx.g_base)}")

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
                    # print("Failed to open Apex process. Retrying in a moment...") # Reduced verbosity
                    time.sleep(3)
            else:
                apex_mem.check_proc()
                if apex_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                    # print("Apex process check failed. Will attempt to reopen.") # Reduced verbosity
                    game_ctx.g_base = 0
                    continue

                if game_ctx.is_in_game and game_ctx.esp_enabled and not game_ctx.esp_next_update:
                    # Main loop is now responsible for triggering ESP data refresh and broadcast
                    game_ctx.esp_next_update = True # Signal ESP loop to gather data
                    # Wait for ESP loop to set esp_valid_data and esp_next_update = False
                    # This simple flag might lead to race conditions or missed broadcasts.
                    # A more robust sync (e.g., event or condition variable) might be better.
                    # For now, assume ESP loop is fast enough.
                    # OR: ESP loop directly calls broadcast_game_state when it's done.
                    # Let's try having ESP loop call it for now for tighter coupling.
                    # No, the original design was broadcast from main.
                    # If esp_loop sets esp_valid_data = True and esp_next_update = False
                    # then main loop can check esp_valid_data.

                    # The broadcast should happen after esp_loop has prepared the data.
                    # The current esp_loop sets esp_next_update = False when done.
                    # So, if it's false, data *should* be ready if esp_valid_data is true.
                    if game_ctx.esp_valid_data:
                         ipc_server.broadcast_game_state()
                         # game_ctx.esp_valid_data = False # Let mock data generation control this for now

                # Mock Data Generation and Broadcast Trigger (if g_base is "valid")
                # This section simulates game activity and data generation for testing IPC
                # In a real scenario, this data would come from game memory reads.
                if game_ctx.g_base != 0: # Simulate game being "active" enough for mock data
                    # Mock data generation removed. esp_loop handles data if game is running.
                    pass

                # Real game state update (signon, map)
                if game_ctx.g_base != 0:
                    signon_state_val = ctypes.c_int()
                    level_name_raw = (ctypes.c_char * 64)()
                    if apex_mem.Read(game_ctx.g_base + GAME_OFFSETS["OFFSET_SIGNONSTATE"], signon_state_val) and \
                       apex_mem.ReadArray(game_ctx.g_base + GAME_OFFSETS["OFFSET_LEVELNAME"], level_name_raw):

                        current_map_name = level_name_raw.value.decode('utf-8', 'ignore').strip()
                        current_is_in_game = (signon_state_val.value == 7) # SIGNONSTATE_FULL

                        # Update context if state changed
                        if game_ctx.map_name != current_map_name or game_ctx.is_in_game != current_is_in_game:
                            game_ctx.map_name = current_map_name
                            game_ctx.is_in_game = current_is_in_game
                            game_ctx.status_message = f"Server Active - In-Game: {game_ctx.is_in_game}, Map: {game_ctx.map_name}"
                            # Clear ESP data on map change or exiting game logic
                            game_ctx.esp_players_data.clear()
                            game_ctx.esp_valid_data = False

                        if "mp_lobby" in game_ctx.map_name or not game_ctx.is_in_game:
                            game_ctx.firing_range = False
                            if game_ctx.esp_players_data or game_ctx.esp_valid_data:
                                game_ctx.esp_players_data.clear()
                                game_ctx.esp_valid_data = False
                        else:
                            game_ctx.firing_range = "mp_rr_canyonlands_staging" in game_ctx.map_name
                    else: # Failed to read game state
                        if game_ctx.is_in_game or game_ctx.map_name != "N/A": # If state changed to unknown
                            game_ctx.is_in_game = False
                            game_ctx.map_name = "N/A"
                            game_ctx.status_message = "Server Active - Game State Unknown"
                            game_ctx.esp_players_data.clear()
                            game_ctx.esp_valid_data = False
                elif game_ctx.g_base == 0 and game_ctx.is_in_game : # Game was running, now it's not
                    game_ctx.is_in_game = False
                    game_ctx.map_name = "N/A" # Reset map name
                    game_ctx.status_message = "Server Active - Apex Not Running"
                    game_ctx.esp_players_data.clear()
                    game_ctx.esp_valid_data = False
                    # Broadcast this cleared state once
                    ipc_server.broadcast_game_state()


                # Trigger ESP loop to run if game is active and it's time for an update
                # esp_loop itself checks g_base, esp_enabled, and esp_next_update.
                # Main loop just needs to set esp_next_update = True when it wants a refresh.
                if game_ctx.g_base != 0 and game_ctx.is_in_game and game_ctx.esp_enabled:
                    if not game_ctx.esp_next_update and not game_ctx.esp_valid_data: # Only trigger if previous data was consumed or invalid
                        game_ctx.esp_next_update = True

                # Broadcast if new valid data is available from esp_loop
                # esp_loop sets esp_valid_data = True and esp_next_update = False.
                if game_ctx.esp_valid_data:
                    ipc_server.broadcast_game_state()
                    game_ctx.esp_valid_data = False # Reset flag, ESP loop will set it true on next successful pass

            time.sleep(game_ctx.main_sleep)

    except KeyboardInterrupt:
        print("Keyboard interrupt received. Shutting down...")
    finally:
        game_ctx.active = False # Signal all threads to terminate
        print("Waiting for threads to finish...")
        # Wait for daemon threads implicitly on main thread exit, or join if not daemon.
        # If not daemon, explicit join is needed:
        # for t in threads:
        #    if t.is_alive():
        #        t.join(timeout=1.0) # Add timeout to prevent indefinite blocking

        apex_mem.close_proc()
        client_mem.close_proc()
        print("Processes closed. Exiting PyApexDMA.")
