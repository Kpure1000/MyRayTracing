#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
#include"Ray.h"
#include"Hitable.h"
#include"RayMath.h"

/*材质*/
class Material
{
public:
	virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const = 0;
};

//直接穿过去的材质
class Simple : public Material
{
	virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const
	{
		scattered = rayIn;
	}
};

/*透光材质*/
class Dielectric : public Material
{
public:

	Dielectric(const Vector2f& Albedo, const float& Refractive_Indices) : refractive_Indices(Refractive_Indices) {}

	/*
	* 折射
	* rayIn:入射线, rec:射线碰撞记录, attenuation, scattered:出射光线
	*/
	virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const
	{
		Vector2f refracted;
		Vector2f reflected = Reflect(rayIn.Direction(), rec.normal);

		float cosine;

		bool isRefracted;

		if (Dot(rayIn.Direction(), rec.normal) > 0)
		{
			isRefracted = Refract(rayIn.Direction(), -rec.normal, refractive_Indices, refracted);
			cosine = refractive_Indices * Dot(Normalize(rayIn.Direction()), rec.normal);
		}
		else
		{
			isRefracted = Refract(rayIn.Direction(), rec.normal, 1.0f / refractive_Indices, refracted);
			cosine = -Dot(Normalize(rayIn.Direction()), rec.normal);
		}

		if (isRefracted)
		{
			if (Drand48() <= Schlick(cosine, refractive_Indices))
				//if (rand() % 100 / (float)100 <= Schlick(cosine, refractive_Indices))
			{
				isRefracted = false;
			}

		}

		// 判断是否全反射
		scattered = Ray(rec.hitPoint, isRefracted == true ? refracted : reflected);

		return true;
	}


	//折射率
	float refractive_Indices;
};

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
