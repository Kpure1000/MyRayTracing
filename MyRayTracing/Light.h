#ifndef LIGHT_H
#define LIGHT_H
#include"Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
	class Light : public Hitable
	{
	public:

		Light() {}

		Light(Sdf* sdfObj, Material* mat)
			:Hitable(sdfObj, mat) {}

		virtual bool Hit(const Ray& r, const float& tMin, const float& tMax, HitRecord& rec)const
		{
			SdfRecord sdfR;
			bool isHited = sdf->Hit(r, tMin, tMax, rec, sdfR);
			if (material != nullptr)
			{
				rec.mat = material;
			}
			rec.isInterrupted = isHited;
			return isHited;
		}

		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

	};
}
#endif // !LIGHT_H
