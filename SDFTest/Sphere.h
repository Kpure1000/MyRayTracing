#pragma once
#include"Hitable.h"
#include"Sdf.h"
#include"MyCircle.h"
using namespace sdf;
namespace ry
{
	class Sphere : public Hitable
	{
	public:
		Sphere() {}

		Sphere(SdfSphere sphere)
			: sdfSphere(sphere), circle(sphere.radius, sphere.center,Color::White)
		{
		}

		Sphere(SdfSphere sphere, Material* mat)
			: sdfSphere(sphere), Hitable(mat),
			circle(sphere.radius, sphere.center, mat->color)
		{
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (material != nullptr)
			{
				rec.mat = material;
			}
			return sdfSphere.Hit(r, tMin, tMax, rec);
		}

		SdfSphere sdfSphere;

		MyCircle circle;

	private:

		virtual void draw(RenderTarget& target, RenderStates states)const
		{
			target.draw(circle, states);
		}
	};
}