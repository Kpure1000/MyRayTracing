#ifndef MATERIAL_H
#define MATERIAL_H
#include"Ray.h"
#include"Hitable.h"
namespace ry
{
	class Material
	{
	public:
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const = 0;
	};

	// Lambertian Diffuse
	class Lambertian : public Material
	{
	public:

		

		/*
		* ������������
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:ɢ�����
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 target = rec.vertex + rec.normal + random_in_unit_ball();
			scattered = Ray(rec.vertex, target - rec.vertex);
			attenuation = albedo;
			return true;
		}

		//������
		Vector3 albedo;
	};

	class Metal : public Material
	{
	public: 

		Metal(const Vector3& Albedo) :albedo(Albedo) {}

		/*
		* ���淴��
		* rayIn:������, rec:������ײ��¼, attenuation:������, scattered:ɢ�����
		*/
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Vector3& attenuation, Ray& scattered)const
		{
			Vector3 reflected = Reflect(rayIn.Direction().Normalize(), rec.normal);
			scattered = Ray(rec.vertex, reflected);
			attenuation = albedo;
			return (Vector3::Dot(scattered.Direction(), rec.normal) > 0);
		}

		//������
		Vector3 albedo;
	};

}
#endif