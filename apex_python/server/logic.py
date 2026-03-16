from .offsets import *
import struct
from .math_utils import QAngle, Vector
import math

def calculate_fov(from_ent, target_ent):
    view_angles = from_ent.get_view_angles()
    cam_pos = from_ent.get_cam_pos()
    target_pos = target_ent.get_origin()

    delta = target_pos - cam_pos
    hyp = math.sqrt(delta.x**2 + delta.y**2)

    ax = math.atan2(-delta.z, hyp) * (180.0 / math.pi)
    ay = math.atan2(delta.y, delta.x) * (180.0 / math.pi)

    aim_angle = QAngle(ax, ay, 0.0)

    diff = aim_angle - view_angles
    diff.normalize()

    return math.sqrt(diff.x**2 + diff.y**2)

class Weapon:
    def __init__(self, proc, local_ptr):
        self.proc = proc
        wep_handle = proc.read(local_ptr + OFFSET_WEAPON, "Q")
        if wep_handle:
            wep_handle &= 0xFFFF
            self.wep_entity = proc.read(proc.base + OFFSET_ENTITYLIST + (wep_handle << 5), "Q")
        else:
            self.wep_entity = 0

        if self.wep_entity:
            self.projectile_speed = proc.read(self.wep_entity + OFFSET_BULLET_SPEED, "f")
            self.projectile_scale = proc.read(self.wep_entity + OFFSET_BULLET_SCALE, "f")
            self.zoom_fov = proc.read(self.wep_entity + OFFSET_ZOOM_FOV, "f")
        else:
            self.projectile_speed = 0
            self.projectile_scale = 0
            self.zoom_fov = 0

def calculate_best_bone_aim(from_ent, target_ent, bone_id, smoothing, aim_no_recoil):
    local_camera = from_ent.get_cam_pos()
    target_bone_pos = target_ent.get_bone_pos(bone_id)

    # Prediction
    weapon = Weapon(from_ent.proc, from_ent.ptr)
    bullet_speed = weapon.projectile_speed
    if bullet_speed > 1.0:
        dist = local_camera.dist_to(target_bone_pos)
        t = dist / (bullet_speed * 0.92) # Bullet drop/speed compensation
        # Simplified: target_bone_pos += target_vel * t
        pass

    delta = target_bone_pos - local_camera
    hyp = math.sqrt(delta.x**2 + delta.y**2)

    ax = math.atan2(-delta.z, hyp) * (180.0 / math.pi)
    ay = math.atan2(delta.y, delta.x) * (180.0 / math.pi)

    calculated_angles = QAngle(ax, ay, 0.0)
    view_angles = from_ent.get_view_angles()

    if aim_no_recoil:
        sway = from_ent.get_sway_angles()
        calculated_angles = calculated_angles - (sway - view_angles)

    calculated_angles.normalize()
    delta_angles = calculated_angles - view_angles
    delta_angles.normalize()

    smoothed_angles = view_angles + delta_angles / smoothing
    return smoothed_angles
