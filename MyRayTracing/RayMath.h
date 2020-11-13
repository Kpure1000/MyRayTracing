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

Vector3 Reflect(const Vector3& v, const Vector3& n)
{
	return v - 2 * Vector3::Dot(v, n) * n;
}
#endif // !RAYMATH_H

#ifndef MAX_FLOAT
#define MAX_FLOAT 1e3f
#endif // !MAX_FLOAT

#ifndef TO_RGB
#define TO_RGB 255.99

#endif // !_TO_RGB
