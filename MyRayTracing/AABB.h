#ifndef AABB_H
#define AABB_H
#include"Ray.h"
#include<algorithm>
namespace ry
{

	inline float ffMin(const float& a, const float& b) { return a < b ? a : b; }

	inline float ffMax(const float& a, const float& b) { return a > b ? a : b; }

	/*Axis-aligned bounding box*/
	class AABB
	{
	public:

		AABB() {}

		AABB(const Vector3& pMin, const Vector3& pMax)
		{
			rect[0] = pMin;
			rect[1] = pMax;
		}

		AABB(const Vector3& middle, const Vector3& pMin, const Vector3& pMax)
		{
			rect[0] = pMin;
			rect[1] = pMax;
		}

		bool Hit(const Ray& r, float tMin, float tMax)const
		{
			float div, t0, t1;
			for (int i = 0; i < 3; i++)
			{
				div = 1.0f / r.direction[i];
				t0 = (rect[0][i] - r.origin[i]) * div;
				t1 = (rect[1][i] - r.origin[i]) * div;
				if (div < 0.0f)
					std::swap(t0, t1);
				tMin = t0 > tMin ? t0 : tMin;
				tMax = t1 < tMax ? t1 : tMax;
				if (tMax <= tMin)return false;
			}
			return true;
		}

		static AABB Surrounding(const AABB& boxA, const AABB& boxB)
		{
			Vector3 pMin(
				std::fmin(boxA.rect[0][0], boxB.rect[0][0]),
				std::fmin(boxA.rect[0][1], boxB.rect[0][1]),
				std::fmin(boxA.rect[0][2], boxB.rect[0][2])
			);
			Vector3 pMax(
				std::fmax(boxA.rect[1][0], boxB.rect[1][0]),
				std::fmax(boxA.rect[1][1], boxB.rect[1][1]),
				std::fmax(boxA.rect[1][2], boxB.rect[1][2])
			);
			return AABB(pMin, pMax);
		}

		Vector3 rect[2];

	};
}

#endif // !AABB_H
