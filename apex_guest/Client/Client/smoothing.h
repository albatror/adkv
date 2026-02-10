#pragma once
#include <cmath>
#include <algorithm>
#include <chrono>
#include <random>

using namespace std;

float clamp(float value, float minimum, float maximum) {
    return std::min(std::max(value, minimum), maximum);
}

float SmoothDamp(float current, float target, float& velocity, float smoothTime, float deltaTime, float maxSpeed = INFINITY)
{
    float omega = 2.0f / smoothTime;
    float x = omega * deltaTime;
    float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
    float change = current - target;
    float originalTo = target;

    float maxChange = maxSpeed * smoothTime;
    change = clamp(change, -maxChange, maxChange);
    target = current - change;

    float temp = (velocity + omega * change) * deltaTime;
    velocity = (velocity - omega * temp) * exp;
    float output = target + (change + temp) * exp;

    if ((originalTo - current > 0.0f) == (output > originalTo))
    {
        output = originalTo;
        velocity = (output - originalTo) / deltaTime;
    }

    return output;
}

// Fonction sigmoid pour l'interpolation non lin\xe9aire
float Sigmoid(float x, float steepness = 1.0f)
{
    return 1.0f / (1.0f + exp(-steepness * x));
}