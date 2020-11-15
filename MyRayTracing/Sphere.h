#ifndef SPHERE_H
#define SPHERE_H
#include "Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{

	class Sphere : public Hitable
	{
	public:

		Sphere() {}

		Sphere(SdfSphere sphere)
			: sdfSphere(sphere)
		{
		}

		Sphere(SdfSphere sphere, Material* mat)
			: sdfSphere(sphere), Hitable(mat)
		{
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (material != nullptr)
			{
				rec.mat = material;
			}
			float discriminant;
			bool isHit = sdfSphere.sdf(r, tMin, tMax, discriminant, rec.t);
			if (discriminant > 0)
			{
				rec.hitPoint = r.PointTo(rec.t);
				rec.normal = (rec.hitPoint - sdfSphere.center) / sdfSphere.radius;
			}
			return isHit;
		}

		SdfSphere sdfSphere;

	};

}

#endif // !SPHERE_H