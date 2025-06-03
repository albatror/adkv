import ctypes
import math
from .memory import Memory
# from .offsets import GAME_OFFSETS # GAME_OFFSETS will be passed as an argument where needed
from . import structs

# Global Glow Settings (mirrored from apex_dma.cpp defaults)
PLAYER_GLOW_ENABLED = True
GLOW_R = 0.0  # Non-Visible RED
GLOW_G = 10.0 # Non-Visible GREEN
GLOW_B = 0.0  # Non-Visible BLUE
GLOW_R_VIZ = 10.0 # Visible RED
GLOW_G_VIZ = 0.0 # Visible GREEN
GLOW_B_VIZ = 0.0 # Visible BLUE
GLOW_R_KNC = 5.0  # Knocked RED
GLOW_G_KNC = 0.0  # Knocked GREEN
GLOW_B_KNC = 10.0 # Knocked BLUE

# Glow general (not directly used in set_player_glow, but good to have)
GLOW_MIN_DIST = 2000.0 # Min Dist for glow
GLOW_MAX_DIST = 5000.0 # Max Dist for glow

# Glow Fixed Values - From C++ comments, these are important for enable_glow logic later
# These might be part of what 'setting_index' or 'context_id' implies, or direct writes.
# For now, defining them here for reference. The actual enable_glow will need careful mapping.
GLOW_FADE_MAX_TIME = 10.0  # Max fade time (seconds)
GLOW_FADE_START_TIME = 5000.0  # Start time for fade # C++ GLOW_FADE_START_TIME is actually OFFSET_VISIBLE_TIME related
GLOW_FADE_END_TIME = 5000.0    # End time for fade   # C++ GLOW_FADE_END_TIME is also OFFSET_VISIBLE_TIME related
GLOW_VISIBLE_TYPE = 1          # Visible type for some glow modes
GLOW_LIFE_TIME = 1             # Lifetime for some glow modes
GLOW_FIX = 35                  # Fix value used in C++ for some glow settings
GLOW_CONTEXT_ID_KNOCKED = 44   # Knocked context ID
GLOW_CONTEXT_ID_VISIBLE = 5    # Visible context ID
GLOW_CONTEXT_ID_INVISIBLE = 1  # Invisible context ID
GLOW_BORDER_SIZE = 40          # Border size for glow
GLOW_BORDER_MODE = 0           # Border mode
GLOW_FULL_BODY_MODE = 0        # Full body mode


