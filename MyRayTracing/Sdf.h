#ifndef SDF_H
#define SDF_H
#include"Ray.h"
using namespace ry;

namespace sdf
{
	/*
	* 有符号距离场
	* 表示0亏格单个面组成的体（球、长方体等）
	*/
	class Sdf
	{
	public:
		virtual bool sdf(const Ray& r, const float& tMin,
			const float& tMax, float& discriminant, float& result)const = 0;
	};

	class SdfSphere : public Sdf
	{
	public:
		SdfSphere() {}

		SdfSphere(const Vector3& Center, const float& Radius)
			: center(Center), radius(Radius) {}

		virtual bool sdf(const Ray& r, const float& tMin,
			const float& tMax, float& discriminant, float& result)const
		{
			Vector3 o2c = r.Origin() - center;
			float a = Vector3::Dot(r.Direction(), r.Direction());
			float b = 2.0f * Vector3::Dot(o2c, r.Direction());
			float c = Vector3::Dot(o2c, o2c) - radius * radius;
			discriminant = b * b - 4 * a * c;
			if (discriminant > 0)
			{
				result = (-b - sqrtf(discriminant)) / (2.0f * a);
				if (result > tMin && result < tMax)
				{
					return true;
				}
				result = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (result > tMin && result < tMax)
				{
					return true;
				}
			}
			return false;
		}

		Vector3 center;

		float radius;
	};


}

#endif // !SDF_H
