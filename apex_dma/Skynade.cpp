#include <cmath>
#include "vector.h"
#include "Skynade.h"

const Skynade::Pitch Skynade::GRENADE_PITCHES[] = {
    {-1.5533f, -1.3990f}, // 89
    {-1.4837f, -1.3267f}, // 85
    {-1.3962f, -1.2433f}, // 80
    {-1.3092f, -1.1534f}, // 75
    {-1.2217f, -1.0779f}, // 70
    {-1.1347f, -0.9783f}, // 65
    {-1.0472f, -0.8977f}, // 60
    {-0.9602f, -0.8104f}, // 55
    {-0.8727f, -0.7268f}, // 50
    {-0.7857f, -0.6375f}, // 45
    {-0.6981f, -0.5439f}, // 40
    {-0.6112f, -0.4688f}, // 35
    {-0.5236f, -0.3880f}, // 30
    {-0.3491f, -0.2050f}, // 25
    {-0.3491f, -0.2050f}, // 20
    {-0.2615f, -0.1165f}, // 15
    {-0.1746f, -0.0421f}, // 10
    {-0.0870f, 0.0644f},  //  5
    {-0.0001f, 0.1403f},  //  0
    {0.0875f, 0.2358f},   // -5
    {0.1745f, 0.3061f},   //-10
    {0.2620f, 0.3753f},   //-15
    {0.3490f, 0.4684f},   //-20
    {0.4365f, 0.5343f},   //-25
    {0.5235f, 0.6238f},   //-30
    {0.6110f, 0.6865f},   //-35
    {0.6979f, 0.7756f},   //-40
    {0.7331f, 0.7968f},   //-42
    {0.7682f, 0.8341f},   //-44
    {0.8027f, 0.8771f},   //-46
    {0.8379f, 0.9038f},   //-48
    {0.8727f, 0.9382f},   //-50
    {0.9079f, 0.9620f},   //-52
    {0.9424f, 1.0048f},   //-54
    {0.9775f, 1.0333f},   //-56
    {1.0121f, 1.0561f},   //-58
    {1.0472f, 1.0987f},   //-60
    {1.0824f, 1.1217f},   //-62
    {1.1175f, 1.1628f},   //-64
    {1.1520f, 1.1868f},   //-66
    {1.1866f, 1.2239f},   //-68
    {1.2217f, 1.2555f},   //-70
    {1.2563f, 1.2859f},   //-72
    {1.2913f, 1.3156f},   //-74
    {1.3264f, 1.3470f},   //-76
    {1.3615f, 1.3822f},   //-78
    {1.3973f, 1.4108f},   //-80
    {1.4837f, 1.4919f},   //-85
    {1.5533f, 1.5546f},   //-89
};
const int Skynade::GRENADE_PITCHES_SIZE = sizeof(GRENADE_PITCHES) / sizeof(GRENADE_PITCHES[0]);

const Skynade::Pitch Skynade::ARC_PITCHES[] = {
    {-1.5533f, -1.5198f},
    {-1.3967f, -1.3672f},
    {-1.2222f, -1.1974f},
    {-1.0477f, -1.0260f},
    {-0.8731f, -0.8550f},
    {-0.6986f, -0.6848f},
    {-0.5241f, -0.5129f},
    {-0.3496f, -0.3416f},
    {-0.1572f, -0.1484f},
    {0.0000f, 0.0080f},
    {0.1751f, 0.1800f},
    {0.3496f, 0.3520f},
    {0.5241f, 0.5234f},
    {0.6992f, 0.6978f},
    {0.8727f, 0.8710f},
    {1.0472f, 1.0453f},
    {1.2218f, 1.2201f},
    {1.3963f, 1.3956f},
    {1.5533f, 1.5533f},
};
const int Skynade::ARC_PITCHES_SIZE = sizeof(ARC_PITCHES) / sizeof(ARC_PITCHES[0]);