class Entity:
    def __init__(self, ptr: int, mem: Memory, g_base: int = 0):
        self.ptr = ptr
        self.mem = mem
        self.g_base = g_base
        # self.buffer = (ctypes.c_uint8 * 0x3FF0)() # Optional: for buffered reads

    def update_buffer(self):
        """
        Placeholder for potentially reading the entire entity buffer.
        Currently not implemented as methods read directly.
        """
        # if self.ptr and self.mem:
        #     self.mem.ReadArray(self.ptr, self.buffer)
        pass

    def get_ptr(self) -> int:
        return self.ptr

    def get_health(self) -> int:
        health = ctypes.c_int()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_HEALTH"], health):
            return health.value
        return 0

    def get_max_health(self) -> int:
        max_health = ctypes.c_int()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_MAXHEALTH"], max_health):
            return max_health.value
        return 0

    def get_shield(self) -> int:
        shield = ctypes.c_int()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_SHIELD"], shield):
            return shield.value
        return 0

    def get_max_shield(self) -> int:
        max_shield = ctypes.c_int()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_MAXSHIELD"], max_shield):
            return max_shield.value
        return 0

    def get_armor_type(self) -> int:
        armor_type = ctypes.c_int()
        # Assuming OFFSET_ARMORTYPE is correct and directly readable as int
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_ARMORTYPE"], armor_type):
            return armor_type.value
        return 0

    def get_team_id(self) -> int:
        team_id = ctypes.c_int()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_TEAM"], team_id):
            return team_id.value
        return 0 # Or appropriate default/error value

    def get_position(self) -> structs.Vector:
        position = structs.Vector()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_ORIGIN"], position):
            return position
        return structs.Vector(0, 0, 0) # Return a zero vector on failure

    def get_abs_velocity(self) -> structs.Vector:
        abs_velocity = structs.Vector()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_ABS_VELOCITY"], abs_velocity):
            return abs_velocity
        return structs.Vector(0, 0, 0)

    def get_view_angles(self) -> structs.QAngle:
        view_angles = structs.QAngle()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_VIEWANGLES"], view_angles):
            return view_angles
        return structs.QAngle(0, 0, 0)

    def get_yaw(self) -> float:
        yaw = ctypes.c_float()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_YAW"], yaw):
            return yaw.value
        return 0.0

    def is_alive(self) -> bool:
        life_state = ctypes.c_int() # Assuming OFFSET_LIFE_STATE points to an integer
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_LIFE_STATE"], life_state):
            return life_state.value == 0
        return False

    def is_knocked(self) -> bool:
        bleed_out_state = ctypes.c_int() # Assuming OFFSET_BLEED_OUT_STATE points to an integer
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_BLEED_OUT_STATE"], bleed_out_state):
            return bleed_out_state.value != 0
        return False

    def is_zooming(self) -> bool:
        # OFFSET_ZOOMING is often a byte (bool)
        zooming_flag = ctypes.c_bool() # or c_uint8
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_ZOOMING"], zooming_flag):
            return zooming_flag.value
        return False

    def last_visible_time(self) -> float:
        visible_time = ctypes.c_float()
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_VISIBLE_TIME"], visible_time):
            return visible_time.value
        return 0.0

    def get_name_raw_ptr(self) -> int:
        name_ptr = ctypes.c_uint64() # Assuming pointer is 64-bit
        if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_SIGN_NAME"], name_ptr):
            return name_ptr.value
        return 0

    def read_name_string(self, name_ptr: int, length: int = 32) -> str:
        if not name_ptr:
            return ""
        name_buffer = (ctypes.c_char * length)()
        if self.mem.ReadArray(name_ptr, name_buffer):
            try:
                return name_buffer.value.decode('utf-8', 'ignore')
            except Exception as e:
                # print(f"Error decoding name string: {e}")
                return ""
        return ""

    def get_name_from_sign_name(self) -> str:
        name_ptr = self.get_name_raw_ptr()
        if name_ptr:
            return self.read_name_string(name_ptr)
        return "NameError_SignName"

    def get_name_from_name_list(self, entity_index: int) -> str:
        # This is a simplified placeholder. Real implementation is more complex.
        # It would involve reading from g_base + OFFSET_NAME_LIST + (entity_index * some_stride)
        # and then potentially another pointer dereference.
        # For now, as requested:
        # name_list_base_ptr_addr = self.g_base + GAME_OFFSETS["OFFSET_NAME_LIST"]
        # name_list_base_ptr = ctypes.c_uint64()
        # if self.mem.Read(name_list_base_ptr_addr, name_list_base_ptr):
        #    name_ptr_addr = name_list_base_ptr.value + (entity_index * POINTER_SIZE_OR_STRUCT_STRIDE)
        #    actual_name_ptr = ctypes.c_uint64()
        #    if self.mem.Read(name_ptr_addr, actual_name_ptr):
        #        return self.read_name_string(actual_name_ptr.value, 32)
        return "NameFromList_Placeholder"

    def enable_glow(self):
        # Placeholder: Actual glow involves multiple writes to different offsets
        # Example: Writing to OFFSET_GLOW_ENABLE, OFFSET_GLOW_CONTEXT_ID, GLOW_COLOR etc.
        # For now, just a conceptual placeholder.
        # Example (simplified, actual glow is more complex):
        # self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_ENABLE"], ctypes.c_int(1))
        # self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_THROUGH_WALLS"], ctypes.c_int(2))
        print(f"Entity.enable_glow() called for ptr {hex(self.ptr)} - Placeholder")
        pass

    def disable_glow(self):
        # Placeholder:
        # self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_ENABLE"], ctypes.c_int(0))
        # print(f"Entity.disable_glow() called for ptr {hex(self.ptr)} - Placeholder")
        if self.ptr and self.mem:
            self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_ENABLE"], ctypes.c_byte(0))
            # May also need to clear other glow settings like context ID if they persist
            # self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_CONTEXT_ID"], ctypes.c_int(0))
        pass

    def is_glowing(self) -> bool:
        glow_enabled_val = ctypes.c_byte()
        if self.ptr and self.mem and self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_GLOW_ENABLE"], glow_enabled_val):
            # Common values for glow enabled are 1 or sometimes other small integers like 7.
            # The C++ code often checks `if (isEnabled > 0 && isEnabled < 60)`
            # For simplicity, we'll check if it's non-zero, specifically 1 as per typical usage.
            # Or if specific values like from HighlightFlowSettings::SetHighlightActive are known, use them.
            # C++ example seems to use `write<unsigned char>(target.ptr + OFFSET_GLOW_ENABLE, 1);` for enabling.
            return glow_enabled_val.value == 1 # Or > 0 if other states also mean "glowing"
        return False

    def enable_glow(self, context_id: int, setting_index: int, color: tuple[float, float, float]):
        """
        Placeholder for enabling glow. Actual implementation is complex.
        Based on C++ source, it involves writing to multiple offsets.
        Args:
            context_id: Controls style (e.g., knocked, visible, invisible). Mapped to GLOW_CONTEXT_ID_*
            setting_index: Controls how glow is rendered (e.g., GLOW_VISIBLE_TYPE).
                           In C++, this is 'type' for OFFSET_GLOW_TYPE.
            color: (r, g, b) float tuple for glow color.
        """
        if not (self.ptr and self.mem):
            return

        print(f"Entity.enable_glow(ptr={hex(self.ptr)}, context_id={context_id}, setting_index={setting_index}, color={color}) - Placeholder")

        # Simplified placeholder actual writes (order and exact values/types need C++ source cross-reference):
        # 1. Enable general glow
        self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_ENABLE"], ctypes.c_byte(1)) # General enable flag

        # 2. Set glow context ID (determines style of glow based on visibility/state)
        self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_CONTEXT_ID"], ctypes.c_int(context_id))

        # 3. Set glow type/visibility (e.g., through walls)
        # This is often 'OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE' or 'OFFSET_GLOW_TYPE'
        # The C++ code uses `OFFSET_GLOW_TYPE` for `settingIndex` (called `타입` there)
        # And `OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE` is set to `GLOW_VISIBLE_TYPE` (constant 1)
        self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_TYPE"], ctypes.c_int(setting_index))
        self.mem.Write(self.ptr + GAME_OFFSETS["OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE"], ctypes.c_byte(GLOW_VISIBLE_TYPE)) # Usually 1

        # 4. Set glow color (r, g, b)
        # The C++ code writes an array of 3 floats for highlightParameter
        # Assuming GLOW_COLOR is the start of these 3 floats
        glow_color_struct = (ctypes.c_float * 3)(color[0], color[1], color[2])
        self.mem.WriteArray(self.ptr + GAME_OFFSETS["GLOW_COLOR"], glow_color_struct)

        # 5. Set other parameters like fade times, border size etc.
        # These are often part of the HighlightFlowSettings_t structure in C++ or specific offsets.
        # Example from C++ (these are often fixed values or derived):
        # self.mem.Write(self.ptr + GAME_OFFSETS["GLOW_LIFE_TIME"], ctypes.c_float(GLOW_LIFE_TIME)) # or int
        # self.mem.Write(self.ptr + GAME_OFFSETS["GLOW_DISTANCE"], ctypes.c_float(GLOW_MAX_DIST)) # example

        # The C++ code also writes to these offsets in the enableEntityGlow function:
        # offset.GLOW_START_TIME -> GLOW_FADE_START_TIME (5000.0f)
        # offset.GLOW_FADE -> GLOW_FADE_END_TIME (5000.0f)
        # offset.OFFSET_GLOW_FIX -> GLOW_FIX (35)
        # These seem to be more about setting up the parameters for the glow type.
        # Note: GAME_OFFSETS["GLOW_START_TIME"] is 0x2F8, GAME_OFFSETS["GLOW_FADE"] is 0x3B8.
        # These were calculated from other base offsets + 0x30 in offsets.py.
        # The actual values 5000.0f and 35 are constants.

        # Example of writing some of these fixed parameters for the glow effect:
        # self.mem.Write(self.ptr + GAME_OFFSETS["GLOW_START_TIME"], ctypes.c_float(GLOW_FADE_START_TIME)) # This offset might be misnamed if it expects a fixed value.
                                                                                                       # OFFSET_VISIBLE_TIME is where dynamic time is read.
        # self.mem.Write(self.ptr + GAME_OFFSETS["GLOW_FADE"], ctypes.c_float(GLOW_FADE_END_TIME)) # Similar to above.

        # The C++ code from apex_dma.cpp in enableEntityGlow function:
        # write<float>(target.ptr + offset.GLOW_START_TIME, value.GLOW_FADE_START_TIME);
        # write<float>(target.ptr + offset.GLOW_FADE, value.GLOW_FADE_END_TIME);
        # write<int>(target.ptr + offset.OFFSET_GLOW_FIX, value.GLOW_FIX);
        # This implies these are parameters for the glow effect itself, not just flags.
        # Let's assume these are fixed values to set up the glow effect properties.
        # The actual float values like 5000.0f are from the global constants.

        # For now, keeping it to the core elements (enable, context, type, color, visibility)
        # Full replication of all HighlightFlowSettings_t fields is very detailed.
        pass

    def is_dummy(self) -> bool:
        # Placeholder: Logic in C++ often checks name or other properties.
        # name = self.get_name_from_sign_name()
        # if "dummy" in name.lower() or "npc" in name.lower(): # Example
        #     return True
        return False # Default placeholder

    def is_player(self) -> bool:
        # Placeholder: May involve checking class ID, name, or other specific flags.
        # For instance, if there's an OFFSET_CLASSTYPE or similar.
        # Or checking if health > 0 and it's not a dummy.
        # name = self.get_name_from_sign_name()
        # if name and not "dummy" in name: # very basic
        #    return True
        return True # Defaulting to true for placeholder behavior

    def get_bone_position(self, bone_id: int) -> structs.Vector:
        # Placeholder: Complex operation involving:
        # 1. Reading bone matrix pointer: self.ptr + GAME_OFFSETS["OFFSET_BONES"]
        # 2. Reading the bone matrix itself (array of structs.Bone or similar)
        # 3. Transforming bone_id relative position by entity's world position/rotation.
        # bone_matrix_ptr = ctypes.c_uint64()
        # if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_BONES"], bone_matrix_ptr):
        #     if bone_matrix_ptr.value == 0: return structs.Vector(0,0,0)
        #     bone_data = structs.Bone() # Assuming Bone struct has x, y, z
        #     # The bone matrix is an array of matrices or vectors.
        #     # Reading a specific bone (e.g., bone_id) requires knowing the structure.
        #     # Example: bone_data_address = bone_matrix_ptr.value + (bone_id * sizeof(BoneMatrixEntry))
        #     # if self.mem.Read(bone_data_address, bone_data):
        #     #    return structs.Vector(bone_data.x, bone_data.y, bone_data.z) # This is simplified
        # print(f"Entity.get_bone_position({bone_id}) called - Placeholder")
        # return structs.Vector(0,0,0) # Placeholder return
        if not (self.ptr and self.mem and game_offsets):
            return None

        bone_array_base_ptr_val = ctypes.c_uint64()
        if not self.mem.Read(self.ptr + game_offsets["OFFSET_BONES"], bone_array_base_ptr_val):
            return None

        bone_array_base_ptr = bone_array_base_ptr_val.value
        if bone_array_base_ptr == 0:
            return None

        BONE_DATA_STRIDE = 0x30  # Common stride for bone data
        bone_data_addr = bone_array_base_ptr + (bone_id * BONE_DATA_STRIDE)

        bone_struct = structs.Bone() # structs.Bone has x, y, z fields at correct offsets

        if self.mem.Read(bone_data_addr, bone_struct):
            # The x,y,z in structs.Bone are world coordinates directly (based on typical game struct layouts)
            # No further transformation with entity origin needed here usually, as bone matrices are world space.
            return structs.Vector(x=bone_struct.x, y=bone_struct.y, z=bone_struct.z)

        return None

    def get_view_offset(self, game_offsets: dict) -> structs.Vector | None:
        view_offset_vec = structs.Vector()
        if self.ptr and self.mem and game_offsets:
            if self.mem.Read(self.ptr + game_offsets["OFFSET_VIEW_OFFSET"], view_offset_vec):
                return view_offset_vec
        return structs.Vector(0,0,0) # Return zero vector or None on failure

    def set_view_angles(self, angles: structs.QAngle, game_offsets: dict) -> bool:
        if self.ptr and self.mem and game_offsets:
            return self.mem.Write(self.ptr + game_offsets["OFFSET_VIEWANGLES"], angles)
        return False

    def get_recoil(self, game_offsets: dict) -> structs.QAngle | None:
        if not (self.ptr and self.mem and game_offsets):
            return None
        recoil_angles = structs.QAngle()
        if self.mem.Read(self.ptr + game_offsets["OFFSET_AIMPUNCH"], recoil_angles):
            return recoil_angles
        return None

    def is_player(self, game_offsets: dict) -> bool:
        """
        Heuristic to determine if the entity is a player.
        Checks for positive health and a plausible team ID.
        Excludes entities identified as dummies.
        """
        if not self.ptr or not self.mem:
            return False

        health = self.get_health() # Uses existing method
        team_id = self.get_team_id() # Uses existing method

        # Basic heuristic: positive health and a team_id that's not 0 (often reserved).
        # Real players usually have team_id > 0. Max team_id can vary, 60 is a common upper limit.
        # A more robust check might involve reading a class ID or specific flags if known.
        is_plausible_player = health > 0 and team_id != 0

        if not is_plausible_player:
            return False

        # Exclude if it's explicitly a dummy
        if self.is_dummy(game_offsets):
            return False

        # Further refinement could be checking name for "player" if that's a convention,
        # but can be unreliable. Relying on health, team, and not being a dummy is a common start.
        return True

    def is_dummy(self, game_offsets: dict) -> bool:
        """
        Heuristic to determine if the entity is a dummy/NPC based on its name.
        """
        if not self.ptr or not self.mem:
            return False

        # Name reading is expensive, do it last if other checks pass.
        # For is_dummy, name is the primary indicator.
        name_str = self.get_name_from_sign_name() # Uses existing method

        if name_str and name_str != "NameError_SignName": # Check if name was successfully read
            name_lower = name_str.lower()
            if "dummy" in name_lower or "npc_dummie" in name_lower:
                return True
        return False


