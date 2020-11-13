#ifndef VECTOR3_H
#define VECTOR3_H
#include<cmath>
namespace ry
{
	class Vector3
	{
	public:

		Vector3() {}

		Vector3(float x, float y, float z) {
			v[0] = x;
			v[1] = y;
			v[2] = z;
		}

		static Vector3 One;

		static Vector3 Zero;

		inline float x()const { return v[0]; }
		inline float y()const { return v[1]; }
		inline float z()const { return v[2]; }
		inline float r()const { return v[0]; }
		inline float g()const { return v[1]; }
		inline float b()const { return v[2]; }

		bool operator== (const Vector3& b)const
		{
			return (*this)[0] == b[0] && (*this)[1] == b[1] && (*this)[2] == b[2];
		}

		Vector3& operator=(const Vector3& b)
		{
			(*this)[0] = b[0];
			(*this)[1] = b[1];
			(*this)[2] = b[2];
			return (*this);
		}

		inline const Vector3& operator+() const { return *this; }
		inline Vector3 operator-()const { return Vector3(-v[0], -v[1], -v[2]); }
		inline float operator[](const int& index)const { return v[index]; }
		inline float& operator[](const int& index) { return v[index]; }

		inline Vector3& operator+=(const Vector3& v2)
		{
			v[0] += v2.v[0];
			v[1] += v2.v[1];
			v[2] += v2.v[2];
			return *this;
		}

		inline Vector3& operator-=(const Vector3& v2)
		{
			v[0] -= v2.v[0];
			v[1] -= v2.v[1];
			v[2] -= v2.v[2];
			return *this;
		}

		inline Vector3& operator*=(const Vector3& v2)
		{
			v[0] *= v2.v[0];
			v[1] *= v2.v[1];
			v[2] *= v2.v[2];
			return *this;
		}

		inline Vector3& operator/=(const Vector3& v2)
		{
			v[0] /= v2.v[0];
			v[1] /= v2.v[1];
			v[2] /= v2.v[2];
			return *this;
		}

		inline Vector3& operator*=(const float& t)
		{
			v[0] *= t;
			v[1] *= t;
			v[2] *= t;
			return *this;
		}

		inline Vector3& operator/=(const float& t)
		{
			float k = 1.0f / t;
			v[0] *= k;
			v[1] *= k;
			v[2] *= k;
			return *this;
		}

		inline float Magnitude()const
		{
			return sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
		}

		inline float SqaureMagnitude()const
		{
			return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
		}

		inline Vector3 Normalize()
		{
			return (*this) /= (*this).Magnitude();
		}


		inline static float Dot(const Vector3& v1, const Vector3& v2)
		{
			return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
		}

		inline static Vector3 Cross(const Vector3& v1, const Vector3& v2)
		{
			return Vector3(v1[1] * v2[2] - v1[2] * v2[1],
				v1[2] * v2[0] - v1[0] * v2[2],
				v1[0] * v2[1] - v1[1] * v2[0]);
		}

		float v[3];
	};

	Vector3 Vector3::Zero = { 0.0f, 0.0f, 0.0f };

	Vector3 Vector3::One = { 1.0f, 1.0f, 1.0f };

	inline Vector3 operator+(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]);
	}

	inline Vector3 operator-(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]);
	}

	inline Vector3 operator*(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] * v2[0], v1[1] * v2[1], v1[2] * v2[2]);
	}

	inline Vector3 operator/(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1[0] / v2[0], v1[1] / v2[1], v1[2] / v2[2]);
	}

	inline Vector3 operator*(const float& t, const Vector3& v2)
	{
		return Vector3(v2[0] * t, v2[1] * t, v2[2] * t);
	}

	inline Vector3 operator*(const Vector3& v2, const float& t)
	{
		return Vector3(v2[0] * t, v2[1] * t, v2[2] * t);
	}

	inline Vector3 operator/(const Vector3 &v2,const float &t)
	{
		float k = 1.0f / t;
		return Vector3(v2[0] * k, v2[1] * k, v2[2] * k);
	}

	inline Vector3 Normalize(const Vector3 &v2)
	{
		return v2 / v2.Magnitude();
	}

	Vector3 random_in_unit_ball()
	{
		Vector3 re;
		do
		{
			re = 2.0f * Vector3(rand() % 100 / (float)100,
				rand() % 100 / (float)100, rand() % 100 / (float)100);
		} while (re.Magnitude() > 1.0f);
		return re;
	}

	Vector3 Reflect(const Vector3& v, const Vector3& n)
	{
		return v - 2 * Vector3::Dot(v, n) * n;
	}
}
#endif // !VECTOR3_H