import dearpygui.dearpygui as dpg
import os
from .client_settings import ClientSettings
from .client_ipc import IPCClient, SERVER_HOST, SERVER_PORT # IPC imports

SETTINGS_FILE = "py_client_settings.json"
settings = ClientSettings()
ipc_handler: IPCClient = None # Will be initialized later

# Global placeholder for ESP data received from server (for rendering)
esp_data_for_rendering = []
server_status_message = "N/A"


# --- IPC Data Handler ---
def handle_server_data_received(data: dict):
    global esp_data_for_rendering, server_status_message # server_status_message might be redundant if status comes in game_state
    # print(f"DPG Client received data: {data}")

    if data.get("type") == "game_state":
        payload = data.get("payload", {})

        status = payload.get("status", "N/A")
        if dpg.does_item_exist("##server_status_text"):
            dpg.set_value("##server_status_text", status)

        map_name = payload.get("map_name", "N/A")
        if dpg.does_item_exist("##map_name_text"):
            dpg.set_value("##map_name_text", f"Map: {map_name}")

        spectators_data = payload.get("spectators", {}) # Expects a dict like {"enemy": X, "allied": Y}
        enemy_specs = spectators_data.get("enemy", 0)
        allied_specs = spectators_data.get("allied", 0)
        if dpg.does_item_exist("##spectator_count_text"):
            dpg.set_value("##spectator_count_text", f"Spectators: E:{enemy_specs} / A:{allied_specs}")

        esp_players = payload.get("esp_players", [])
        esp_data_for_rendering = esp_players # Store for ESP draw loop
        if dpg.does_item_exist("##esp_player_count_text"):
            dpg.set_value("##esp_player_count_text", f"ESP Players: {len(esp_players)}")

        # print(f"Client received ESP players: {esp_players}") # For console verification

    # elif data.get("type") == "status_update": # If server sends separate status updates
    #     server_status_message = data.get("message", "Status update received")
    #     if dpg.does_item_exist("##server_status_text"):
    #         dpg.set_value("##server_status_text", server_status_message)
    #     if "spectators" in data and dpg.does_item_exist("##spectator_count_text"): # Example if status_update carries spectator info
    #         dpg.set_value("##spectator_count_text", f"Spectators: {data['spectators']}")


# --- Callback Functions ---
def bool_setting_callback(sender, app_data, user_data_key):
    setattr(settings, user_data_key, app_data)
    if ipc_handler: ipc_handler.send_settings()

def float_setting_callback(sender, app_data, user_data_key):
    setattr(settings, user_data_key, app_data)
    if ipc_handler: ipc_handler.send_settings()
    # Update associated text display if it exists
    text_display_tag = f"{sender}_text"
    if dpg.does_item_exist(text_display_tag):
        dpg.set_value(text_display_tag, f"{app_data:.2f}")


def int_setting_callback(sender, app_data, user_data_key):
    setattr(settings, user_data_key, app_data)
    if ipc_handler: ipc_handler.send_settings()
    text_display_tag = f"{sender}_text"
    if dpg.does_item_exist(text_display_tag):
        dpg.set_value(text_display_tag, f"{app_data}")

def visuals_bool_callback(sender, app_data, user_data_key):
    settings.visuals[user_data_key] = app_data
    if ipc_handler: ipc_handler.send_settings()

def visuals_float_callback(sender, app_data, user_data_key):
    settings.visuals[user_data_key] = app_data
    if ipc_handler: ipc_handler.send_settings()
    text_display_tag = f"{sender}_text"
    if dpg.does_item_exist(text_display_tag):
        dpg.set_value(text_display_tag, f"{app_data:.2f}")

def color_setting_callback(sender, app_data, user_data_key):
    # app_data is usually a list of 3 or 4 floats (0-1 range)
    setattr(settings, user_data_key, [app_data[0], app_data[1], app_data[2], app_data[3] if len(app_data) > 3 else 1.0])
    if ipc_handler: ipc_handler.send_settings()

def save_config_callback():
    settings.save_to_file(SETTINGS_FILE)
    if ipc_handler: ipc_handler.send_settings() # Send settings after saving too, if server needs to know
    print("Config saved.")

