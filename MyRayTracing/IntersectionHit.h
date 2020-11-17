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
			SdfRecord sdfR;
			auto result = sdf->Hit(r, tMin, tMax, rec, sdfR);
			if (result && sdfR == SdfRecord::A)
			{
				if (hA->material)rec.mat = hA->material;
			}
			else if (result && sdfR == SdfRecord::B)
			{
				if (hB->material)rec.mat = hB->material;
			}
			else
			{
				if (material)rec.mat = material;
			}
			return result;
		}
		
		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

		Hitable* hA, * hB;

	};
}
#endif // !INTERSECTIONHIT_H
