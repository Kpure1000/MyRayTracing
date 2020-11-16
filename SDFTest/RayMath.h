#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;

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

double Drand48();

void Srand48(unsigned int i);

Vector2f randomUnitVector();
Vector2f Reflect(const Vector2f& in, const Vector2f& normal);
bool Refract(const Vector2f& in, const Vector2f& normal,
	const float& refRate, Vector2f& refracted);
float Schlick(const float& cosine, const float& refRate);


float Distance(Vector2f const& a, Vector2f const& b);

float Magnitude(Vector2f const& v);

float Dot(Vector2f const& a, Vector2f const& b);

Vector2f Normalize(Vector2f const& v);

