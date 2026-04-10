use std::ops::Sub;

#[derive(Copy, Clone, Debug, Default, PartialEq)]
pub struct Vector3 {
    pub x: f32,
    pub y: f32,
    pub z: f32,
}

impl Vector3 {
    pub fn new(x: f32, y: f32, z: f32) -> Self {
        Self { x, y, z }
    }

    pub fn dist_to(&self, other: &Self) -> f32 {
        ((self.x - other.x).powi(2) + (self.y - other.y).powi(2) + (self.z - other.z).powi(2)).sqrt()
    }

    pub fn length(&self) -> f32 {
        (self.x.powi(2) + self.y.powi(2) + self.z.powi(2)).sqrt()
    }
}

impl Sub for Vector3 {
    type Output = Self;
    fn sub(self, other: Self) -> Self {
        Self::new(self.x - other.x, self.y - other.y, self.z - other.z)
    }
}

pub fn normalize_angles(mut angle: Vector3) -> Vector3 {
    while angle.x > 89.0 {
        angle.x -= 180.0;
    }
    while angle.x < -89.0 {
        angle.x += 180.0;
    }
    while angle.y > 180.0 {
        angle.y -= 360.0;
    }
    while angle.y < -180.0 {
        angle.y += 360.0;
    }
    angle
}

pub fn calc_angle(src: Vector3, dst: Vector3) -> Vector3 {
    let delta = src - dst;
    let hyp = (delta.x.powi(2) + delta.y.powi(2)).sqrt();

    let mut angle = Vector3::new(
        (delta.z / hyp).atan().to_degrees(),
        (delta.y / delta.x).atan().to_degrees(),
        0.0
    );

    if delta.x >= 0.0 {
        angle.y += 180.0;
    }

    normalize_angles(angle)
}

pub fn get_fov(view_angle: Vector3, aim_angle: Vector3) -> f32 {
    let delta = normalize_angles(aim_angle - view_angle);
    (delta.x.powi(2) + delta.y.powi(2)).sqrt()
}
