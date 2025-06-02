import threading
import time
import ctypes

# Assuming the package structure allows these imports
from py_apex_dma.memory import Memory, ProcessStatus
from py_apex_dma import game
from py_apex_dma.offsets import GAME_OFFSETS
from py_apex_dma import structs

# CLIENT_ADD_OFFSET: Base offset in Client.exe from which other addresses are read.
# This value (0x0) is a placeholder from the C++ code and likely needs to be a real,
# identified offset within Client.exe's memory space if this mechanism is to work.
CLIENT_ADD_OFFSET = 0x0000000

class GameContext:
    def __init__(self):
        # Process-related
        self.g_base: int = 0  # Game base address (r5apex_dx12.exe)
        self.c_base: int = 0  # Client base address (Client.exe)

        # Loop control
        self.active: bool = True # Overall main loop control

        # Feature toggles (defaults)
        self.aim_enabled: bool = True
        self.esp_enabled: bool = True
        self.player_glow_enabled: bool = True
        self.item_glow_enabled: bool = False # Assuming this might be a feature
        self.trigger_bot_enabled: bool = False # Assuming

        # Aimbot specific
        self.aim_entity_ptr: int = 0 # Current entity pointer for aimbot
        self.max_dist: float = 200.0 * 40.0 # Max distance for aim/esp (converted from meters to game units)
        self.max_fov: float = 15.0 # Max FOV for aimbot
        self.smooth_factor: float = 0.2 # Smoothing for aimbot
        self.bone_target: int = 0 # Target bone for aimbot (0 might be head or default)
        self.aim_key_pressed: bool = False # If aim key is active

        # Game mode / context
        self.firing_range: bool = False
        self.onevone_mode: bool = False # 1v1 mode flag
        self.team_player_id: int = 0 # Local player's team ID

        # Spectator info
        self.spectators_count: int = 0
        self.allied_spectators_count: int = 0

        # ESP / Glow related
        self.last_vis_aim_times: dict[int, float] = {} # target_index: last_visible_time
        self.esp_players_data: list[structs.PlayerData] = [] # List of PlayerData for ESP
        self.esp_valid_data: bool = False # If esp_players_data is fresh
        self.esp_next_update: bool = True # Trigger for ESP data update in its loop

        # Glow Colors (using names from description, can be more structured if needed)
        # These would ideally be loaded from a config or UI, hardcoded for now
        self.glow_r: float = game.GLOW_R # Non-Visible RED
        self.glow_g: float = game.GLOW_G # Non-Visible GREEN
        self.glow_b: float = game.GLOW_B # Non-Visible BLUE
        self.glow_r_viz: float = game.GLOW_R_VIZ # Visible RED
        self.glow_g_viz: float = game.GLOW_G_VIZ # Visible GREEN
        self.glow_b_viz: float = game.GLOW_B_VIZ # Visible BLUE
        self.glow_r_knc: float = game.GLOW_R_KNC # Knocked RED
        self.glow_g_knc: float = game.GLOW_G_KNC # Knocked GREEN
        self.glow_b_knc: float = game.GLOW_B_KNC # Knocked BLUE

        # Additional settings mirroring C++ globals if needed for loops
        self.main_sleep: float = 0.001 # Main loop sleep
        self.map_name: str = ""
        self.is_in_game: bool = False # e.g. SignOnState == 7

        # Client.exe shared memory addresses and state
        self.client_vars_initialized: bool = False
        self.client_vars_valid: bool = False

        self.client_check_addr: int = 0
        self.client_aim_addr: int = 0
        self.client_esp_addr: int = 0
        self.client_player_glow_addr: int = 0 # Assuming separate from general esp toggle
        self.client_item_glow_addr: int = 0
        self.client_aim_key_addr: int = 0
        self.client_bone_target_addr: int = 0
        self.client_max_dist_addr: int = 0
        self.client_max_fov_addr: int = 0
        self.client_smooth_factor_addr: int = 0

        self.client_firing_range_addr: int = 0
        self.client_onevone_mode_addr: int = 0

        self.client_glow_r_addr: int = 0
        self.client_glow_g_addr: int = 0
        self.client_glow_b_addr: int = 0
        self.client_glow_r_viz_addr: int = 0
        self.client_glow_g_viz_addr: int = 0
        self.client_glow_b_viz_addr: int = 0
        self.client_glow_r_knc_addr: int = 0
        self.client_glow_g_knc_addr: int = 0
        self.client_glow_b_knc_addr: int = 0

        self.client_g_base_addr: int = 0
        self.client_spectators_addr: int = 0
        self.client_allied_spectators_addr: int = 0
        self.client_player_addr: int = 0 # For ESP player data array
        self.client_valid_addr: int = 0  # For ESP valid flag
        self.client_next_addr: int = 0   # For ESP sync/next flag

        # Thread handles (optional, can be managed in main scope)
        self.aimbot_thread_handle = None
        self.esp_thread_handle = None
        self.do_actions_thread_handle = None
        self.vars_thread_handle = None
        self.item_glow_thread_handle = None # If item glow has its own loop

