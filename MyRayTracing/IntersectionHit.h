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
			:Hitable(new SdfIntersection(a->sdf, b->sdf), mat)
		{
			hits[0] = a;
			hits[1] = b;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			SdfRecord sdfR;
			auto result = sdf->Hit(r, tMin, tMax, rec, sdfR);
			if (result && sdfR == SdfRecord::A)
			{
				if (hits[0]->material)rec.mat = hits[0]->material;
			}
			else if (result && sdfR == SdfRecord::B)
			{
				if (hits[1]->material)rec.mat = hits[1]->material;
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

		Hitable* hits[2];

	};
}
#endif // !INTERSECTIONHIT_H