class WeaponXEntity:
    def __init__(self, ptr: int, mem: Memory):
        self.ptr = ptr
        self.mem = mem

    def get_projectile_speed(self) -> float:
        # Placeholder: Read from self.ptr + GAME_OFFSETS["OFFSET_BULLET_SPEED"]
        # speed = ctypes.c_float()
        # if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_BULLET_SPEED"], speed):
        #     return speed.value
        print(f"WeaponXEntity.get_projectile_speed() for ptr {hex(self.ptr)} - Placeholder")
        return 0.0

    def get_projectile_gravity(self) -> float: # Often referred to as "scale" in some contexts
        # Placeholder: Read from self.ptr + GAME_OFFSETS["OFFSET_BULLET_SCALE"]
        # scale = ctypes.c_float() # May or may not be direct gravity, often a scale factor
        # if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_BULLET_SCALE"], scale):
        #     return scale.value
        print(f"WeaponXEntity.get_projectile_gravity() for ptr {hex(self.ptr)} - Placeholder")
        return 0.0

    def get_zoom_fov(self) -> float:
        # Placeholder: Read from self.ptr + GAME_OFFSETS["OFFSET_ZOOM_FOV"]
        # fov = ctypes.c_float()
        # if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_ZOOM_FOV"], fov):
        #     return fov.value
        print(f"WeaponXEntity.get_zoom_fov() for ptr {hex(self.ptr)} - Placeholder")
        return 0.0

    def get_ammo(self) -> int:
        # Placeholder: Read from self.ptr + GAME_OFFSETS["OFFSET_AMMO"]
        # ammo = ctypes.c_int()
        # if self.mem.Read(self.ptr + GAME_OFFSETS["OFFSET_AMMO"], ammo):
        #     return ammo.value
        print(f"WeaponXEntity.get_ammo() for ptr {hex(self.ptr)} - Placeholder")
        return 0

