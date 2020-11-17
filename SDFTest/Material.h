#pragma once
#include<SFML/Graphics.hpp>
#include"Hitable.h"
#include"RayMath.h"
using namespace sf;

namespace ry
{

	/*����*/
	class Material
	{
	public:
		Material(Color c) :color(c) {}
		virtual bool Scatter(const Ray& rayIn, const HitRecord& rec, Ray& scattered)const = 0;
		Color color;
	};

	//ֱ�Ӵ���ȥ�Ĳ���
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

	//����
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

	/*͸�����*/
	class Dielectric : public Material
	{
	public:
		Dielectric(const float& Refractive_Indices) 
			: refractive_Indices(Refractive_Indices),Material(Color::Green) {}

		/*
		* ����
		* rayIn:������, rec:������ײ��¼, attenuation, scattered:�������
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

			// �ж��Ƿ�ȫ����
			scattered = Ray(rec.hitPoint, isRefracted == true ? refracted : reflected);
			return true;
		}


		//������
		float refractive_Indices;
	};

}