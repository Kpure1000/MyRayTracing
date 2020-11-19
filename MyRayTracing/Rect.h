#ifndef RECT_H
#define RECT_H
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{

	class Rect : public Hitable
	{
	public:

		Rect() {}

		Rect(SdfRect* rect)
			: Hitable(rect)
		{
		}

		Rect(SdfRect* rect, Material* mat)
			: Hitable(rect, mat)
		{
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			SdfRecord sdfR;
			if (material != nullptr)
			{
				rec.mat = material;
			}
			return sdf->Hit(r, tMin, tMax, rec, sdfR);
		}

		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			return sdf->GetBBox(t0, t1, box);
		}

	};

}

#endif // !RECT_H