# Placeholder functions - will be defined in later steps
def get_entity(entity_ptr: int, mem: Memory, g_base: int) -> Entity:
    return Entity(entity_ptr, mem, g_base)

# --- Helper Math Functions ---
def normalize_angle(angle: float) -> float:
    """Normalize an angle to the range [-180, 180) or [-pi, pi)."""
    # Assuming degrees for now, common in game hacking contexts for angles
    while angle <= -180.0: angle += 360.0
    while angle > 180.0: angle -= 360.0
    return angle

def normalize_angles(angles: structs.QAngle) -> structs.QAngle:
    """Normalizes QAngle components. Pitch is clamped."""
    angles.x = normalize_angle(angles.x)
    angles.y = normalize_angle(angles.y)

    # Clamp pitch to avoid looking too far up or down
    if angles.x > 89.0: angles.x = 89.0
    if angles.x < -89.0: angles.x = -89.0

    angles.z = 0.0 # Usually roll is not set or needed for view angles
    return angles

def vector_subtract(v1: structs.Vector, v2: structs.Vector) -> structs.Vector:
    return structs.Vector(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z)

def vector_length(v: structs.Vector) -> float:
    return math.sqrt(v.x*v.x + v.y*v.y + v.z*v.z)

def calculate_angles_from_direction(src_pos: structs.Vector, dest_pos: structs.Vector) -> structs.QAngle:
    delta = vector_subtract(dest_pos, src_pos)
    if vector_length(delta) == 0: # Avoid division by zero if positions are identical
        return structs.QAngle(0,0,0)

    hyp = vector_length(structs.Vector(x=delta.x, y=delta.y, z=0)) # Length in XY plane (horizontal distance)

    yaw = math.atan2(delta.y, delta.x) * (180.0 / math.pi)
    pitch = math.atan2(-delta.z, hyp) * (180.0 / math.pi)

    return normalize_angles(structs.QAngle(x=pitch, y=yaw, z=0.0))

