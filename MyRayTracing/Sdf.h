#ifndef SDF_H
#define SDF_H
#include"Ray.h"
#include"AABB.h"
using namespace ry;
using std::min;
using std::max;
namespace sdf
{
	/*碰撞距离场记录*/
	enum class SdfRecord
	{
		FLASE,
		TRUE,
		A,
		B
	};

	class Sdf
	{
	public:
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const = 0;

		virtual bool sdf(const Vector3& p, float& sdfResult)const = 0;

		virtual bool Range(AABB& box)
		{
			return true;
		}

		virtual bool Range(float t0, float t1, AABB& box)
		{
			return true;
		}

	};

	class SdfSphere : public Sdf
	{
	public:
		SdfSphere() {}

		SdfSphere(const Vector3& Center, const float& Radius)
			: center(Center), radius(Radius) {}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			Vector3 o2c = r.Origin() - center;
			float a = Vector3::Dot(r.Direction(), r.Direction());
			float b = 2.0f * Vector3::Dot(o2c, r.Direction());
			float c = Vector3::Dot(o2c, o2c) - radius * radius;
			float discriminant = b * b - 4 * a * c;
			if (discriminant > 0)
			{
				result.t = (-b - sqrtf(discriminant)) / (2.0f * a);
				if (result.t > tMin && result.t < tMax)
				{
					result.hitPoint = r.PointTo(result.t);
					result.normal = (result.hitPoint - center) / radius;
					return true;
				}
				result.t = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (result.t > tMin && result.t < tMax)
				{
					result.hitPoint = r.PointTo(result.t);
					result.normal = (result.hitPoint - center) / radius;
					return true;
				}
			}
			return false;
		}

		/*SDF函数
		* 如果不在外部，返回true
		*/
		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			sdfResult = Vector3::Distance(p, center);
			if (sdfResult <= radius + 1e-2f)
			{
				return true;
			}
		}

		virtual bool Range(AABB& box)
		{
			box.rect[0][0] = center[0] - radius;
			box.rect[0][1] = center[1] - radius;
			box.rect[0][2] = center[2] - radius;
			box.rect[1][0] = center[0] + radius;
			box.rect[1][1] = center[1] + radius;
			box.rect[1][2] = center[2] + radius;
			box.rect[2][0] = (box.rect[0][0] + box.rect[1][0]) * 0.5f;
			box.rect[2][1] = (box.rect[0][1] + box.rect[1][1]) * 0.5f;
			box.rect[2][2] = (box.rect[0][2] + box.rect[1][2]) * 0.5f;
			return true;
		}

		Vector3 center;

		float radius;
	};

	class SdfIntersection : public Sdf
	{
	public:

		SdfIntersection(Sdf* A, Sdf* B)			
		{
			sdfs[0] = A;
			sdfs[1] = B;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			float disA, disB; //  距离
			HitRecord resA = result, resB = result; //  碰撞结果
			bool isHitA, isHitB; //  是否碰撞
			float discriminant;
			isHitA = sdfs[0]->Hit(r, tMin, tMax, resA, sdfRec);
			isHitB = sdfs[1]->Hit(r, tMin, tMax, resB, sdfRec);
			float sdfResult;
			if (isHitA || isHitB)
			{
				if (resA.t < resB.t)
				{
					if (this->sdf(r.PointTo(resA.t), sdfResult))//HitA in
					{
						result = resA;
						sdfRec = SdfRecord::A;
						return isHitA;
					}
					else if (this->sdf(r.PointTo(resB.t), sdfResult))//HitB in
					{
						result = resB;
						sdfRec = SdfRecord::B;
						return isHitB;
					}
					sdfRec = SdfRecord::FLASE;
					return false; //  none in
				}
				else
				{
					if (this->sdf(r.PointTo(resB.t), sdfResult))//HitB in
					{
						result = resB;
						sdfRec = SdfRecord::B;
						return isHitB;
					}
					else if (this->sdf(r.PointTo(resA.t), sdfResult))//HitA in
					{
						result = resA;
						sdfRec = SdfRecord::A;
						return isHitA;
					}
					sdfRec = SdfRecord::FLASE;
					return false; //  none in
				}
			}
			sdfRec = SdfRecord::FLASE;
			return false;
		}

		/*相交区域的*/
		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			return sdfs[0]->sdf(p, sdfResult)
				&& sdfs[1]->sdf(p, sdfResult);
		}

		virtual bool Range(AABB& box)
		{
			AABB boxA, boxB;
			sdfs[0]->Range(boxA);
			sdfs[1]->Range(boxB);
			box.rect[0][0] = max(boxA.rect[0][0], boxB.rect[0][0]);
			box.rect[0][1] = max(boxA.rect[0][1], boxB.rect[0][1]);
			box.rect[0][2] = max(boxA.rect[0][2], boxB.rect[0][2]);
			box.rect[1][0] = min(boxA.rect[1][0], boxB.rect[1][0]);
			box.rect[1][1] = min(boxA.rect[1][1], boxB.rect[1][1]);
			box.rect[1][2] = min(boxA.rect[1][2], boxB.rect[1][2]);
			box.rect[2][0] = (box.rect[0][0] + box.rect[1][0]) * 0.5f;
			box.rect[2][1] = (box.rect[0][1] + box.rect[1][1]) * 0.5f;
			box.rect[2][2] = (box.rect[0][2] + box.rect[1][2]) * 0.5f;
			return true;
		}

		Sdf* sdfs[2];

	};

	class SdfUnion : public Sdf
	{
	public:

		SdfUnion(Sdf* A, Sdf* B):sdfs()
		{
			sdfs[0] = A;
			sdfs[1] = B;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			return true;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			return true;
		}

		virtual bool Range(AABB& box)
		{
			AABB boxA, boxB;
			sdfs[0]->Range(boxA);
			sdfs[1]->Range(boxB);
			box.rect[0][0] = min(boxA.rect[0][0], boxB.rect[0][0]);
			box.rect[0][1] = min(boxA.rect[0][1], boxB.rect[0][1]);
			box.rect[0][2] = min(boxA.rect[0][2], boxB.rect[0][2]);
			box.rect[1][0] = max(boxA.rect[1][0], boxB.rect[1][0]);
			box.rect[1][1] = max(boxA.rect[1][1], boxB.rect[1][1]);
			box.rect[1][2] = max(boxA.rect[1][2], boxB.rect[1][2]);
			box.rect[2][0] = (box.rect[0][0] + box.rect[1][0]) * 0.5f;
			box.rect[2][1] = (box.rect[0][1] + box.rect[1][1]) * 0.5f;
			box.rect[2][2] = (box.rect[0][2] + box.rect[1][2]) * 0.5f;
			return true;
		}

		Sdf* sdfs[2];
	};

}

#endif // !SDF_H
