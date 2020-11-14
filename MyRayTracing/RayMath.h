#ifndef RAYMATH_H
#define RAYMATH_H
#include"Vector3.h"
using namespace ry;

Vector3 randomUnitVector()
{
	Vector3 re;
	do
	{
		re = 2.0f * Vector3(rand() % 100 / (float)100,
			rand() % 100 / (float)100, rand() % 100 / (float)100) - Vector3::One;
	} while (re.Magnitude() > 1.0f);
	return re;
}

Vector3 Reflect(const Vector3& in, const Vector3& normal)
{
	return in - 2 * Vector3::Dot(in, normal) * normal;
}

Vector3 Refract(const Vector3& in, const Vector3& normal, const float& refRate)
{
	return (refRate - 1) * Vector3::Dot(in, normal) * normal - refRate * in;
}

#endif // !RAYMATH_H

#ifndef MAX_FLOAT
#define MAX_FLOAT 1e3f
#endif // !MAX_FLOAT

#ifndef TO_RGB
#define TO_RGB 255.5f

#endif // !_TO_RGB
