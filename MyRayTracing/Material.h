#ifndef MATERIAL_H
#define MATERIAL_H
#include"Ray.h"
#include"Hitable.h"
#include<iostream>

namespace ry
{
	/*材质*/
	class Material
	{
	public:
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
			Vector3& attenuation, Ray& scattered)const = 0;
	};

	/*兰伯特漫反射材质*/
	class Lambertian : public Material
	{
	public:

		Lambertian(const Vector3 Albedo) :albedo(Albedo) {}

		/*
		* 兰伯特漫反射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:散射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 target = rec.hitPoint + rec.normal + random_in_unit_ball();
			scattered = Ray(rec.hitPoint, target - rec.hitPoint);
			attenuation = albedo;
			return true;
		}

		//反射率
		Vector3 albedo;
	};

	/*镜面反射材质*/
	class Metal : public Material
	{
	public:

		Metal(const Vector3& Albedo, const float& Fuzz)
			:albedo(Albedo), fuzz(abs(Fuzz)) {
			fuzz = fuzz < 1 ? fuzz : 1;
		}

		/*
		* 镜面反射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:出射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 reflected = Reflect(rayIn.Direction(), rec.normal);
			scattered = Ray(rec.hitPoint, reflected + fuzz * randomUnitVector());
			attenuation = albedo;
			return (Vector3::Dot(scattered.Direction(), rec.normal) > 0);
		}

		//反射率
		Vector3 albedo;

		//粗糙率
		float fuzz;
	};

	/*透光材质*/
	class Dielectric : public Material
	{
	public:

		Dielectric(const Vector3 &Albedo, const float& Refractive_Indices) :albedo(Albedo), refractive_Indices(Refractive_Indices) {}

		/*
		* 折射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:出射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 refracted;
			Vector3 reflected = Reflect(rayIn.Direction(), rec.normal);
			attenuation = albedo;

			float cosine;

			bool isRefracted;

			if (Vector3::Dot(rayIn.Direction(), rec.normal) > 0)
			{
				isRefracted = Refract(rayIn.Direction(), -rec.normal, refractive_Indices, refracted);
				cosine = refractive_Indices * Vector3::Dot(Vector3::Normalize(rayIn.Direction()), rec.normal);
			}
			else 
			{
				isRefracted = Refract(rayIn.Direction(), rec.normal, 1.0f / refractive_Indices, refracted);
				cosine = -Vector3::Dot(Vector3::Normalize(rayIn.Direction()), rec.normal);
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

		//反射率
		Vector3 albedo;

		//折射率
		float refractive_Indices;
	};

}
#endif