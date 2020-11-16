#ifndef INTERSECTIONHIT_H
#define INTERSECTIONHIT_H
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;

namespace ry
{

	class IntersectionHit :
		public Hitable
	{
	public:
		IntersectionHit(Sdf* a, Sdf* b, Material* mat)
			: sdfA(a), sdfB(b),
			sdf(a, b),
			Hitable(mat)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (material != nullptr)
			{
				rec.mat = material;
			}
			return sdf.Hit(r, tMin, tMax, rec);
		}

		SdfIntersection sdf;

		Sdf* sdfA;
		Sdf* sdfB;

		/*Material* matA;
		Material* matB;*/
	};
}
#endif // !INTERSECTIONHIT_H
