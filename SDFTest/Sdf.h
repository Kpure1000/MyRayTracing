#ifndef SDF_H
#define SDF_H
#include"Ray.h"
using namespace ry;

namespace sdf
{
	class Sdf
	{
	public:
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result)const = 0;

		virtual bool sdf(const Vector2f& p, float& sdfResult)const = 0;

	};

	class SdfSphere : public Sdf
	{
	public:

		SdfSphere() {}

		SdfSphere(const Vector2f& Center, const float& Radius)
			: center(Center), radius(Radius) {}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			Vector2f o2c = r.Origin() - center;
			float a = Dot(r.Direction(), r.Direction());
			float b = 2.0f * Dot(o2c, r.Direction());
			float c = Dot(o2c, o2c) - radius * radius;
			float discriminant = b * b - 4 * a * c;
			if (discriminant > 0)
			{
				rec.t = (-b - sqrtf(discriminant)) / (2.0f * a);
				if (rec.t > tMin && rec.t < tMax)
				{
					rec.hitPoint = r.PointTo(rec.t);
					rec.normal = (rec.hitPoint - center) / radius;
					return true;
				}
				rec.t = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (rec.t > tMin && rec.t < tMax)
				{
					rec.hitPoint = r.PointTo(rec.t);
					rec.normal = (rec.hitPoint - center) / radius;
					return true;
				}
			}
			return false;
		}

		virtual bool sdf(const Vector2f& p, float& sdfResult)const
		{
			sdfResult = Distance(p, center);
			if (sdfResult <= radius + 1e-3f)
			{
				return true;
			}
		}

		Vector2f center;

		float radius;
	};
}

#endif // !SDF_H
