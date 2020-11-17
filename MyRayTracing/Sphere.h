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

		Sphere(SdfSphere* sphere)
			: Hitable(sphere)
		{
		}

		Sphere(SdfSphere* sphere, Material* mat)
			: Hitable(sphere, mat)
		{
		}

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

	};

}

#endif // !SPHERE_H