def load_config_callback():
    settings.load_from_file(SETTINGS_FILE)
    update_ui_from_settings()
    if ipc_handler: ipc_handler.send_settings() # Send newly loaded settings to server
    print("Config loaded and UI updated.")

def update_ui_from_settings():
    print("Updating UI from settings...")
    # Main Tab
    dpg.set_value("##esp_enabled", settings.esp_enabled)
    dpg.set_value("##aim_enabled", settings.aim_enabled)
    dpg.set_value("##aim_visibility_check", settings.aim_visibility_check)
    dpg.set_value("##aim_no_recoil", settings.aim_no_recoil)
    dpg.set_value("##player_glow_enabled", settings.player_glow_enabled)
    dpg.set_value("##dds_enabled", settings.dds_enabled)

    # Config Tab
    dpg.set_value("##max_dist", settings.max_dist)
    dpg.set_value("##max_dist_text", f"{settings.max_dist:.2f}")
    dpg.set_value("##smooth_aim_value", settings.smooth_aim_value)
    dpg.set_value("##smooth_aim_value_text", f"{settings.smooth_aim_value:.2f}")
    dpg.set_value("##max_fov", settings.max_fov)
    dpg.set_value("##max_fov_text", f"{settings.max_fov:.2f}")
    dpg.set_value("##aim_bone", settings.aim_bone)
    dpg.set_value("##aim_bone_text", f"{settings.aim_bone}")

    dpg.set_value("##dds_distance", settings.dds_distance)
    dpg.set_value("##dds_distance_text", f"{settings.dds_distance:.2f}")
    dpg.set_value("##dds_min_fov", settings.dds_min_fov)
    dpg.set_value("##dds_min_fov_text", f"{settings.dds_min_fov:.2f}")
    dpg.set_value("##dds_max_fov", settings.dds_max_fov)
    dpg.set_value("##dds_max_fov_text", f"{settings.dds_max_fov:.2f}")
    dpg.set_value("##dds_min_smooth", settings.dds_min_smooth)
    dpg.set_value("##dds_min_smooth_text", f"{settings.dds_min_smooth:.2f}")
    dpg.set_value("##dds_max_smooth", settings.dds_max_smooth)
    dpg.set_value("##dds_max_smooth_text", f"{settings.dds_max_smooth:.2f}")

    # Visuals Tab
    dpg.set_value("##box_esp", settings.visuals["box_esp"])
    dpg.set_value("##name_esp", settings.visuals["name_esp"])
    dpg.set_value("##circle_fov_enabled", settings.visuals["circle_fov_enabled"])
    dpg.set_value("##line_esp", settings.visuals["line_esp"])
    dpg.set_value("##distance_esp", settings.visuals["distance_esp"])
    dpg.set_value("##health_bar_esp", settings.visuals["health_bar_esp"])
    dpg.set_value("##shield_bar_esp", settings.visuals["shield_bar_esp"])

    dpg.set_value("##circle_fov_size", settings.visuals["circle_fov_size"])
    dpg.set_value("##circle_fov_size_text", f"{settings.visuals['circle_fov_size']:.2f}")

    dpg.set_value("##glow_visible_color", settings.glow_visible_ui)
    dpg.set_value("##glow_not_visible_color", settings.glow_not_visible_ui)
    dpg.set_value("##glow_knocked_color", settings.glow_knocked_ui)


