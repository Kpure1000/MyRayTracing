#ifndef MAX_FLOAT
#define MAX_FLOAT 1e30f
#endif // !MAX_FLOAT

#ifndef TO_RGB
#define TO_RGB 255.99f
#endif // !_TO_RGB

#ifndef RAND_M
#define RAND_M 0x100000000LL  
#endif // !RAND_M
#ifndef RAND_C
#define RAND_C 0xB16  
#endif // !RAND_C
#ifndef RAND_A
#define RAND_A 0x5DEECE66DLL 
#endif // !RAND_A

#ifndef Pi
#define Pi (acosf(-1.0f))
#endif // !Pi


#ifndef RAYMATH_H
#define RAYMATH_H
#include"Vector3.h"
#include<iostream>

using namespace ry;

class RayMath
{
public:

	static inline double Drand48()
	{
		seed = (RAND_A * seed + RAND_C) & 0xffffffffffffLL;
		unsigned int x = seed >> 16;
		return  ((double)x / (double)RAND_M);
	}

	static inline void Srand48(unsigned int i)
	{
		srand(i);
		seed = (((long long int)i) << 16) | rand();
	}

	static inline Vector3 randomUnitVector()
	{
		Vector3 re;
		do
		{
			re = 2.0f * Vector3(Drand48(), Drand48(), Drand48()) - Vector3::One;
		} while (re.Magnitude() > 1.0f);
		return re;
	}

	static inline Vector3 random_in_unit_ball()
	{
		Vector3 re;
		do
		{
			re = 2.0f * Vector3(Drand48(), Drand48(), Drand48()) - Vector3::One;
		} while (re.Magnitude() > 1.0f);
		return re;
	}

	static inline Vector3 Reflect(const Vector3& in, const Vector3& normal)
	{
		return in - 2 * Vector3::Dot(in, normal) * normal;
	}

	static bool Refract(const Vector3& in, const Vector3& normal, const float& refRate, Vector3& refracted)
	{
		Vector3 uvin = Vector3::Normalize(in);
		float dt = Vector3::Dot(uvin, normal);
		float discrimination = 1 - refRate * refRate * (1 - dt * dt);
		if (discrimination > 0)
		{
			refracted = refRate * (uvin - normal * dt) - normal * sqrt(discrimination);
			return true;
		}
		return false;
	}

	static inline float Schlick(const float& cosine, const float& refRate)
	{
		float r0 = (1 - refRate) / (1 + refRate);
		r0 *= r0;
		return r0 + (1 - r0) * powf((1 - cosine), 5);
	}

private:

	static unsigned long long seed;

};


#endif // !RAYMATH_H
