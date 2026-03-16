import memflow
from .offsets import *
from .math_utils import Vector, QAngle
import struct

class GameProcess:
    def __init__(self, name="r5apex_dx12.ex"):
        self.inventory = memflow.Inventory()
        try:
            self.os = self.inventory.create_os("win32", connector="kvm")
        except:
            self.os = self.inventory.create_os("win32", connector="qemu")

        self.process = self.os.process_by_name(name)
        self.base = self.process.module_by_name(name).base

    def read(self, addr, fmt):
        size = struct.calcsize(fmt)
        data = self.process.read(addr, size)
        if not data: return None
        return struct.unpack(fmt, data)[0]

    def read_array(self, addr, fmt, count):
        size = struct.calcsize(fmt) * count
        data = self.process.read(addr, size)
        if not data: return None
        return struct.unpack(f"{count}{fmt}", data)

    def write(self, addr, fmt, value):
        data = struct.pack(fmt, value)
        self.process.write(addr, data)

class Entity:
    def __init__(self, proc, ptr):
        self.proc = proc
        self.ptr = ptr
        # Optimization: only read necessary header parts instead of full 0x3FF0
        self.buffer = self.proc.process.read(ptr, 0x1000) # Read first 4KB for common offsets

    def get_team(self):
        return struct.unpack_from("i", self.buffer, OFFSET_TEAM)[0]

    def get_health(self):
        return struct.unpack_from("i", self.buffer, OFFSET_HEALTH)[0]

    def get_shield(self):
        return struct.unpack_from("i", self.buffer, OFFSET_SHIELD)[0]

    def get_max_shield(self):
        return struct.unpack_from("i", self.buffer, OFFSET_MAXSHIELD)[0]

    def get_armortype(self):
        return struct.unpack_from("i", self.buffer, OFFSET_ARMORTYPE)[0]

    def get_origin(self):
        return Vector(*struct.unpack_from("fff", self.buffer, OFFSET_ORIGIN))

    def is_alive(self):
        return struct.unpack_from("i", self.buffer, OFFSET_LIFE_STATE)[0] == 0

    def is_player(self):
        name_ptr = struct.unpack_from("Q", self.buffer, OFFSET_NAME)[0]
        # In Apex, players have a specific name pointer pattern or we can check class name
        # For simplicity and consistency with C++, we use the constant found there
        return name_ptr == 125780153691248

    def is_knocked(self):
        return struct.unpack_from("i", self.buffer, OFFSET_BLEED_OUT_STATE)[0] > 0

    def last_vis_time(self):
        return struct.unpack_from("f", self.buffer, OFFSET_VISIBLE_TIME)[0]

    def get_view_angles(self):
        return QAngle(*struct.unpack_from("fff", self.buffer, OFFSET_VIEWANGLES))

    def get_cam_pos(self):
        return Vector(*struct.unpack_from("fff", self.buffer, OFFSET_CAMERAPOS))

    def get_sway_angles(self):
        return QAngle(*struct.unpack_from("fff", self.buffer, OFFSET_BREATH_ANGLES))

    def get_aim_punch(self):
        return QAngle(*struct.unpack_from("fff", self.buffer, OFFSET_AIMPUNCH))

    def get_bone_pos(self, bone_id):
        origin = self.get_origin()
        model = struct.unpack_from("Q", self.buffer, OFFSET_STUDIOHDR)[0]
        studio_hdr = self.proc.read(model + 0x8, "Q")

        # Hitbox logic
        hitbox_cache = self.proc.read(studio_hdr + 0x34, "H")
        hitbox_array = studio_hdr + ((hitbox_cache & 0xFFFE) << (4 * (hitbox_cache & 1)))

        index_cache = self.proc.read(hitbox_array + 0x4, "H")
        hitbox_index = ((index_cache & 0xFFFE) << (4 * (index_cache & 1)))

        bone = self.proc.read(hitbox_array + hitbox_index + (bone_id * 0x20), "H")

        if bone < 0 or bone > 255:
            return origin

        bone_array = struct.unpack_from("Q", self.buffer, OFFSET_BONES)[0]
        # matrix3x4_t is 3 rows of 4 floats
        matrix = self.proc.read_array(bone_array + bone * 48, "f", 12)

        return Vector(matrix[3] + origin.x, matrix[7] + origin.y, matrix[11] + origin.z)

    def set_view_angles(self, angles):
        data = struct.pack("fff", angles.x, angles.y, angles.z)
        self.proc.process.write(self.ptr + OFFSET_VIEWANGLES, data)

    def is_glowing(self):
        return struct.unpack_from("i", self.buffer, OFFSET_GLOW_ENABLE)[0] == 7

    def enable_glow(self, context_id, setting_index, colors):
        # Implementation of SetPlayerGlow/enableGlow
        self.proc.write(self.ptr + OFFSET_GLOW_ENABLE, "i", context_id)
        self.proc.write(self.ptr + OFFSET_GLOW_THROUGH_WALLS, "i", 2)
        highlight_settings = self.proc.read(self.proc.base + HIGHLIGHT_SETTINGS, "Q")

        # glow mode bits
        # fill, outside, thickness, entityvis
        glow_bits = struct.pack("BBBB", 6, 125, 32, 64)
        self.proc.process.write(highlight_settings + HIGHLIGHT_TYPE_SIZE * context_id, glow_bits)
        self.proc.process.write(highlight_settings + HIGHLIGHT_TYPE_SIZE * context_id + 4, struct.pack("fff", *colors))

    def disable_glow(self):
        self.proc.write(self.ptr + OFFSET_GLOW_ENABLE, "i", 0)

class Item:
    def __init__(self, proc, ptr):
        self.proc = proc
        self.ptr = ptr

    def get_origin(self):
        return self.proc.read_vector(self.ptr + OFFSET_ORIGIN)

    def get_model_name(self):
        model_name_ptr = self.proc.read(self.ptr + OFFSET_MODELNAME, "Q")
        if model_name_ptr:
            data = self.proc.process.read(model_name_ptr, 64)
            return data.split(b'\0')[0].decode('utf-8', errors='ignore')
        return "unknown"

    def enable_glow(self, glow_id):
        self.proc.write(self.ptr + OFFSET_GLOW_ENABLE, "B", glow_id)
        self.proc.write(self.ptr + OFFSET_GLOW_THROUGH_WALLS, "B", 2)
