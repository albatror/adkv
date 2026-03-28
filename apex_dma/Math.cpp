#include "Math.h"

void Math::NormalizeAngles(QAngle& angle)
{
	if (angle.x > 89.0f)
		angle.x = 89.0f;
	if (angle.x < -89.0f)
		angle.x = -89.0f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;

	angle.z = 0;
}

QAngle Math::CalcAngle(const Vector& src, const Vector& dst)
{
	QAngle angle = QAngle();
	Vector delta = dst - src;

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);

	angle.x = atan2(-delta.z, hyp) * (180.0f / M_PI);
	angle.y = atan2(delta.y, delta.x) * (180.0f / M_PI);
	angle.z = 0;

	return angle;
}

double Math::GetFov(const QAngle& viewAngle, const QAngle& aimAngle)
{
	QAngle delta = aimAngle - viewAngle;
	NormalizeAngles(delta);

	return sqrt(pow(delta.x, 2.0f) + pow(delta.y, 2.0f));
}

double Math::DotProduct(const Vector& v1, const float* v2)
{
	return v1.x * v2[0] + v1.y * v2[1] + v1.z * v2[2];
}

float Math::SmoothStep(float edge0, float edge1, float x)
{
	// Scale, bias and saturate x to 0..1 range
	x = (x - edge0) / (edge1 - edge0);
	x = x < 0 ? 0 : (x > 1 ? 1 : x);
	// Evaluate polynomial
	return x * x * (3 - 2 * x);
}