const Skynade::Pitch Skynade::GRENADIER_GRENADE_PITCHES[] = {
    {-1.5533f, -1.3991f},
    {-1.3973f, -1.2456f},
    {-1.2227f, -1.0736f},
    {-1.0477f, -0.9010f},
    {-0.8737f, -0.7293f},
    {-0.6992f, -0.5562f},
    {-0.5247f, -0.3832f},
    {-0.3507f, -0.2101f},
    {-0.1762f, -0.0358f},
    {0.0000f, 0.1406f},
    {0.1745f, 0.2984f},
    {0.3496f, 0.4565f},
    {0.5247f, 0.6157f},
    {0.6987f, 0.7741f},
    {0.8732f, 0.9331f},
    {1.0477f, 1.0924f},
    {1.2222f, 1.2519f},
    {1.3973f, 1.4120f},
    {1.5533f, 1.5548f},
};
const int Skynade::GRENADIER_GRENADE_PITCHES_SIZE = sizeof(GRENADIER_GRENADE_PITCHES) / sizeof(GRENADIER_GRENADE_PITCHES[0]);

const Skynade::Pitch Skynade::GRENADIER_ARC_PITCHES[] = {
    {-1.5533f, -1.5193f},
    {-1.3973f, -1.3657f},
    {-1.2222f, -1.1931f},
    {-1.0477f, -1.0210f},
    {-0.8731f, -0.8485f},
    {-0.6986f, -0.6759f},
    {-0.5241f, -0.5034f},
    {-0.3496f, -0.3297f},
    {-0.1751f, -0.1554f},
    {0.0000f, 0.0190f},
    {0.1763f, 0.1916f},
    {0.3502f, 0.3624f},
    {0.5241f, 0.5339f},
    {0.6992f, 0.7065f},
    {0.8737f, 0.8792f},
    {1.0480f, 1.0518f},
    {1.2220f, 1.2251f},
    {1.3965f, 1.3976f},
    {1.5533f, 1.5534f},
};
const int Skynade::GRENADIER_ARC_PITCHES_SIZE = sizeof(GRENADIER_ARC_PITCHES) / sizeof(GRENADIER_ARC_PITCHES[0]);

// 角度转换函数
float Skynade::launch2view(const Pitch* pitches, int size, float launch) {
    if (size < 2) {
        return launch;
    }

    int low = 0;
    int high = size - 1;

    while (low + 1 != high) {
        int middle = (low + high) / 2;
        if (launch < pitches[middle].launch) {
            high = middle;
        }
        else {
            low = middle;
        }
    }

    float fraction = (launch - pitches[low].launch) / (pitches[high].launch - pitches[low].launch);
    return pitches[low].view + fraction * (pitches[high].view - pitches[low].view);
}

// 最佳角度计算
std::optional<float> Skynade::optimalAngle(float x, float y, float v0, float g) {
    float root = v0 * v0 * v0 * v0 - g * (g * x * x + 2.0f * y * v0 * v0);
    if (root < 0.0f) {
        return std::nullopt;
    }

    root = std::sqrt(root);
    float slope = (v0 * v0 - root) / (g * x);
    return std::atan(slope);
}

// 抛物线角度计算
std::optional<float> Skynade::lobAngle(float x, float y, float v0, float g) {
    float root = v0 * v0 * v0 * v0 - g * (g * x * x + 2.0f * y * v0 * v0);
    if (root < 0.0f) {
        return std::nullopt;
    }

    root = std::sqrt(root);
    float slope = (v0 * v0 + root) / (g * x);
    return std::atan(slope);
}