def calculate_fov_distance(current_angles: structs.QAngle, target_angles: structs.QAngle) -> float:
    """Calculates the FOV 'distance' between two sets of angles."""
    # Normalize angles before calculating delta to ensure smallest angle difference
    # This is important for yaw, which can wrap around 360.
    norm_current_y = normalize_angle(current_angles.y)
    norm_target_y = normalize_angle(target_angles.y)

    delta_yaw = normalize_angle(norm_target_y - norm_current_y)

    # Pitch usually doesn't need this complex normalization if clamped to +/- 89
    delta_pitch = normalize_angle(target_angles.x - current_angles.x)

    return math.sqrt(delta_yaw*delta_yaw + delta_pitch*delta_pitch)


# --- Main Game Logic Functions ---

def world_to_screen(world_pos: structs.Vector, view_matrix_data: structs.MatrixData, screen_width: int, screen_height: int) -> structs.Vector | None:
    """
    Projects a 3D world position to 2D screen coordinates.
    Returns a Vector with x, y screen coordinates, or None if behind camera.
    """
    if not world_pos or not view_matrix_data:
        return None

    matrix = view_matrix_data.matrix # This is float[16]

    # Homogeneous coordinates for the world position
    world_x, world_y, world_z = world_pos.x, world_pos.y, world_pos.z

    # Transform by the view matrix (matrix is column-major or row-major? Assuming standard row-major for multiplication)
    # If matrix is from DirectX/OpenGL, it's typically column-major if passed as is.
    # If it's just a float[16] array, common layout is m[row][col].
    # matrix[0] matrix[1] matrix[2] matrix[3]  (row 0)
    # matrix[4] matrix[5] matrix[6] matrix[7]  (row 1)
    # ...
    # Clip Coordinate W (perspective Z)
    clip_w = world_x * matrix[3] + world_y * matrix[7] + world_z * matrix[11] + matrix[15]

    if clip_w < 0.01: # Point is behind or too close to the camera plane
        return None

    # Clip Coordinate X
    clip_x = world_x * matrix[0] + world_y * matrix[4] + world_z * matrix[8] + matrix[12]
    # Clip Coordinate Y
    clip_y = world_x * matrix[1] + world_y * matrix[5] + world_z * matrix[9] + matrix[13]

    # Normalized Device Coordinates (NDC)
    ndc_x = clip_x / clip_w
    ndc_y = clip_y / clip_w

    # Screen Coordinates
    # Common formula:
    # ScreenX = (ScreenWidth / 2 * NDCX) + (NDCX + ScreenWidth / 2) - this seems a bit off.
    # Standard formula:
    # ScreenX = (NDCX + 1.0) * ScreenWidth / 2.0
    # ScreenY = (1.0 - NDCY) * ScreenHeight / 2.0 (Y is often inverted)

    screen_x = (screen_width / 2.0) + (ndc_x * screen_width / 2.0)
    screen_y = (screen_height / 2.0) - (ndc_y * screen_height / 2.0) # Y is inverted in screen space

    return structs.Vector(x=screen_x, y=screen_y, z=0) # z component is not typically used for 2D screen pos

