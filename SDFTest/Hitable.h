#pragma once
#include<SFML/Graphics.hpp>
#include"Ray.h"
#include"AABB.h"
using namespace sf;
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

		//返回射线碰撞数据record,射线被约束在tMin和tMax之间
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const = 0;

		virtual bool GetBBox(AABB& box)const
		{
			return false;
		}

		virtual void SetMaterial(Material* mat) {}

		Material* material;

		sdf::Sdf* sdf;

	};





}