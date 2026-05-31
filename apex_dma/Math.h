#pragma once
#include <math.h>
#include "vector.h"

struct SVector
{
	float x;
	float y;
	float z;
	SVector(float x1, float y1, float z1)
    {
		x = x1;
		y = y1;
		z = z1;
	}

	SVector(QAngle q)
    {
		x = q.x;
		y = q.y;
		z = q.z;
	}
};

namespace Math
{
	void NormalizeAngles(QAngle& angle);
	double GetFov(const QAngle& viewAngle, const QAngle& aimAngle);
	double DotProduct(const Vector& v1, const float* v2);
	QAngle CalcAngle(const Vector& src, const Vector& dst);
	float SmoothStep(float edge0, float edge1, float x);
}

inline float SmoothDamp(float current, float target, float& velocity, float smoothTime, float deltaTime, float maxSpeed = 1e9f)
{
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float expVal = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float originalTo = target;
    float maxChange = maxSpeed * smoothTime;
    if (change >  maxChange) change =  maxChange;
    if (change < -maxChange) change = -maxChange;
    target = current - change;
    float temp = (velocity + omega * change) * deltaTime;
    velocity = (velocity - omega * temp) * expVal;
    float output = target + (change + temp) * expVal;
    if ((originalTo - current > 0.0f) == (output > originalTo)) {
        output = originalTo;
        velocity = (output - originalTo) / deltaTime;
    }
    return output;
}

// Linear: constant fraction per frame — direct/raw (original pre-SmoothDamp behavior)
inline float AimLinear(float delta, float smoothing)
{
    float factor = 1.0f / (smoothing > 0.0f ? smoothing : 1.0f);
    return delta * factor;
}

// Bezier (quadratic ease-out): fast approach, gentle arrival at target
// B(t) = 2t - t² with t = 1/smoothing
inline float AimBezier(float delta, float smoothing)
{
    float t = 1.0f / (smoothing > 1.0f ? smoothing : 1.0f);
    if (t > 1.0f) t = 1.0f;
    float factor = 2.0f * t - t * t;
    return delta * factor;
}

// Cubic Bezier (ease-in-out): smooth accel + decel — most human-looking
// f(t) = t²(3 - 2t) — same curve as GLSL smoothstep
inline float AimCubicBezier(float delta, float smoothing)
{
    float t = 1.0f / (smoothing > 1.0f ? smoothing : 1.0f);
    if (t > 1.0f) t = 1.0f;
    float factor = t * t * (3.0f - 2.0f * t);
    return delta * factor;
}

// S-Curve (Smootherstep): zero 1st and 2nd derivatives at endpoints — ultra-smooth
// f(t) = 6t⁵ - 15t⁴ + 10t³  (Ken Perlin's improved smoothstep)
inline float AimSCurve(float delta, float smoothing)
{
    float t = 1.0f / (smoothing > 1.0f ? smoothing : 1.0f);
    if (t > 1.0f) t = 1.0f;
    float factor = t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
    return delta * factor;
}