def calculate_fov(local_player: Entity, target_entity: Entity, game_offsets: dict) -> float: # Added game_offsets
    # This is a simplified FOV calculation based on current view vs target direction
    # A more common use of "FOV" in cheats is the angular distance from crosshair to target.
    if not local_player or not target_entity:
        return 360.0 # Max FOV if inputs are bad

    local_cam_pos = vector_subtract(local_player.get_position(), local_player.get_view_offset(game_offsets)) # Assuming subtraction for this example
    # Alternative: local_cam_pos = structs.Vector(local_player.get_position().x + local_view_offset.x, ...)

    # For simplicity, let's use the target's origin. For bone-specific FOV, use bone position.
    target_pos = target_entity.get_position()
    if not target_pos: return 360.0

    desired_angles_to_target = calculate_angles_from_direction(local_cam_pos, target_pos)
    current_view_angles = local_player.get_view_angles()

    if not current_view_angles: return 360.0

    return calculate_fov_distance(current_view_angles, desired_angles_to_target)


def calculate_best_bone_aim(local_player: Entity, target_entity: Entity, max_fov_degrees: float, aim_bone_id: int, game_offsets: dict) -> structs.QAngle | None:
    if not local_player or not target_entity:
        return None

    local_view_offset = local_player.get_view_offset(game_offsets)
    if not local_view_offset: return None # Ensure view_offset is valid

    # Common way to get camera position: player origin + view_offset (head height)
    # Check specific game implementation; sometimes it's subtraction if view_offset is from a different reference.
    # Assuming addition here as it's more standard for view_offset representing height from origin.
    local_cam_pos = structs.Vector(
        local_player.get_position().x + local_view_offset.x,
        local_player.get_position().y + local_view_offset.y,
        local_player.get_position().z + local_view_offset.z
    )

    target_bone_pos = target_entity.get_bone_position(aim_bone_id, game_offsets)
    if not target_bone_pos:
        # print(f"AimDebug: Failed to get bone {aim_bone_id} for entity {target_entity.get_ptr()}")
        return None

    desired_angles = calculate_angles_from_direction(local_cam_pos, target_bone_pos)

    current_angles = local_player.get_view_angles()
    if not current_angles:
        # print("AimDebug: Failed to get local player current view angles.")
        return None

    fov = calculate_fov_distance(current_angles, desired_angles)

    # print(f"AimDebug: Target Ptr: {hex(target_entity.get_ptr())}, Bone: {aim_bone_id}, DesiredAngles: ({desired_angles.x:.2f}, {desired_angles.y:.2f}), FOV: {fov:.2f}")

    if fov <= max_fov_degrees:
        return normalize_angles(desired_angles) # Return the calculated angles if within FOV

    return None


