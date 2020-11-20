#ifndef MATERIAL_H
#define MATERIAL_H
#include"Ray.h"
#include"RayMath.h"
#include"Hitable.h"
#include"Texture.h"
#include<iostream>

namespace ry
{
	/*材质*/
	class Material
	{
	public:
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
			Vector3& attenuation, Ray& scattered)const = 0;

		virtual Vector3 Emitted(const float& u, const float& v, const Vector3& pos)const
		{
			return { 0,0,0 };
		}
	};

	/*兰伯特漫反射材质*/
	class Lambertian : public Material
	{
	public:

		Lambertian(Texture* t) :albedo(t), normalTexture(nullptr) {}

		Lambertian(Texture* t, Texture* NormalT)
			:albedo(t), normalTexture(NormalT) {}

		/*
		* 兰伯特漫反射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:散射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 target;
			if (normalTexture != nullptr) // 法线贴图
			{
				target = rec.hitPoint + normalTexture->Value(rec.u, rec.v, rec.hitPoint) + RayMath::random_in_unit_ball();
			}
			else //  普通贴图
			{
				target = rec.hitPoint + rec.normal + RayMath::random_in_unit_ball();
			}
			scattered = Ray(rec.hitPoint, target - rec.hitPoint);
			attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
			return true;
		}

		//纹理
		Texture* albedo;

		//法线贴图纹理
		Texture* normalTexture;
	};

	/*镜面反射材质*/
	class Metal : public Material
	{
	public:

		Metal(Texture* t, const float& Fuzz)
			:albedo(t), fuzz(abs(Fuzz)) {
			fuzz = fuzz < 1 ? fuzz : 1;
		}

		/*
		* 镜面反射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:出射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 reflected = RayMath::Reflect(rayIn.Direction(), rec.normal);
			scattered = Ray(rec.hitPoint, reflected + fuzz * RayMath::randomUnitVector());
			attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
			return (Vector3::Dot(scattered.Direction(), rec.normal) > 0);
		}

		//纹理
		Texture* albedo;

		//粗糙率
		float fuzz;
	};

	/*透光材质*/
	class Dielectric : public Material
	{
	public:

		Dielectric(Texture* t, const float& Refractive_Indices) :albedo(t), refractive_Indices(Refractive_Indices) {}

		/*
		* 折射
		* rayIn:入射线, rec:射线碰撞记录, attenuation:反射率, scattered:出射光线
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 refracted;
			Vector3 reflected = RayMath::Reflect(rayIn.Direction(), rec.normal);
			attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);

			float cosine;

			bool isRefracted;

			if (Vector3::Dot(rayIn.Direction(), rec.normal) > 0)
			{
				isRefracted = RayMath::Refract(rayIn.Direction(), -rec.normal, refractive_Indices, refracted);
				cosine = refractive_Indices * Vector3::Dot(Vector3::Normalize(rayIn.Direction()), rec.normal);
			}
			else 
			{
				isRefracted = RayMath::Refract(rayIn.Direction(), rec.normal, 1.0f / refractive_Indices, refracted);
				cosine = -Vector3::Dot(Vector3::Normalize(rayIn.Direction()), rec.normal);
			}

			if (isRefracted)
			{
				if (RayMath::Drand48() <= RayMath::Schlick(cosine, refractive_Indices))
				{
					isRefracted = false;
				}

			}

			// 判断是否全反射
			scattered = Ray(rec.hitPoint, isRefracted == true ? refracted : reflected);
			
			return true;
		}

		//纹理
		Texture* albedo;

		//折射率
		float refractive_Indices;
	};

	/*自发光材质，给光源和天空盒用*/
	class Illumination : public Material
	{
	public:

		Illumination(Texture* t, const float& Intensity)
			:albedo(t), intensity(Intensity) {}

		
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			/*attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint) * std::max(0.0f, std::min(1.0f, intensity));*/
			return false;
		}

		virtual Vector3 Emitted(const float& u, const float& v, const Vector3& pos)const
		{
			return albedo->Value(u, v, pos) * std::max(0.0f, std::min(1.0f, intensity));

		}

		//纹理
		Texture* albedo;

		float intensity;
	};



}
#endif