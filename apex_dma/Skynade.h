#pragma once
#include "vector.h"
#include <optional>
#include <cstdint>

class Skynade {
public:
    // 主函数：计算投射物的发射角度
    std::optional<Vector2D> calculateAngle(
        uint32_t weapon_id,
        uint32_t weapon_mod_bitfield,
        float weapon_projectile_scale,
        float weapon_projectile_speed,
        const Vector& local_view_origin,
        const Vector& target
    );

    // 投射物类型ID常量
    static constexpr uint32_t WEAP_ID_THERMITE_GRENADE = 125;
    static constexpr uint32_t WEAP_ID_FRAG_GRENADE = 123;
    static constexpr uint32_t WEAP_ID_ARC_STAR = 124;

private:
    struct Pitch {
        float view;
        float launch;
    };

    // 角度查找表
    static const Pitch GRENADE_PITCHES[];
    static const int GRENADE_PITCHES_SIZE;
    static const Pitch ARC_PITCHES[];
    static const int ARC_PITCHES_SIZE;
    static const Pitch GRENADIER_GRENADE_PITCHES[];
    static const int GRENADIER_GRENADE_PITCHES_SIZE;
    static const Pitch GRENADIER_ARC_PITCHES[];
    static const int GRENADIER_ARC_PITCHES_SIZE;

    // 辅助函数
    float launch2view(const Pitch* pitches, int size, float launch);
    std::optional<float> optimalAngle(float x, float y, float v0, float g);
    std::optional<float> lobAngle(float x, float y, float v0, float g);
    Vector2D calculateViewAngles(const Vector& vec);
};
