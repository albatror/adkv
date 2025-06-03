from enum import Enum
import ctypes
import math # Added for sqrt in Vector.dist_to

class ProcessStatus(Enum):
    NOT_FOUND = 0
    FOUND_NO_ACCESS = 1
    FOUND_READY = 2

class Bone(ctypes.Structure):
    _fields_ = [
        ("pad1", ctypes.c_uint8 * 0xCC),
        ("x", ctypes.c_float),
        ("pad2", ctypes.c_uint8 * 0xC),
        ("y", ctypes.c_float),
        ("pad3", ctypes.c_uint8 * 0xC),
        ("z", ctypes.c_float),
    ]

class GColor(ctypes.Structure):
    _fields_ = [
        ("r", ctypes.c_float),
        ("g", ctypes.c_float),
        ("b", ctypes.c_float),
    ]

class Fade(ctypes.Structure):
    _fields_ = [
        ("a", ctypes.c_int),
        ("b", ctypes.c_int),
        ("c", ctypes.c_float),
        ("d", ctypes.c_float),
        ("e", ctypes.c_float),
        ("f", ctypes.c_float),
    ]

class GlowMode(ctypes.Structure):
    _fields_ = [
        ("GeneralGlowMode", ctypes.c_int8),
        ("BorderGlowMode", ctypes.c_int8),
        ("BorderSize", ctypes.c_int8),
        ("TransparentLevel", ctypes.c_int8),
    ]

class SVector(ctypes.Structure):
    _fields_ = [
        ("x", ctypes.c_float),
        ("y", ctypes.c_float),
        ("z", ctypes.c_float),
    ]

# Placeholder for Vector and QAngle, will be properly defined after analyzing vector.h
# For now, assuming they are similar to SVector for ctypes compatibility in other structs if needed.
class Vector(SVector):
    def __str__(self):
        return f"Vector(x={self.x}, y={self.y}, z={self.z})"

    def dist_to(self, other_vector: 'Vector') -> float:
        """Calculates the Euclidean distance to another Vector."""
        if not isinstance(other_vector, Vector):
            # Or handle SVector if QAngle also needs this directly via SVector
            raise TypeError("other_vector must be an instance of Vector")
        delta_x = self.x - other_vector.x
        delta_y = self.y - other_vector.y
        delta_z = self.z - other_vector.z
        return math.sqrt(delta_x**2 + delta_y**2 + delta_z**2)

class QAngle(SVector):
     def __str__(self):
        return f"QAngle(x={self.x}, y={self.y}, z={self.z})"

class ClientClass(ctypes.Structure):
    _fields_ = [
        ("pCreateFn", ctypes.c_uint64),
        ("pCreateEventFn", ctypes.c_uint64),
        ("pNetworkName", ctypes.c_uint64),
        ("pRecvTable", ctypes.c_uint64),
        ("pNext", ctypes.c_uint64),
        ("ClassID", ctypes.c_uint32),
        ("ClassSize", ctypes.c_uint32),
    ]

class KButton(ctypes.Structure):
    _fields_ = [
        ("down", ctypes.c_int * 2),
        ("state", ctypes.c_int),
    ]

class InState(ctypes.Structure):
    _fields_ = [
        ("button", KButton),
        ("state", ctypes.c_bool),
        ("force", ctypes.c_bool),
        ("press", ctypes.c_bool),
        ("release", ctypes.c_bool),
    ]

# Basic data structure for Entity (methods will be added later)
class EntityData(ctypes.Structure):
    _fields_ = [
        ("ptr", ctypes.c_uint64),
        ("buffer", ctypes.c_uint8 * 0x3FF0)
        # This buffer is large and its internal structure is determined by offsets.
        # Direct translation to a fixed ctypes Structure might be less flexible than
        # reading parts of it on demand based on offsets.
        # For now, it's represented as a byte array.
    ]

# Basic data structure for WeaponXEntity (methods will be added later)
class WeaponXEntityData(ctypes.Structure):
    _fields_ = [
        # Assuming these are the primary data fields based on class members.
        # Actual layout in memory for a class instance is more complex.
        # This is a simplified representation if we were to treat it as a plain struct.
        ("projectile_scale", ctypes.c_float),
        ("projectile_speed", ctypes.c_float),
        ("zoom_fov", ctypes.c_float),
        ("ammo", ctypes.c_int)
    ]

# From apex_dma.cpp
class PlayerData(ctypes.Structure):
    _fields_ = [
        ("dist", ctypes.c_float),
        ("entity_team", ctypes.c_int),
        ("boxMiddle", ctypes.c_float),
        ("h_y", ctypes.c_float), # head_y?
        ("width", ctypes.c_float),
        ("height", ctypes.c_float),
        ("b_x", ctypes.c_float), # base_x?
        ("b_y", ctypes.c_float), # base_y?
        ("knocked", ctypes.c_bool),
        ("visible", ctypes.c_bool),
        ("health", ctypes.c_int),
        ("shield", ctypes.c_int),
        ("maxshield", ctypes.c_int),
        ("armortype", ctypes.c_int),
		("player_xp_level", ctypes.c_int),
        ("name", ctypes.c_char * 33)
    ]

    def to_dict(self) -> dict:
        return {
            "dist": self.dist,
            "entity_team": self.entity_team,
            "boxMiddle": self.boxMiddle,
            "h_y": self.h_y,
            "width": self.width,
            "height": self.height,
            "b_x": self.b_x,
            "b_y": self.b_y,
            "knocked": self.knocked,
            "visible": self.visible,
            "health": self.health,
            "shield": self.shield,
            "maxshield": self.maxshield,
            "armortype": self.armortype,
            "player_xp_level": self.player_xp_level,
            # Decode name from bytes to string for JSON serialization
            "name": self.name.decode(errors='ignore') if isinstance(self.name, bytes) else str(self.name),
        }

class SpectatorData(ctypes.Structure):
	_fields_ = [
		("is_spec", ctypes.c_bool),
		("name", ctypes.c_char * 33)
	]

class MatrixData(ctypes.Structure): # Renamed from Matrix to avoid conflict if there's a Matrix class later
    _fields_ = [
        ("matrix", ctypes.c_float * 16)
    ]
