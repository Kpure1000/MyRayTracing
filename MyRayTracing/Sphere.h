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
			return sdfSphere.Hit(r, tMin, tMax, rec);
		}

		SdfSphere sdfSphere;

	};

}

#endif // !SPHERE_H