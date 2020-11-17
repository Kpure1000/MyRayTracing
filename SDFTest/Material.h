#pragma once
#include<SFML/Graphics.hpp>
#include"Hitable.h"
#include"RayMath.h"
using namespace sf;

namespace ry
{

	/*材质*/
	class Material
	{
	public:
		Material(Color c) :color(c) {}
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const = 0;
		Color color;
	};

	//直接穿过去的材质
	class Simple : public Material
	{
	public:
		Simple() :Material(Color::White) {}
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const
		{
			scattered = Ray(rec.hitPoint, rayIn.direction);
			return true;
		}
	};

	//反射
	class Metal : public Material
	{
	public:
		Metal() :Material(Color::Blue) {}
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const
		{
			Vector2f reflected = Reflect(rayIn.Direction(), rec.normal);
			scattered = Ray(rec.hitPoint, reflected);
			return true;
		}
	};

	/*透光材质*/
	class Dielectric : public Material
	{
	public:
		Dielectric(const float& Refractive_Indices) 
			: refractive_Indices(Refractive_Indices),Material(Color::Green) {}

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

			/*if (isRefracted)
			{
				if (Drand48() <= Schlick(cosine, refractive_Indices))
				{
					isRefracted = false;
				}
			}*/

			// 判断是否全反射
			scattered = Ray(rec.hitPoint, isRefracted == true ? refracted : reflected);
			return true;
		}


		//折射率
		float refractive_Indices;
	};

}