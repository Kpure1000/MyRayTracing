#ifndef HITABLE_H
#define HITABLE_H
#include"AABB.h"
namespace sdf
{
	class Sdf;
}
namespace ry
{

	class Material;

	struct HitRecord
	{
	public:
		float t = 0.0f;
		Vector3 hitPoint;
		Vector3 normal;
		Material* mat = nullptr;
		bool isInRange = false;
		float u = 0.0f, v = 0.0f;
	};

	class Hitable
	{
	public:

		Hitable() :sdf(NULL), material(NULL) {}

		Hitable(sdf::Sdf* sdf) :sdf(sdf), material(NULL) {}

		Hitable(Material* mat) :sdf(NULL), material(mat) {}

		Hitable(sdf::Sdf* sdf, Material* mat) :sdf(sdf), material(mat) {}

		//返回射线碰撞数据record,射线被约束在tMin和tMax之间
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const = 0;

		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const = 0;

		Material* material;

		sdf::Sdf* sdf;
	};

}

#endif // !HITABLE_H