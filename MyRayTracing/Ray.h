#ifndef RAY_H
#define RAY_H
#include"Vector3.h"
namespace ry
{
	class Ray
	{
	public:

		Ray() {}

		Ray(const Vector3& a, const Vector3& b)
			: origin(a), direction(b), m_time(0.0f)
		{}

		Ray(const Vector3& a, const Vector3& b, const float& time)
			: origin(a), direction(b), m_time(time)
		{}

		Vector3 Origin()const { return origin; }

		Vector3 Direction()const { return direction; }

		Vector3 PointTo(const float& t)const { return origin + t * direction; }

		float time() { return m_time; }

		Vector3 origin;
		Vector3 direction;
		float m_time;
	};
}
#endif