# --- Main UI Setup ---
def setup_ui():
    dpg.create_context()

    with dpg.window(label="Py Apex Guest Controls", tag="Primary Window"):
        with dpg.tab_bar():
            # --- Main Tab ---
            with dpg.tab(label="Main"):
                dpg.add_checkbox(label="ESP Enabled", tag="##esp_enabled", default_value=settings.esp_enabled, callback=bool_setting_callback, user_data="esp_enabled")
                dpg.add_checkbox(label="Aim Enabled", tag="##aim_enabled", default_value=settings.aim_enabled, callback=bool_setting_callback, user_data="aim_enabled")
                dpg.add_checkbox(label="Aim Visibility Check", tag="##aim_visibility_check", default_value=settings.aim_visibility_check, callback=bool_setting_callback, user_data="aim_visibility_check")
                dpg.add_checkbox(label="Aim No Recoil (Placeholder)", tag="##aim_no_recoil", default_value=settings.aim_no_recoil, callback=bool_setting_callback, user_data="aim_no_recoil")
                dpg.add_checkbox(label="Player Glow Enabled", tag="##player_glow_enabled", default_value=settings.player_glow_enabled, callback=bool_setting_callback, user_data="player_glow_enabled")
                dpg.add_separator()
                dpg.add_checkbox(label="DDS Enabled (Dynamic Distance Smoothing/FOV)", tag="##dds_enabled", default_value=settings.dds_enabled, callback=bool_setting_callback, user_data="dds_enabled")


            # --- Config Tab ---
            with dpg.tab(label="Config"):
                dpg.add_slider_float(label="Max Distance", tag="##max_dist", default_value=settings.max_dist, max_value=10000.0, callback=float_setting_callback, user_data="max_dist")
                dpg.add_text(f"{settings.max_dist:.2f}", tag="##max_dist_text")
                dpg.add_slider_float(label="Smooth Aim Value", tag="##smooth_aim_value", default_value=settings.smooth_aim_value, max_value=1.0, callback=float_setting_callback, user_data="smooth_aim_value")
                dpg.add_text(f"{settings.smooth_aim_value:.2f}", tag="##smooth_aim_value_text")
                dpg.add_slider_float(label="Max FOV", tag="##max_fov", default_value=settings.max_fov, max_value=90.0, callback=float_setting_callback, user_data="max_fov")
                dpg.add_text(f"{settings.max_fov:.2f}", tag="##max_fov_text")
                dpg.add_slider_int(label="Aim Bone", tag="##aim_bone", default_value=settings.aim_bone, max_value=5, callback=int_setting_callback, user_data="aim_bone")
                dpg.add_text(f"{settings.aim_bone}", tag="##aim_bone_text")
                dpg.add_separator()
                dpg.add_text("DDS Settings (if DDS Enabled)")
                dpg.add_slider_float(label="DDS Max Distance", tag="##dds_distance", default_value=settings.dds_distance, max_value=10000.0, callback=float_setting_callback, user_data="dds_distance")
                dpg.add_text(f"{settings.dds_distance:.2f}", tag="##dds_distance_text")
                dpg.add_slider_float(label="DDS Min FOV", tag="##dds_min_fov", default_value=settings.dds_min_fov, max_value=90.0, callback=float_setting_callback, user_data="dds_min_fov")
                dpg.add_text(f"{settings.dds_min_fov:.2f}", tag="##dds_min_fov_text")
                dpg.add_slider_float(label="DDS Max FOV", tag="##dds_max_fov", default_value=settings.dds_max_fov, max_value=90.0, callback=float_setting_callback, user_data="dds_max_fov")
                dpg.add_text(f"{settings.dds_max_fov:.2f}", tag="##dds_max_fov_text")
                dpg.add_slider_float(label="DDS Min Smooth", tag="##dds_min_smooth", default_value=settings.dds_min_smooth, max_value=1.0, callback=float_setting_callback, user_data="dds_min_smooth")
                dpg.add_text(f"{settings.dds_min_smooth:.2f}", tag="##dds_min_smooth_text")
                dpg.add_slider_float(label="DDS Max Smooth", tag="##dds_max_smooth", default_value=settings.dds_max_smooth, max_value=1.0, callback=float_setting_callback, user_data="dds_max_smooth")
                dpg.add_text(f"{settings.dds_max_smooth:.2f}", tag="##dds_max_smooth_text")


            # --- Visuals Tab ---
            with dpg.tab(label="Visuals"):
                dpg.add_checkbox(label="Box ESP", tag="##box_esp", default_value=settings.visuals["box_esp"], callback=visuals_bool_callback, user_data="box_esp")
                dpg.add_checkbox(label="Name ESP", tag="##name_esp", default_value=settings.visuals["name_esp"], callback=visuals_bool_callback, user_data="name_esp")
                dpg.add_checkbox(label="FOV Circle", tag="##circle_fov_enabled", default_value=settings.visuals["circle_fov_enabled"], callback=visuals_bool_callback, user_data="circle_fov_enabled")
                dpg.add_slider_float(label="FOV Circle Size", tag="##circle_fov_size", default_value=settings.visuals["circle_fov_size"], max_value=90.0, callback=visuals_float_callback, user_data="circle_fov_size")
                dpg.add_text(f"{settings.visuals['circle_fov_size']:.2f}", tag="##circle_fov_size_text")
                dpg.add_checkbox(label="Line ESP", tag="##line_esp", default_value=settings.visuals["line_esp"], callback=visuals_bool_callback, user_data="line_esp")
                dpg.add_checkbox(label="Distance ESP", tag="##distance_esp", default_value=settings.visuals["distance_esp"], callback=visuals_bool_callback, user_data="distance_esp")
                dpg.add_checkbox(label="Health Bar ESP", tag="##health_bar_esp", default_value=settings.visuals["health_bar_esp"], callback=visuals_bool_callback, user_data="health_bar_esp")
                dpg.add_checkbox(label="Shield Bar ESP", tag="##shield_bar_esp", default_value=settings.visuals["shield_bar_esp"], callback=visuals_bool_callback, user_data="shield_bar_esp")
                dpg.add_separator()
                dpg.add_text("Glow Colors (UI - 0 to 1 scale)")
                dpg.add_color_picker(label="Visible Glow", tag="##glow_visible_color", default_value=settings.glow_visible_ui, callback=color_setting_callback, user_data="glow_visible_ui", no_alpha=False)
                dpg.add_color_picker(label="Not Visible Glow", tag="##glow_not_visible_color", default_value=settings.glow_not_visible_ui, callback=color_setting_callback, user_data="glow_not_visible_ui", no_alpha=False)
                dpg.add_color_picker(label="Knocked Glow", tag="##glow_knocked_color", default_value=settings.glow_knocked_ui, callback=color_setting_callback, user_data="glow_knocked_ui", no_alpha=False)
                dpg.add_separator()
                dpg.add_button(label="Save Config", callback=save_config_callback)
                dpg.add_button(label="Load Config", callback=load_config_callback)

        # Placeholder for Info Display
        # Ensure this window is created before attempting to set values to its children
        # Define height to fit all text lines.
        info_window_height = 120
        with dpg.window(label="Info Display", tag="info_window", no_move=True, no_close=True, no_collapse=True, width=250, height=info_window_height):
            # Position this window dynamically if needed, or set a fixed pos.
            # dpg.set_item_pos("info_window", [0, dpg.get_viewport_height() - info_window_height - 40]) # Example dynamic positioning

            dpg.add_text("IPC: Disconnected", tag="##connection_status_text")
            dpg.add_text("Server: N/A", tag="##server_status_text")
            dpg.add_text("Map: N/A", tag="##map_name_text")
            dpg.add_text("Spectators: E:0 / A:0", tag="##spectator_count_text")
            dpg.add_text("ESP Players: 0", tag="##esp_player_count_text")

    # Placeholder for ESP Display (Overlay)
    # This requires more setup for transparency and click-through, typically done at viewport level
    # For now, a simple drawlist in a window.
    with dpg.window(label="ESP Overlay", tag="##esp_overlay_window", show=True, no_title_bar=True, no_resize=True, no_move=True, no_scrollbar=True, no_collapse=True, no_close=True):
        # Make it full screen and always on top (specific DPG viewport/window flags needed)
        # dpg.set_primary_window("##esp_overlay_window", True) # Example
        with dpg.drawlist(width=dpg.get_viewport_width(), height=dpg.get_viewport_height(), tag="##esp_drawlist"):
            dpg.draw_text((10, 10), "ESP Active (Test Text)", color=(255, 0, 0, 255), size=20)
            # Actual ESP drawings (boxes, lines, names) would be added here dynamically.

    dpg.create_viewport(title='Py Apex Guest', width=700, height=600)
    dpg.setup_dearpygui()
    dpg.show_viewport()

    # Load settings after UI is created and shown
    if os.path.exists(SETTINGS_FILE):
        settings.load_from_file(SETTINGS_FILE)
        update_ui_from_settings()
    else:
        print(f"{SETTINGS_FILE} not found, using default settings for UI.")
        # UI already reflects defaults from ClientSettings instantiation and DPG widget defaults.

    # Set primary window after setup
    dpg.set_primary_window("Primary Window", True)

    # Adjust ESP overlay window to be full viewport size
    # This is tricky; viewport size isn't fixed until render. Best effort here.
    # Ideally, make it a child of viewport or use specific overlay flags if DPG supports them well.

    # Attempt to set after first render or on viewport resize for better accuracy
    # dpg.set_item_width("##esp_overlay_window", dpg.get_viewport_width()) # Best called after viewport is shown
    # dpg.set_item_height("##esp_overlay_window", dpg.get_viewport_height())
    dpg.set_item_pos("##esp_overlay_window", [0,0])

    # Set main window resize callback to adjust overlay size
    # def on_viewport_resize(sender, app_data):
    #     viewport_width = app_data[0]
    #     viewport_height = app_data[1]
    #     if dpg.does_item_exist("##esp_overlay_window"):
    #         dpg.set_item_width("##esp_overlay_window", viewport_width)
    #         dpg.set_item_height("##esp_overlay_window", viewport_height)
    #     if dpg.does_item_exist("info_window"): # Adjust info window pos
    #         dpg.set_item_pos("info_window", [0, viewport_height - info_window_height - 10]) # 10 for padding from bottom

    # dpg.set_viewport_resize_callback(on_viewport_resize) - This can cause issues if not careful with init order.