# Placeholder loop functions - will be defined in the next step
def aimbot_loop(ctx: GameContext, apex_mem: Memory):
    print("Aimbot loop started.")
    while ctx.active:
        if ctx.g_base == 0: # Game not ready
            time.sleep(0.1)
            continue
        # Aimbot logic here
        time.sleep(0.001)
    print("Aimbot loop ended.")

def esp_loop(ctx: GameContext, apex_mem: Memory):
    print("ESP loop started.")
    local_player = None # Define local_player outside the loop if it needs to persist or be checked

    while ctx.active:
        if ctx.g_base == 0: # Game not ready
            time.sleep(0.1)
            local_player = None # Reset local_player if game is no longer ready
            ctx.esp_players_data.clear()
            ctx.esp_valid_data = False
            continue

        # Simplified ESP logic structure from apex_dma.cpp
        if not ctx.esp_enabled:
            time.sleep(0.1)
            ctx.esp_players_data.clear()
            ctx.esp_valid_data = False
            continue

        if not ctx.esp_next_update: # Wait for main loop to signal update
            time.sleep(0.001) # Sleep briefly
            continue

        temp_esp_data = []

        # Get Local Player
        try:
            local_entity_handle = ctypes.c_uint64()
            apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_LOCAL_ENTITY_HANDLE"], local_entity_handle)

            local_player_ptr = ctypes.c_uint64()
            apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_ENTITYLIST"] + ((local_entity_handle.value & 0xFFFF) << 5), local_player_ptr)

            if local_player_ptr.value == 0:
                time.sleep(0.1) # Wait if local player not found
                local_player = None
                continue

            local_player = game.get_entity(local_player_ptr.value, apex_mem, ctx.g_base)
            if not local_player or not local_player.is_alive(): # Check if local_player is valid and alive
                time.sleep(0.1)
                local_player = None
                continue

            # Update team_player_id if not in firing range
            if not ctx.firing_range:
                 ctx.team_player_id = local_player.get_team_id()

        except Exception as e:
            # print(f"ESP Error getting local player: {e}")
            local_player = None
            time.sleep(1) # Longer sleep on error
            continue

        # Entity List Iteration
        for i in range(1, 100): # Max players approx
            try:
                entity_ptr_val = ctypes.c_uint64()
                apex_mem.Read(ctx.g_base + GAME_OFFSETS["OFFSET_ENTITYLIST"] + (i << 5), entity_ptr_val)
                if entity_ptr_val.value == 0 or entity_ptr_val.value == local_player_ptr.value:
                    continue

                target_entity = game.get_entity(entity_ptr_val.value, apex_mem, ctx.g_base)
                if not target_entity: # Basic check
                    continue

                # process_player will handle glow, visibility checks, team checks etc.
                # For ESP, we might want a lighter version or just use parts of process_player logic
                # The C++ ESP loop collects data into PlayerData struct.

                # Simplified settings for process_player call from ESP context
                # (as ESP might have different rules than aimbot for display)
                esp_game_settings = {
                    "max_dist": ctx.max_dist, # Could be a separate esp_max_dist
                    "firing_range": ctx.firing_range,
                    "onevone": ctx.onevone_mode,
                    "team_player": ctx.team_player_id,
                    "player_glow_enabled": ctx.player_glow_enabled, # Let process_player handle glow via set_player_glow
                    "glowr": ctx.glow_r, "glowg": ctx.glow_g, "glowb": ctx.glow_b,
                    "glowrviz": ctx.glow_r_viz, "glowgviz": ctx.glow_g_viz, "glowbviz": ctx.glow_b_viz,
                    "glowrknc": ctx.glow_r_knc, "glowgknc": ctx.glow_g_knc, "glowbknc": ctx.glow_b_knc,
                    "last_vis_aim_times": ctx.last_vis_aim_times, # Shared for consistent glow logic
                }

                # Call process_player for its side effects (like glow) and checks.
                # For ESP data collection, we'd do it here.
                game.process_player(apex_mem, ctx.g_base, local_player, target_entity, i, esp_game_settings)

                # ESP Data Collection (simplified)
                if target_entity.is_alive(): # Re-check after process_player might have filtered
                    player_data = structs.PlayerData() # Assuming PlayerData is defined in structs
                    player_data.dist = local_player.get_position().dist_to(target_entity.get_position())

                    if player_data.dist > esp_game_settings["max_dist"]: # Check dist again after calculation
                        continue

                    player_data.entity_team = target_entity.get_team_id()

                    # Simplified team check for ESP display (might differ from process_player's filtering)
                    if not ctx.firing_range and not ctx.onevone_mode:
                        if player_data.entity_team < 0 or player_data.entity_team > 50 or player_data.entity_team == ctx.team_player_id:
                            continue # Don't add to ESP list if same team (unless FF or 1v1)

                    player_data.health = target_entity.get_health()
                    player_data.shield = target_entity.get_shield()
                    player_data.maxshield = target_entity.get_max_shield()
                    player_data.knocked = target_entity.is_knocked()
                    player_data.visible = target_entity.last_visible_time() > ctx.last_vis_aim_times.get(i, 0.0)

                    name_str = target_entity.get_name_from_sign_name()
                    player_data.name = name_str.encode('utf-8')[:32] # Ensure it fits PlayerData.name

                    # For box ESP, more calculations needed (bones, screen projection)
                    # player_data.boxMiddle, player_data.h_y, player_data.width, player_data.height etc.
                    # These are highly dependent on world_to_screen and bone data, which are placeholders.
                    # For now, we'll skip these complex calculations.

                    temp_esp_data.append(player_data)

            except Exception as e:
                # print(f"Error processing entity {i}: {e}")
                continue

        ctx.esp_players_data = temp_esp_data
        ctx.esp_valid_data = True
        ctx.esp_next_update = False # Signal that update is done

        time.sleep(0.01) # ESP loop update rate
    print("ESP loop ended.")


