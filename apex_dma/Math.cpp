#include "Math.h"

void Math::NormalizeAngles(QAngle& angle)
{
	while (angle.x > 89.0f)
		angle.x -= 180.f;

	while (angle.x < -89.0f)
		angle.x += 180.f;

	while (angle.y > 180.f)
		angle.y -= 360.f;

	while (angle.y < -180.f)
		angle.y += 360.f;
}

QAngle Math::CalcAngle(const Vector& src, const Vector& dst)
{
	QAngle angle = QAngle();
	Vector delta = src - dst;

	double hyp = delta.Length2D();

	angle.x = RAD2DEG(atan2(delta.z, hyp));
	angle.y = RAD2DEG(atan2(-delta.y, -delta.x));
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