def on_dpg_exit():
    print("DPG is attempting to close.")
    if ipc_handler:
        print("Stopping IPC handler...")
        ipc_handler.stop()
    # dpg.destroy_context() # This will be called by DPG's main loop exit after this handler

def start_client():
    global ipc_handler
    ipc_handler = IPCClient(SERVER_HOST, SERVER_PORT, settings, handle_server_data_received)

    setup_ui() # This creates the context and viewport

    # Start IPC client after UI is set up and shown
    ipc_handler.start()

    # Configure viewport close callback
    dpg.set_exit_callback(on_dpg_exit)

    # Position info window after viewport is shown and has dimensions
    # This is a bit of a hack; DPG layout can be tricky for fixed overlay-like elements.
    # A more robust way might involve using a viewport resize callback or specific DPG container flags.
    try:
        viewport_h = dpg.get_viewport_height()
        info_window_h = dpg.get_item_height("info_window") if dpg.does_item_exist("info_window") else 120
        if dpg.does_item_exist("info_window"):
            dpg.set_item_pos("info_window", [5, viewport_h - info_window_h - 35]) # Small offset from bottom-left
    except Exception as e:
        print(f"Error positioning info_window: {e}")


    frame_count = 0
    while dpg.is_dearpygui_running():
        # Update dynamic UI elements here
        if ipc_handler and dpg.does_item_exist("##connection_status_text"):
             if frame_count % 30 == 0: # Update status roughly every half-second
                dpg.set_value("##connection_status_text", "IPC: Connected" if ipc_handler.is_client_connected() else "IPC: Disconnected")

        # Update ESP overlay (example)
        if dpg.does_item_exist("##esp_drawlist") and esp_data_for_rendering:
            # Clear previous drawings (or manage them if objects are tagged)
            # dpg.clear_drawlist("##esp_drawlist") # If not using layers or tags
            # This is a naive clear, better to manage draw items.
            # For simplicity, let's assume for now we just add and it overlays.
            # A proper ESP would clear and redraw based on esp_data_for_rendering.
            pass # Actual drawing logic would go here based on esp_data_for_rendering

        dpg.render_dearpygui_frame()
        frame_count += 1

    # DPG loop has exited
    # on_dpg_exit should have been called if viewport was closed by user.
    # If loop exited due to other reasons, ensure ipc_handler is stopped.
    if ipc_handler and ipc_handler.is_running:
        print("DPG loop exited, ensuring IPC handler is stopped.")
        ipc_handler.stop()

    dpg.destroy_context() # Clean up DPG

if __name__ == "__main__":
    start_client()

```
