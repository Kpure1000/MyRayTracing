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

		UnionHit(Sdf* a, Sdf* b)
			:sdfA(a), sdfB(b), sdf(a, b)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{

		}

		SdfUnion sdf;

		Sdf* sdfA;
		Sdf* sdfB;

		Material* matA;
		Material* matB;
	};
}
#endif // !UNIONHIT_H
