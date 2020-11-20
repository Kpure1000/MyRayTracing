#include"Vector3.h"

namespace ry
{

	Vector3 Vector3::Zero = { 0.0f, 0.0f, 0.0f };

	Vector3 Vector3::One = { 1.0f, 1.0f, 1.0f };

	/*inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
	}*/

	/*Vector3 operator-(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
	}

	Vector3 operator*(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
	}

	Vector3 operator/(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]);
	}

	Vector3 operator*(const float& t, const Vector3& v2)
	{
		return Vector3(v2[0] * t, v2[1] * t, v2[2] * t);
	}

	Vector3 operator*(const Vector3& v2, const float& t)
	{
		return Vector3(v2[0] * t, v2[1] * t, v2[2] * t);
	}

	Vector3 operator/(const Vector3& v2, const float& t)
	{
		float k = 1.0f / t;
		return Vector3(v2[0] * k, v2[1] * k, v2[2] * k);
	}*/

	/*float Vector3::Magnitude(const Vector3& v)
	{
		return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	Vector3 Vector3::Normalize(const Vector3& v2)
	{
		return v2 / v2.Magnitude();
	}*/
}
