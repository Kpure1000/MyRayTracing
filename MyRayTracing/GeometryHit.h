#ifndef GEOMETRYHIT_H
#define GEOMETRYHIT_H
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
	class GeometryHit :
		public Hitable
	{
	public:

		GeometryHit() {}

		GeometryHit(Sdf* sdf, Material* mat)
			: Hitable(sdf, mat)
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
#endif // !GEOMETRYHIT_H
