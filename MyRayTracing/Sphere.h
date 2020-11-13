#ifndef SPHERE_H
#define SPHERE_H
#include "Hitable.h"

namespace ry
{

	class Sphere : public Hitable
	{
	public:

		Sphere() {}

		Sphere(const Vector3& Center, const float& Radius) : center(Center), radius(Radius) {}

		virtual bool Hit(const Ray& r, const float& tMin, const float& tMax, HitRecord& rec)const
		{
			Vector3 o2c = r.Origin() - center;
			float a = Vector3::Dot(r.Direction(), r.Direction());
			float b = 2.0f * Vector3::Dot(o2c, r.Direction());
			float c = Vector3::Dot(o2c, o2c) - radius * radius;
			float discriminant = b * b - 4 * a * c;
			if (discriminant > 0)
			{
				float tmpResult = (-b - sqrtf(discriminant)) / (2.0f * a);
				if (tmpResult<tMax && tmpResult >tMin)
				{
					rec.t = tmpResult;
					rec.vertex = r.PointTo(tmpResult);
					rec.normal = (rec.vertex - center).Normalize();
					return true;
				}
				tmpResult = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (tmpResult<tMax && tmpResult >tMin)
				{
					rec.t = tmpResult;
					rec.vertex = r.PointTo(tmpResult);
					rec.normal = (rec.vertex - center).Normalize();
					return true;
				}
			}
			return false;
		}

		Vector3 center;

		float radius;

	};

}

#endif // !SPHERE_H