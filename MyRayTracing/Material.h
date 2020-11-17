#ifndef MATERIAL_H
#define MATERIAL_H
#include"Ray.h"
#include"Hitable.h"
#include<iostream>

namespace ry
{
	/*����*/
	class Material
	{
	public:
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec,
			Vector3& attenuation, Ray& scattered)const = 0;
	};

	/*���������������*/
	class Lambertian : public Material
	{
	public:

		Lambertian(const Vector3 Albedo) :albedo(Albedo) {}

		/*
		* ������������
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:ɢ�����
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 target = rec.hitPoint + rec.normal + random_in_unit_ball();
			scattered = Ray(rec.hitPoint, target - rec.hitPoint);
			attenuation = albedo;
			return true;
		}

		//������
		Vector3 albedo;
	};

	/*���淴�����*/
	class Metal : public Material
	{
	public:

		Metal(const Vector3& Albedo, const float& Fuzz)
			:albedo(Albedo), fuzz(abs(Fuzz)) {
			fuzz = fuzz < 1 ? fuzz : 1;
		}

		/*
		* ���淴��
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:�������
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 reflected = Reflect(rayIn.Direction(), rec.normal);
			scattered = Ray(rec.hitPoint, reflected + fuzz * randomUnitVector());
			attenuation = albedo;
			return (Vector3::Dot(scattered.Direction(), rec.normal) > 0);
		}

		//������
		Vector3 albedo;

		//�ֲ���
		float fuzz;
	};

	/*͸�����*/
	class Dielectric : public Material
	{
	public:

		Dielectric(const Vector3 &Albedo, const float& Refractive_Indices) :albedo(Albedo), refractive_Indices(Refractive_Indices) {}

		/*
		* ����
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:�������
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

			// �ж��Ƿ�ȫ����
			scattered = Ray(rec.hitPoint, isRefracted == true ? refracted : reflected);
			
			return true;
		}

		//������
		Vector3 albedo;

		//������
		float refractive_Indices;
	};

}
#endif