#ifndef MATERIAL_H
#define MATERIAL_H
#include"Ray.h"
#include"RayMath.h"
#include"Hitable.h"
#include"Texture.h"
#include<iostream>

namespace ry
{
	/*����*/
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

	/*���������������*/
	class Lambertian : public Material
	{
	public:

		Lambertian(Texture* t) :albedo(t), normalTexture(nullptr) {}

		Lambertian(Texture* t, Texture* NormalT)
			:albedo(t), normalTexture(NormalT) {}

		/*
		* ������������
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:ɢ�����
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 target;
			if (normalTexture != nullptr) // ������ͼ
			{
				target = rec.hitPoint + normalTexture->Value(rec.u, rec.v, rec.hitPoint) + RayMath::random_in_unit_ball();
			}
			else //  ��ͨ��ͼ
			{
				target = rec.hitPoint + rec.normal + RayMath::random_in_unit_ball();
			}
			scattered = Ray(rec.hitPoint, target - rec.hitPoint);
			attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
			return true;
		}

		//����
		Texture* albedo;

		//������ͼ����
		Texture* normalTexture;
	};

	/*���淴�����*/
	class Metal : public Material
	{
	public:

		Metal(Texture* t, const float& Fuzz)
			:albedo(t), fuzz(abs(Fuzz)) {
			fuzz = fuzz < 1 ? fuzz : 1;
		}

		/*
		* ���淴��
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:�������
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 reflected = RayMath::Reflect(rayIn.Direction(), rec.normal);
			scattered = Ray(rec.hitPoint, reflected + fuzz * RayMath::randomUnitVector());
			attenuation = albedo->Value(rec.u, rec.v, rec.hitPoint);
			return (Vector3::Dot(scattered.Direction(), rec.normal) > 0);
		}

		//����
		Texture* albedo;

		//�ֲ���
		float fuzz;
	};

	/*͸�����*/
	class Dielectric : public Material
	{
	public:

		Dielectric(Texture* t, const float& Refractive_Indices) :albedo(t), refractive_Indices(Refractive_Indices) {}

		/*
		* ����
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:�������
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

			// �ж��Ƿ�ȫ����
			scattered = Ray(rec.hitPoint, isRefracted == true ? refracted : reflected);
			
			return true;
		}

		//����
		Texture* albedo;

		//������
		float refractive_Indices;
	};

	/*�Է�����ʣ�����Դ����պ���*/
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

		//����
		Texture* albedo;

		float intensity;
	};



}
#endif