use glam::{Vec3, Vec2, Mat4};

pub fn world_to_screen(from: Vec3, matrix: [f32; 16], width: i32, height: i32) -> Option<Vec2> {
    let m = Mat4::from_cols_array(&matrix).transpose();

    let w = m.col(3).x * from.x + m.col(3).y * from.y + m.col(3).z * from.z + m.col(3).w;

    if w < 0.01 {
        return None;
    }

    let x = m.col(0).x * from.x + m.col(0).y * from.y + m.col(0).z * from.z + m.col(0).w;
    let y = m.col(1).x * from.x + m.col(1).y * from.y + m.col(1).z * from.z + m.col(1).w;

    let screen_x = (width as f32 / 2.0) * (1.0 + x / w);
    let screen_y = (height as f32 / 2.0) * (1.0 - y / w);

    Some(Vec2::new(screen_x, screen_y))
}

pub fn calculate_fov(view_angles: Vec3, angle_to_target: Vec3) -> f32 {
    let mut delta = angle_to_target - view_angles;

    if delta.y > 180.0 { delta.y -= 360.0; }
    if delta.y < -180.0 { delta.y += 360.0; }

    (delta.x.powi(2) + delta.y.powi(2)).sqrt()
}