if __name__ == '__main__':
    # This section can be used for basic testing of game.py components later
    print("py_apex_dma/game.py updated with new Entity methods and math helpers.")

    # Example Test (requires mock Memory and structs)
    # class MockMemory:
    #     def Read(self, addr, ctype_obj): print(f"MockRead: {hex(addr)} into {type(ctype_obj)}"); return True
    #     def Write(self, addr, ctype_obj): print(f"MockWrite: {hex(addr)} from {type(ctype_obj)}"); return True
    # mock_mem = MockMemory()
    # mock_offsets = {"OFFSET_VIEW_OFFSET": 0x10, "OFFSET_BONES": 0x20, "OFFSET_VIEWANGLES": 0x30, "OFFSET_AIMPUNCH": 0x40}
    # test_entity = Entity(0x1000, mock_mem)
    # test_entity.get_view_offset(mock_offsets)
    # test_entity.get_bone_position(0, mock_offsets) # BONE_HEAD
    pass


def set_player_glow(mem: Memory, local_player: Entity, target_entity: Entity, target_index: int, game_settings: dict):
    if not game_settings.get("player_glow_enabled", False):
        if target_entity.is_glowing(): # is_glowing will be updated/added in Entity
            target_entity.disable_glow() # disable_glow will be updated/added in Entity
        return

    # Read current glow state
    # current_glow_enable_val = ctypes.c_byte() # Assuming is_glowing handles the read
    # mem.Read(target_entity.ptr + GAME_OFFSETS["OFFSET_GLOW_ENABLE"], current_glow_enable_val)

    glow_visible_type_val_byte = ctypes.c_byte()
    mem.Read(target_entity.ptr + GAME_OFFSETS["OFFSET_GLOW_THROUGH_WALLS_GLOW_VISIBLE_TYPE"], glow_visible_type_val_byte)
    glow_visible_type_val = glow_visible_type_val_byte.value

    # Logic from C++: if (!target.isGlowing() || target.getGlowVisibleType() != GLOW_VISIBLE_TYPE)
    # GLOW_VISIBLE_TYPE is 1 from our global constants
    if not target_entity.is_glowing() or glow_visible_type_val != GLOW_VISIBLE_TYPE:
        current_entity_time = target_entity.last_visible_time() # Using actual last visible time

        # C++ uses: float currentEntityTime = mem.Read<float>(target.ptr + OFFSET_VISIBLE_TIME);
        # And then checks: if (currentEntityTime > 0.f && !isnan(currentEntityTime) && currentEntityTime < GLOW_FADE_MAX_TIME)
        # The GLOW_FADE_MAX_TIME check seems odd if currentEntityTime is a timestamp.
        # The C++ code has `GLOW_FADE_START_TIME` and `GLOW_FADE_END_TIME` as fixed values (5000.0f)
        # which are used in `enable_glow` method directly, not as a condition here.
        # For now, let's assume if last_visible_time is positive, it's a valid time.

        if current_entity_time > 0.0 and not math.isnan(current_entity_time):
            context_id = 0
            # setting_index refers to HighlightFunctionBits in C++, which is complex.
            # For now, we'll pass a simplified value or map it like C++ context IDs.
            # The C++ code uses specific context IDs for different states.
            setting_index = GLOW_VISIBLE_TYPE # Defaulting to visible type, this might need refinement based on C++ HighlightFunctionBits logic.
                                            # The C++ code does `write<int>(target.ptr + OFFSET_GLOW_TYPE, 타입);` where 타입 is settingIndex.
                                            # And `write<int>(target.ptr + OFFSET_GLOW_CONTEXT_ID, 컨텍스트);`

            r, g, b = 0.0, 0.0, 0.0

            is_knocked = target_entity.is_knocked()
            is_alive = target_entity.is_alive()

            if not game_settings.get("firing_range", False) and (is_knocked or not is_alive):
                r = game_settings.get("glowrknc", GLOW_R_KNC)
                g = game_settings.get("glowgknc", GLOW_G_KNC)
                b = game_settings.get("glowbknc", GLOW_B_KNC)
                context_id = GLOW_CONTEXT_ID_KNOCKED # 44
            # C++ logic: else if (target.lastVisibleTime() > game_settings.lastVisAimTimes[i])
            elif target_entity.last_visible_time() > game_settings.get("last_vis_aim_times", {}).get(target_index, 0.0):
                r = game_settings.get("glowrviz", GLOW_R_VIZ)
                g = game_settings.get("glowgviz", GLOW_G_VIZ)
                b = game_settings.get("glowbviz", GLOW_B_VIZ)
                context_id = GLOW_CONTEXT_ID_VISIBLE # 5
            else: # Not visible or firing range with no special conditions
                r = game_settings.get("glowr", GLOW_R)
                g = game_settings.get("glowg", GLOW_G)
                b = game_settings.get("glowb", GLOW_B)
                context_id = GLOW_CONTEXT_ID_INVISIBLE # 1

            highlight_params = (r, g, b)
            # The enable_glow method in Entity will handle the multiple writes.
            # Parameters like border size, modes are often part of HighlightFlowSetting in C++
            # which are applied via context_id and setting_index.
            target_entity.enable_glow(context_id, setting_index, highlight_params)
    # else:
        # If already glowing correctly, no action needed based on this simplified logic.
        # More complex scenarios might require re-applying glow if color settings change dynamically.
        # print(f"Entity {target_index} already glowing as expected.")
        pass


