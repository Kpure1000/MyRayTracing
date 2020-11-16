#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;

Vector2f randomUnitVector();
Vector2f Reflect(const Vector2f& in, const Vector2f& normal);
bool Refract(const Vector2f& in, const Vector2f& normal,
	const float& refRate, Vector2f& refracted);
float Schlick(const float& cosine, const float& refRate);
inline float Distance(Vector2f const& a, Vector2f const& b);
inline float Magnitude(Vector2f const& v);
inline float Dot(Vector2f const& a, Vector2f const& b);
inline Vector2f Normalize(Vector2f const& v);


inline float Distance(Vector2f const& a, Vector2f const& b)
{
	return sqrtf((a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y));
}

inline float Magnitude(Vector2f const& v)
{
	return sqrtf(v.x * v.x + v.y * v.y);
}

inline float Dot(Vector2f const& a, Vector2f const& b)
{
	return a.x * b.x + a.y * b.y;
}

inline Vector2f Normalize(Vector2f const& v)
{
	return v / Magnitude(v);
}

static unsigned long long seed = 1;

#ifndef MAX_FLOAT
#define MAX_FLOAT 1e10f
#endif // !MAX_FLOAT


#ifndef RAND_M
#define RAND_M 0x100000000LL  
#endif // !RAND_M
#ifndef RAND_C
#define RAND_C 0xB16  
#endif // !RAND_C
#ifndef RAND_A
#define RAND_A 0x5DEECE66DLL 
#endif // !RAND_A

inline double Drand48()
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