def do_actions_loop(ctx: GameContext, apex_mem: Memory):
    print("DoActions loop started.")
    while ctx.active:
        if ctx.g_base == 0:
            time.sleep(0.1)
            continue
        # Trigger bot, other actions logic here
        time.sleep(0.001)
    print("DoActions loop ended.")

def set_vars_loop(ctx: GameContext, client_mem: Memory): # Renamed from client_loop to set_vars_loop
    print("SetVars loop started.")
    while ctx.active:
        if ctx.c_base == 0: # Client.exe not ready
            time.sleep(0.1)
            continue
        # Logic to read/write variables from Client.exe's memory space
        # Example: Reading a boolean toggle for aim_enabled
        # aim_toggle_addr = ctx.c_base + 0xOFFSET_IN_CLIENT_FOR_AIM_TOGGLE
        # current_aim_toggle_val = ctypes.c_bool()
        # if client_mem.Read(aim_toggle_addr, current_aim_toggle_val):
        #    ctx.aim_enabled = current_aim_toggle_val.value

        if not ctx.client_vars_initialized and ctx.c_base != 0:
            print("SetVars: c_base valid, attempting to initialize client addresses...")
            add_addr = ctx.c_base + CLIENT_ADD_OFFSET
            addr_offset = 0
            temp_val = ctypes.c_uint64()

            try:
                # Read addresses of shared variables from Client.exe
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_check_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_g_base_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_spectators_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_allied_spectators_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_player_addr = temp_val.value; addr_offset += 8 # ESP Data Array
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_valid_addr = temp_val.value; addr_offset += 8  # ESP Valid Flag
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_next_addr = temp_val.value; addr_offset += 8   # ESP Next/Sync Flag

                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_aim_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_esp_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_player_glow_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_item_glow_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_aim_key_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_bone_target_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_max_dist_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_max_fov_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_smooth_factor_addr = temp_val.value; addr_offset += 8

                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_firing_range_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_onevone_mode_addr = temp_val.value; addr_offset += 8

                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_r_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_g_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_b_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_r_viz_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_g_viz_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_b_viz_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_r_knc_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_g_knc_addr = temp_val.value; addr_offset += 8
                client_mem.Read(add_addr + addr_offset, temp_val); ctx.client_glow_b_knc_addr = temp_val.value; # Last one, no addr_offset increment needed

                check_val = ctypes.c_uint32()
                if ctx.client_check_addr != 0 and client_mem.Read(ctx.client_check_addr, check_val):
                    if check_val.value == 0xABCD:
                        print("SetVars: Client check value 0xABCD confirmed. Initializing shared variables.")
                        ctx.client_vars_valid = True
                        # Signal client that memory is ready by writing 0 to check_addr
                        client_mem.Write(ctx.client_check_addr, ctypes.c_uint32(0))
                    else:
                        print(f"SetVars: Client check value mismatch. Expected 0xABCD, got {hex(check_val.value)}. Vars not valid.")
                        ctx.client_vars_valid = False
                else:
                    print("SetVars: Failed to read client check value or client_check_addr is null.")
                    ctx.client_vars_valid = False

                ctx.client_vars_initialized = True # Mark as initialized even if check failed, to prevent re-reading pointers.
                                                 # Validity is handled by client_vars_valid.
            except Exception as e:
                print(f"SetVars: Error during client address initialization: {e}")
                ctx.client_vars_initialized = True # Prevent retry loops on error
                ctx.client_vars_valid = False


        if ctx.client_vars_initialized and ctx.client_vars_valid and ctx.c_base != 0:
            try:
                # Write data to Client.exe
                if ctx.client_g_base_addr: client_mem.Write(ctx.client_g_base_addr, ctypes.c_uint64(ctx.g_base))
                if ctx.client_spectators_addr: client_mem.Write(ctx.client_spectators_addr, ctypes.c_int(ctx.spectators_count))
                if ctx.client_allied_spectators_addr: client_mem.Write(ctx.client_allied_spectators_addr, ctypes.c_int(ctx.allied_spectators_count))

                # ESP Data Synchronization
                # In C++, esp_next_update is set by main loop, read by ESP loop, then ESP sets valid and next.
                # Client reads valid and next, then client sets next to false.
                if ctx.esp_enabled and ctx.esp_next_update: # esp_next_update is True when main wants ESP to run
                    if ctx.esp_valid_data and ctx.client_player_addr and ctx.client_valid_addr and ctx.client_next_addr:
                        if ctx.esp_players_data: # Only write if list is not empty
                            player_array_type = structs.PlayerData * len(ctx.esp_players_data)
                            esp_data_ctypes = player_array_type()
                            for i, p_data in enumerate(ctx.esp_players_data):
                                esp_data_ctypes[i] = p_data
                            client_mem.WriteArray(ctx.client_player_addr, esp_data_ctypes)

                        client_mem.Write(ctx.client_valid_addr, ctypes.c_bool(True)) # Data is valid
                        client_mem.Write(ctx.client_next_addr, ctypes.c_bool(True))  # Signal client: new data available

                        # Wait for client to acknowledge receipt by setting next_addr to false
                        # This is a blocking wait, consider a timeout or non-blocking mechanism for robustness
                        wait_start_time = time.time()
                        client_ack = ctypes.c_bool(True)
                        while client_ack.value and (time.time() - wait_start_time) < 1.0: # Max 1 sec wait
                            if not client_mem.Read(ctx.client_next_addr, client_ack):
                                print("SetVars: Failed to read client_next_addr for ack. Aborting ESP sync.")
                                break
                            if not client_ack.value: # Client has acknowledged
                                break
                            time.sleep(0.001) # Brief sleep while polling

                        if client_ack.value:
                            print("SetVars: Timeout or error waiting for client ESP data acknowledgement.")

                    else: # esp_valid_data is false (e.g. in lobby) or addresses are null
                        if ctx.client_valid_addr: client_mem.Write(ctx.client_valid_addr, ctypes.c_bool(False))
                        if ctx.client_next_addr: client_mem.Write(ctx.client_next_addr, ctypes.c_bool(False)) # No new data or ensure client doesn't wait

                    ctx.esp_next_update = False # Reset flag, main loop will set it True when appropriate


                # Read data from Client.exe
                bool_val = ctypes.c_bool()
                int_val = ctypes.c_int()
                float_val = ctypes.c_float()

                if ctx.client_aim_addr and client_mem.Read(ctx.client_aim_addr, bool_val): ctx.aim_enabled = bool_val.value
                if ctx.client_esp_addr and client_mem.Read(ctx.client_esp_addr, bool_val): ctx.esp_enabled = bool_val.value
                if ctx.client_player_glow_addr and client_mem.Read(ctx.client_player_glow_addr, bool_val): ctx.player_glow_enabled = bool_val.value
                if ctx.client_item_glow_addr and client_mem.Read(ctx.client_item_glow_addr, bool_val): ctx.item_glow_enabled = bool_val.value
                if ctx.client_aim_key_addr and client_mem.Read(ctx.client_aim_key_addr, bool_val): ctx.aim_key_pressed = bool_val.value # Assuming aim_key is bool

                if ctx.client_bone_target_addr and client_mem.Read(ctx.client_bone_target_addr, int_val): ctx.bone_target = int_val.value
                if ctx.client_max_dist_addr and client_mem.Read(ctx.client_max_dist_addr, float_val): ctx.max_dist = float_val.value
                if ctx.client_max_fov_addr and client_mem.Read(ctx.client_max_fov_addr, float_val): ctx.max_fov = float_val.value
                if ctx.client_smooth_factor_addr and client_mem.Read(ctx.client_smooth_factor_addr, float_val): ctx.smooth_factor = float_val.value

                if ctx.client_firing_range_addr and client_mem.Read(ctx.client_firing_range_addr, bool_val): ctx.firing_range = bool_val.value
                if ctx.client_onevone_mode_addr and client_mem.Read(ctx.client_onevone_mode_addr, bool_val): ctx.onevone_mode = bool_val.value

                if ctx.client_glow_r_addr and client_mem.Read(ctx.client_glow_r_addr, float_val): ctx.glow_r = float_val.value
                if ctx.client_glow_g_addr and client_mem.Read(ctx.client_glow_g_addr, float_val): ctx.glow_g = float_val.value
                if ctx.client_glow_b_addr and client_mem.Read(ctx.client_glow_b_addr, float_val): ctx.glow_b = float_val.value
                if ctx.client_glow_r_viz_addr and client_mem.Read(ctx.client_glow_r_viz_addr, float_val): ctx.glow_r_viz = float_val.value
                if ctx.client_glow_g_viz_addr and client_mem.Read(ctx.client_glow_g_viz_addr, float_val): ctx.glow_g_viz = float_val.value
                if ctx.client_glow_b_viz_addr and client_mem.Read(ctx.client_glow_b_viz_addr, float_val): ctx.glow_b_viz = float_val.value
                if ctx.client_glow_r_knc_addr and client_mem.Read(ctx.client_glow_r_knc_addr, float_val): ctx.glow_r_knc = float_val.value
                if ctx.client_glow_g_knc_addr and client_mem.Read(ctx.client_glow_g_knc_addr, float_val): ctx.glow_g_knc = float_val.value
                if ctx.client_glow_b_knc_addr and client_mem.Read(ctx.client_glow_b_knc_addr, float_val): ctx.glow_b_knc = float_val.value

            except Exception as e:
                print(f"SetVars: Error during main sync: {e}")
                # Consider setting ctx.client_vars_valid = False here if errors persist, to trigger re-init or stop.
                # For now, just print and continue.

        time.sleep(0.001) # Loop interval for set_vars_loop, C++ uses 1ms. Can be higher.
    print("SetVars loop ended.")