def process_player(mem: Memory, g_base: int, local_player: Entity, target_entity: Entity, target_index: int, game_settings: dict):
    if not target_entity or not local_player:
        return

    # Get target name (primarily for debugging or logging, not used in core logic here)
    # target_name = target_entity.get_name_from_sign_name()
    # print(f"Processing player {target_index}: {target_name}")

    if not target_entity.is_alive():
        # print(f"Target {target_index} is not alive.")
        # If not alive, consider disabling glow if it was previously enabled
        if target_entity.is_glowing():
            target_entity.disable_glow()
        return

    entity_team = target_entity.get_team_id()

    # Placeholder for target.Observing(entitylist) logic.
    # This would check if the target is currently being spectated by someone important,
    # potentially altering behavior (e.g., not aiming at them).
    # is_being_observed_by_enemy = False # Placeholder

    # Distance check
    # This requires Vector class to have dist_to method.
    try:
        dist = local_player.get_position().dist_to(target_entity.get_position())
    except AttributeError:
        # This will happen if dist_to is not yet implemented in Vector
        print("Error: Vector.dist_to() method not yet implemented. Using placeholder distance 0.")
        dist = 0.0
        # Fallback or re-raise, for now, using 0 to allow flow.
        # For robust code, this should be handled properly once Vector.dist_to is confirmed.

    if dist > game_settings.get("max_dist", GLOW_MAX_DIST): # Use global GLOW_MAX_DIST if not in settings
        # print(f"Target {target_index} distance {dist} > max_dist")
        return

    # Team check
    if not game_settings.get("firing_range", False) and not game_settings.get("onevone", False):
        if entity_team < 0 or entity_team > 50 or entity_team == game_settings.get("team_player", -1): # team_player should be local_player's team
            # print(f"Target {target_index} is on the same team or invalid team_id.")
            return

    # Aimbot related logic (placeholder for now)
    # fov = calculate_fov(local_player, target_entity)
    # if game_settings.get("aim_enabled", False) and fov < game_settings.get("max_fov", 30.0):
    #     # Perform aim calculations, potentially find best bone
    #     # aim_angle = calculate_best_bone_aim(local_player, target_entity.get_ptr(), game_settings.get("max_fov", 30.0))
    #     # if aim_angle is not None and (aim_angle.x != 0 or aim_angle.y != 0):
    #     #    local_player.set_view_angles(aim_angle) # Entity would need set_view_angles
    #     pass

    # Player Glow
    set_player_glow(mem, local_player, target_entity, target_index, game_settings)

    # Update last visible/aim times (used by set_player_glow logic)
    if "last_vis_aim_times" in game_settings and isinstance(game_settings["last_vis_aim_times"], dict):
        game_settings["last_vis_aim_times"][target_index] = target_entity.last_visible_time()
    else:
        # Initialize if not present, though it should be by the calling context
        game_settings["last_vis_aim_times"] = {target_index: target_entity.last_visible_time()}
