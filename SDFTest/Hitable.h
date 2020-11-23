#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
#include"Ray.h"
namespace sdf {
	class Sdf;
}
namespace ry
{

	class Material;

	struct HitRecord
	{
	public:
		float t = 0.0f;
		Vector2f hitPoint;
		Vector2f normal;
		Material* mat = nullptr;
		bool isInRange = false;
	};

	class Hitable : public Drawable
	{
	public:
		
		Hitable() :sdf(NULL), material(NULL) {}
		
		Hitable(sdf::Sdf* sdf) :sdf(sdf), material(NULL) {}
		
		Hitable(Material* mat) :sdf(NULL), material(mat) {}

		Hitable(sdf::Sdf* sdf, Material* mat) :sdf(sdf), material(mat) {}

		//����������ײ����record,���߱�Լ����tMin��tMax֮��
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec) = 0;

		virtual void SetMaterial(Material* mat) = 0;

		Material* material;

		sdf::Sdf* sdf;

	};





}