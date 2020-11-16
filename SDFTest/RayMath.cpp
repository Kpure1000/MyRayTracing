#include"RayMath.h"

Vector2f randomUnitVector()
{
	Vector2f re;
	do
	{
		re = 2.0f * Vector2f(rand() % 100 / (float)100,
			rand() % 100 / (float)100) - Vector2f(1, 1);
	} while (Magnitude(re) > 1.0f);
	return re;
}

Vector2f Reflect(const Vector2f& in, const Vector2f& normal)
{
	return in - 2 * Dot(in, normal) * normal;
}

/*
* 折射
* in 入射光
* normal 法线
* refRate 相对折射率（入/出）
* refracted 出射光
*/
bool Refract(const Vector2f& in, const Vector2f& normal, const float& refRate, Vector2f& refracted)
{
	Vector2f uvin = Normalize(in);
	float dt = Dot(uvin, normal);
	float discrimination = 1 - refRate * refRate * (1 - dt * dt);
	if (discrimination > 0)
	{
		refracted = refRate * (uvin - normal * dt) - normal * sqrt(discrimination);
		return true;
	}
	return false;
}

float Schlick(const float& cosine, const float& refRate)
{
	float r0 = (1 - refRate) / (1 + refRate);
	r0 *= r0;
	return r0 + (1 - r0) * powf((1 - cosine), 5);
}

double Drand48()
{
	seed = (RAND_A * seed + RAND_C) & 0xffffffffffffLL;
	unsigned int x = seed >> 16;
	return  ((double)x / (double)RAND_M);
}

void Srand48(unsigned int i)
{
	srand(i);
	seed = (((long long int)i) << 16) | rand();
}

float Distance(Vector2f const& a, Vector2f const& b)
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

float Magnitude(Vector2f const& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

float Dot(Vector2f const& a, Vector2f const& b)
{
	return a.x * b.x + a.y * b.y;
}

Vector2f Normalize(Vector2f const& v)
{
	return v / Magnitude(v);
}

