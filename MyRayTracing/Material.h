#ifndef MATERIAL_H
#define MATERIAL_H
#include"Ray.h"
#include"Hitable.h"

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
			Vector3 reflected = Reflect(rayIn.Direction().Normalize(), rec.normal);
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

		Dielectric(const float& Refractive_Indices) : refractive_Indices(Refractive_Indices) {}

		/*
		* 折射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:出射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 refracted;
			Vector3 rayDir = rayIn.Direction().Normalize();
			if (Vector3::Dot(rayDir, rec.normal) > 0)
			{
				refracted = Refract(rayIn.Direction().Normalize(), -rec.normal, refractive_Indices);
			}
			else 
			{
				refracted = Refract(rayIn.Direction().Normalize(), rec.normal, 1.0f / refractive_Indices);
			}
			scattered = Ray(rec.hitPoint, refracted - rec.hitPoint);
			attenuation = Vector3::One;
			return true;
		}


		//折射率
		float refractive_Indices;
	};

}
#endif