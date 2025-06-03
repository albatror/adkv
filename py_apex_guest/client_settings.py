import json
import os

class ClientSettings:
    def __init__(self):
        # Booleans
        self.esp_enabled: bool = True
        self.aim_enabled: bool = True
        self.aim_visibility_check: bool = True
        self.aim_no_recoil: bool = True # Placeholder, actual no-recoil is complex
        self.player_glow_enabled: bool = True

        # Floats
        self.max_dist: float = 200.0 * 40.0 # Default from previous context, adjust as needed
        self.smooth_aim_value: float = 0.2
        self.max_fov: float = 15.0

        # DDS (Dynamic Distance Smoothing/FOV) - from config.h
        self.dds_enabled: bool = False # Added this toggle as it's common
        self.dds_distance: float = 100.0 * 40.0 # Max distance for DDS effect
        self.dds_min_fov: float = 5.0      # Min FOV when target is at/beyond dds_distance
        self.dds_max_fov: float = 20.0     # Max FOV when target is very close
        self.dds_min_smooth: float = 0.1   # Min smooth when target is at/beyond dds_distance
        self.dds_max_smooth: float = 0.3   # Max smooth when target is very close

        # Integers
        self.aim_bone: int = 0 # 0: Head, 1: Neck, 2: Chest (example mapping)

        # Visuals (nested dictionary for organization)
        self.visuals: dict = {
            "box_esp": True,
            "name_esp": True,
            "circle_fov_enabled": True,
            "line_esp": True,
            "distance_esp": True,
            "health_bar_esp": True,
            "shield_bar_esp": True,
            "circle_fov_size": self.max_fov # Default FOV circle to max_fov
        }

        # Glow Colors (dictionaries with 'r', 'g', 'b' from 0.0 to 1.0)
        # These should match the DPG color picker format (list/tuple of 3 or 4 floats)
        # DPG uses 0-1 float for colors.
        self.glow_visible: list[float] = [10.0/255.0, 0.0/255.0, 0.0/255.0, 1.0] # Red, with alpha
        self.glow_not_visible: list[float] = [0.0/255.0, 10.0/255.0, 0.0/255.0, 1.0] # Green, with alpha
        self.glow_knocked: list[float] = [0.0/255.0, 0.0/255.0, 10.0/255.0, 1.0] # Blue, with alpha

        # For DPG color pickers, it's often easier if they are [R, G, B] or [R, G, B, A] lists/tuples.
        # The provided C++ code uses individual floats (0-10, or 0-255 then divided).
        # Let's adapt to DPG's typical 0-1 float scale.
        # Example: GLOW_R_VIZ = 10.f; -> 10.0/255.0 if it was 0-255 based, or just ensure it's scaled if it was 0-10.
        # The C++ code seems to use arbitrary float values (like 10.f), not strictly 0-1 for shaders.
        # For DPG UI, we should probably keep them in 0-1 range for color pickers.
        # Let's assume the C++ values (0.f, 10.f) were for a specific shader system.
        # For UI, we'll use a 0-1 scale.
        self.glow_visible_ui: list[float] = [1.0, 0.0, 0.0, 1.0]  # Bright Red
        self.glow_not_visible_ui: list[float] = [0.0, 1.0, 0.0, 1.0]  # Bright Green
        self.glow_knocked_ui: list[float] = [0.0, 0.0, 1.0, 1.0]  # Bright Blue

    def to_dict(self) -> dict: # Renamed from _to_dict
        """Converts settings to a dictionary, handling nested visuals."""
        data = self.__dict__.copy()
        # No special handling needed if visuals is already a dict and colors are lists.
        return data

    def _from_dict(self, data: dict):
        """Updates settings from a dictionary."""
        for key, value in data.items():
            if hasattr(self, key):
                # Basic type checking or conversion could be added here if needed
                # For example, ensuring colors are lists/tuples of correct length
                if key.startswith("glow_") and key.endswith("_ui"): # ensure color lists are mutable for dpg
                    setattr(self, key, list(value))
                elif key == "visuals" and isinstance(value, dict): # ensure visuals dict is updated correctly
                    if isinstance(self.visuals, dict):
                        self.visuals.update(value)
                    else:
                        setattr(self, key, value) # fallback if self.visuals wasn't a dict
                else:
                    setattr(self, key, value)

    def save_to_file(self, filepath: str):
        """Serializes settings to a JSON file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(self._to_dict(), f, indent=4)
            print(f"Settings saved to {filepath}")
        except IOError as e:
            print(f"Error saving settings to {filepath}: {e}")

    def load_from_file(self, filepath: str):
        """Loads settings from a JSON file, updating instance attributes."""
        if not os.path.exists(filepath):
            print(f"Settings file {filepath} not found. Using default settings.")
            return

        try:
            with open(filepath, 'r') as f:
                data = json.load(f)
            self._from_dict(data)
            print(f"Settings loaded from {filepath}")
        except (IOError, json.JSONDecodeError) as e:
            print(f"Error loading settings from {filepath}: {e}. Using default settings.")

if __name__ == '__main__':
    settings = ClientSettings()

    # Test save
    test_save_path = "test_settings.json"
    settings.visuals["box_esp"] = False # Change a value
    settings.max_fov = 25.0
    settings.save_to_file(test_save_path)

    # Test load into a new instance
    new_settings = ClientSettings()
    # Verify it has defaults first
    print(f"New settings default box_esp: {new_settings.visuals['box_esp']}")
    print(f"New settings default max_fov: {new_settings.max_fov}")

    new_settings.load_from_file(test_save_path)
    print(f"Loaded box_esp: {new_settings.visuals['box_esp']}") # Should be False
    print(f"Loaded max_fov: {new_settings.max_fov}") # Should be 25.0

    # Clean up test file
    if os.path.exists(test_save_path):
        os.remove(test_save_path)
    print("Test finished.")

```
