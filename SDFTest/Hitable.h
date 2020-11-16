#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
#include"Ray.h"

namespace ry
{

	class Material;

	struct HitRecord
	{
	public:
		float t;
		Vector2f hitPoint;
		Vector2f normal;
		Material* mat;
	};

	class Hitable : public Drawable
	{
	public:
		Hitable() {}
		Hitable(Material* mat) :material(mat) {}

		//返回射线碰撞数据record,射线被约束在tMin和tMax之间
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const = 0;

		Material* material;

	};

	

	

}