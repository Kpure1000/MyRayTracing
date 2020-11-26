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

		Sphere(SdfSphere* sphere)
			: Hitable(sphere), circle(sphere->radius, sphere->center,Color::White)
		{
		}

		Sphere(SdfSphere* sphere, Material* mat)
			: Hitable(sphere,mat),
			circle(sphere->radius, sphere->center, mat->color)
		{
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			HitRecord far;
			if (material != nullptr)
			{
				rec.mat = material;
			}
			SdfRecord sdfR;
			return sdf->Hit(r, tMin, tMax, rec,far, sdfR);
		}

		virtual bool GetBBox(AABB& box)const
		{
			return sdf->GetBBox(box);
		}

		virtual void SetMaterial(Material* mat)
		{
			material = mat;
			circle.SetColor(material->color);
		}

		MyCircle circle;

	private:

		virtual void draw(RenderTarget& target, RenderStates states)const
		{
			target.draw(circle, states);
		}
	};
}