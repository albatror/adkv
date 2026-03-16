# communication.py
import struct
import ctypes

# In the original C++ project, host and guest sync via a shared memory buffer
# specifically the 'add' array in Client.exe.
# Host finds Client.exe base and reads the 'add' array address to sync data.

# We define the structure for ESP data transfer
# Each player is 15 floats + 33 chars + 15*2 floats for bones.
PLAYER_STRUCT_FMT = "ffffffff??iiii33s30f"
PLAYER_STRUCT_SIZE = struct.calcsize(PLAYER_STRUCT_FMT)

class SharedData:
    def __init__(self):
        self.check = 0xABCD
        self.aim = 0
        self.esp = False
        self.aiming = False
        self.g_base = 0
        self.next = False
        self.valid = False
        # ... mapping to the 64-element add array

def get_add_array_offsets():
    # Mapping indices based on main.cpp
    return {
        'check': 0,
        'aim': 1,
        'esp': 2,
        'aiming': 3,
        'g_base': 4,
        'next': 5,
        'players': 6,
        'valid': 7,
        'max_dist': 8,
        'item_glow': 9,
        'player_glow': 48,
        'spectators': 14,
        'allied_spectators': 15,
        'bones': 31,
        'screen_width': 32,
        'screen_height': 33,
    }
