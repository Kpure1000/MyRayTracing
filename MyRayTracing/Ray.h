#ifndef RAY_H
#define RAY_H
#include"Vector3.h"
namespace ry
{
	class Ray
	{
	public:

		Ray() {}

		Ray(const Vector3& a, const Vector3& b) { A = a; B = b; }

		Vector3 Origin()const { return A; }

		Vector3 Direction()const { return B; }

		Vector3 PointTo(const float& t) { return A + t * B; }

		Vector3 A;
		Vector3 B;
	};
}
#endif