#ifndef UNIONHIT_H
#define UNIONHIT_H
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
	class UnionHit :
		public Hitable
	{
	public:

		UnionHit(Hitable* a, Hitable* b, Material* mat)
			:Hitable(new SdfUnion(a->sdf, b->sdf), mat)
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

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			return true;
		}

		Hitable* hits[2];
	};
}
#endif // !UNIONHIT_H
