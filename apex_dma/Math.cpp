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
	SVector delta = SVector((src.x - dst.x), (src.y - dst.y), (src.z - dst.z));

	double hyp = sqrt(delta.x*delta.x + delta.y * delta.y);

	angle.x = atan(delta.z / hyp) * (180.0f / M_PI);
	angle.y = atan(delta.y / delta.x) * (180.0f / M_PI);
	angle.z = 0;
	if (delta.x >= 0.0) angle.y += 180.0f;

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

bool Math::IsLineIntersectingSphere(const Vector& start, const Vector& end, const Vector& sphereCenter, float radius)
{
	const float CHECK_RADIUS = radius + 10.0f;
	const float INTERSECT_RADIUS = radius;

	auto getDistanceSqr = [](const Vector& point, const Vector& center) {
		return (point.x - center.x) * (point.x - center.x)
			+ (point.y - center.y) * (point.y - center.y)
			+ (point.z - center.z) * (point.z - center.z);
	};

	float startDistSqr = getDistanceSqr(start, sphereCenter);
	float endDistSqr = getDistanceSqr(end, sphereCenter);
	float checkRadiusSqr = CHECK_RADIUS * CHECK_RADIUS;

	// If one point is inside and the other is outside, the line must intersect the sphere surface
	if ((startDistSqr <= checkRadiusSqr) ^ (endDistSqr <= checkRadiusSqr)) {
		return true;
	}

	// If both points are inside, they can see each other through the dome
	if (startDistSqr <= checkRadiusSqr && endDistSqr <= checkRadiusSqr) {
		return false;
	}

	// Ray-Sphere intersection for both points outside
	Vector dir = {
		end.x - start.x,
		end.y - start.y,
		end.z - start.z
	};

	Vector oc = {
		start.x - sphereCenter.x,
		start.y - sphereCenter.y,
		start.z - sphereCenter.z
	};

	float a = dir.x * dir.x + dir.y * dir.y + dir.z * dir.z;
	float b = 2.0f * (oc.x * dir.x + oc.y * dir.y + oc.z * dir.z);
	float c = (oc.x * oc.x + oc.y * oc.y + oc.z * oc.z) - (INTERSECT_RADIUS * INTERSECT_RADIUS);

	float discriminant = b * b - 4 * a * c;

	if (discriminant < 0) {
		return false;
	}

	float t1 = (-b - sqrt(discriminant)) / (2 * a);
	float t2 = (-b + sqrt(discriminant)) / (2 * a);

	return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}