Vector2D Skynade::calculateViewAngles(const Vector& vec) {
    float yaw;
    float pitch;

    if (vec.y == 0.0f && vec.x == 0.0f) {
        yaw = 0.0f;
        if (vec.z > 0.0f) {
            pitch = 270.0f;
        }
        else {
            pitch = 90.0f;
        }
    }
    else {
        yaw = std::atan2(vec.y, vec.x) * 180.0f / M_PI;
        float tmp = std::sqrt(vec.x * vec.x + vec.y * vec.y);
        pitch = std::atan2(-vec.z, tmp) * 180.0f / M_PI;
    }

    return Vector2D(pitch, yaw);
}

// 主函数：计算手榴弹发射角度
std::optional<Vector2D> Skynade::calculateAngle(
    uint32_t weapon_id,
    uint32_t weapon_mod_bitfield,
    float weapon_projectile_scale,
    float weapon_projectile_speed,
    const Vector& local_view_origin,
    const Vector& target
) {
    bool lob;
    const Pitch* pitches;
    int pitches_size;
    float z_offset;

    // 根据武器类型和修改器选择相应的角度表和参数
    bool has_grenadier = (weapon_mod_bitfield & 0x4) != 0;

    if (weapon_id == WEAP_ID_THERMITE_GRENADE && !has_grenadier) {
        lob = false;
        pitches = GRENADE_PITCHES;
        pitches_size = GRENADE_PITCHES_SIZE;
        z_offset = 0.0f;
    }
    else if (weapon_id == WEAP_ID_FRAG_GRENADE && !has_grenadier) {
        lob = true;
        pitches = GRENADE_PITCHES;
        pitches_size = GRENADE_PITCHES_SIZE;
        z_offset = 70.0f;
    }
    else if (weapon_id == WEAP_ID_ARC_STAR && !has_grenadier) {
        lob = false;
        pitches = ARC_PITCHES;
        pitches_size = ARC_PITCHES_SIZE;
        z_offset = 25.0f;
    }
    else if (weapon_id == WEAP_ID_THERMITE_GRENADE && has_grenadier) {
        lob = false;
        pitches = GRENADIER_GRENADE_PITCHES;
        pitches_size = GRENADIER_GRENADE_PITCHES_SIZE;
        z_offset = 0.0f;
    }
    else if (weapon_id == WEAP_ID_FRAG_GRENADE && has_grenadier) {
        lob = true;
        pitches = GRENADIER_GRENADE_PITCHES;
        pitches_size = GRENADIER_GRENADE_PITCHES_SIZE;
        z_offset = 70.0f;
    }
    else if (weapon_id == WEAP_ID_ARC_STAR && has_grenadier) {
        lob = true;
        pitches = GRENADIER_ARC_PITCHES;
        pitches_size = GRENADIER_ARC_PITCHES_SIZE;
        z_offset = 25.0f;
    }
    else {
        return std::nullopt;
    }

    // 重力和初速度
    float g = 750.0f * weapon_projectile_scale;
    float v0 = weapon_projectile_speed;

    // 计算目标和玩家之间的向量
    Vector delta = target - local_view_origin;

    // 将delta向量扩展20个单位
    float delta_length = delta.Length();
    if (delta_length < 0.001f) return std::nullopt;
    Vector adjusted_delta = delta + (delta * (20.0f / delta_length));

    // 计算水平距离和垂直偏移
    float dx = std::sqrt(adjusted_delta.x * adjusted_delta.x + adjusted_delta.y * adjusted_delta.y);
    float dy = adjusted_delta.z + z_offset;

    // 根据是否需要抛物线路径选择合适的角度计算方法
    std::optional<float> launch_pitch;
    if (lob) {
        launch_pitch = lobAngle(dx, dy, v0, g);
    }
    else {
        launch_pitch = optimalAngle(dx, dy, v0, g);
    }

    // 如果能计算出角度，转换为视角角度并返回
    if (launch_pitch) {
        float view_pitch = launch2view(pitches, pitches_size, *launch_pitch);

        // 获取视角偏航角（度数）
        Vector2D angles = calculateViewAngles(delta);

        return Vector2D(view_pitch, angles.y);
    }

    return std::nullopt;
}
