#ifndef RAY_H
#define RAY_H
#include"Vector3.h"
namespace ry
{
	class Ray
	{
	public:

		Ray() {}

		Ray(const Vector3& a, const Vector3& b) { origin = a; direction = b; }

		Vector3 Origin()const { return origin; }

		Vector3 Direction()const { return direction; }

		Vector3 PointTo(const float& t)const { return origin + t * direction; }

		Vector3 origin;
		Vector3 direction;
	};
}
#endif