if __name__ == "__main__":
    print("Starting PyApexDMA...")

    apex_mem = Memory()
    client_mem = Memory() # For Client.exe if used to set variables
    game_ctx = GameContext()

    threads = []

    try:
        while game_ctx.active:
            # --- Game Process Handling (r5apex_dx12.exe) ---
            if apex_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                print("Apex process not found or not ready. Attempting to open...")
                if game_ctx.g_base != 0: # Was previously running
                    print("Apex process lost. Signaling dependent threads to pause.")
                    game_ctx.g_base = 0 # Signal threads that game is not available
                    # Threads should handle g_base == 0 and pause their operations

                if apex_mem.open_proc("r5apex_dx12.exe"): # Target process name
                    print(f"Apex process opened successfully. PID: {apex_mem.get_proc_pid()}")
                    game_ctx.g_base = apex_mem.get_proc_baseaddr()
                    print(f"Apex base address: {hex(game_ctx.g_base)}")

                    # Start game-dependent threads if not already running or if they exited
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
                    print("Failed to open Apex process. Retrying in a moment...")
                    time.sleep(3) # Wait before retrying connection
            else: # Game process is ready
                apex_mem.check_proc() # Check if it's still running
                if apex_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                    print("Apex process check failed. Will attempt to reopen.")
                    game_ctx.g_base = 0 # Signal threads
                    continue # Loop back to try reopening

                # Signal ESP loop to perform an update
                if game_ctx.esp_enabled and not game_ctx.esp_next_update :
                     # Check map, signon state etc. to determine if ESP should run full logic
                    signon_state_val = ctypes.c_int()
                    level_name_raw = (ctypes.c_char * 64)() # Max length for level name

                    if apex_mem.Read(game_ctx.g_base + GAME_OFFSETS["OFFSET_SIGNONSTATE"], signon_state_val) and \
                       apex_mem.ReadArray(game_ctx.g_base + GAME_OFFSETS["OFFSET_LEVELNAME"], level_name_raw):

                        game_ctx.map_name = level_name_raw.value.decode('utf-8', 'ignore').strip()
                        game_ctx.is_in_game = (signon_state_val.value == 7) # SIGNONSTATE_FULL typically

                        if "mp_lobby" in game_ctx.map_name or not game_ctx.is_in_game:
                            game_ctx.firing_range = False # Assuming lobby is not firing range
                            if game_ctx.esp_valid_data : # Clear data if in lobby
                                game_ctx.esp_players_data.clear()
                                game_ctx.esp_valid_data = False
                        else:
                            # Could set firing_range based on map name, e.g. "mp_rr_canyonlands_staging"
                            game_ctx.firing_range = "mp_rr_canyonlands_staging" in game_ctx.map_name
                            game_ctx.esp_next_update = True # Allow ESP update if in a game map
                    else:
                        game_ctx.is_in_game = False # Failed to read critical info

            # --- Client Process Handling (Client.exe for settings) ---
            if client_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                # print("Client process not found. Attempting to open...") # Optional: can be noisy
                if game_ctx.c_base != 0:
                    # print("Client process lost. Signaling vars_thread to pause.")
                    game_ctx.c_base = 0

                if client_mem.open_proc("Client.exe"): # Name of the external settings client
                    # print(f"Client process opened. PID: {client_mem.get_proc_pid()}")
                    game_ctx.c_base = client_mem.get_proc_baseaddr()
                    # print(f"Client base address: {hex(game_ctx.c_base)}")

                    if game_ctx.vars_thread_handle is None or not game_ctx.vars_thread_handle.is_alive():
                        game_ctx.vars_thread_handle = threading.Thread(target=set_vars_loop, args=(game_ctx, client_mem), daemon=True)
                        threads.append(game_ctx.vars_thread_handle)
                        game_ctx.vars_thread_handle.start()
                # else:
                    # print("Client.exe not found. Settings will use defaults.")
                    # No need to sleep heavily here, as it's not critical for core game interaction
                    pass
            else: # Client process is ready
                client_mem.check_proc()
                if client_mem.get_proc_status() != ProcessStatus.FOUND_READY:
                    # print("Client process check failed.")
                    game_ctx.c_base = 0

            time.sleep(game_ctx.main_sleep) # Main loop polling rate

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
