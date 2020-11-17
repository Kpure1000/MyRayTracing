#ifndef INTERSECTIONHIT_H
#define INTERSECTIONHIT_H
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;

namespace ry
{
	class IntersectionHit : public Hitable
	{
	public:

		IntersectionHit(Hitable* a, Hitable* b, Material* mat)
			:Hitable(new SdfIntersection(a->sdf, b->sdf), mat),
			hA(a), hB(b)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (material != nullptr)
			{
				rec.mat = material;
			}
			return sdf->Hit(r, tMin, tMax, rec);
		}
		
		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

		Hitable* hA, * hB;

	};
}
#endif // !INTERSECTIONHIT_H
