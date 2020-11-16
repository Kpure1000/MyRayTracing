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
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result)const = 0;

		virtual bool sdf(const Vector3& p, float& sdfResult)const = 0;
	};

	class SdfSphere : public Sdf
	{
	public:
		SdfSphere() {}

		SdfSphere(const Vector3& Center, const float& Radius)
			: center(Center), radius(Radius) {}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result)const
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
			if (sdfResult <= radius + 1e-3f)
			{
				return true;
			}
		}

		Vector3 center;

		float radius;
	};

	class SdfIntersection : public Sdf
	{
	public:

		SdfIntersection(Sdf* A, Sdf* B)
			:sdfA(A), sdfB(B)
		{

		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result)const
		{
			float disA, disB; //  距离
			HitRecord resA = result, resB = result; //  碰撞结果
			bool isHitA, isHitB; //  是否碰撞
			float discriminant;
			isHitA = sdfA->Hit(r, tMin, tMax, resA);
			isHitB = sdfB->Hit(r, tMin, tMax, resB);
			float sdfResult;
			if (resA.t < resB.t)
			{
				if (this->sdf(r.PointTo(resA.t), sdfResult))//HitA in
				{
					result = resA;
					return isHitA;
				}
				else if(this->sdf(r.PointTo(resB.t), sdfResult))//HitB in
				{
					result = resB;
					return isHitB;
				}
				
				return false; //  none in
			}
			else
			{
				if (this->sdf(r.PointTo(resB.t), sdfResult))//HitB in
				{
					result = resB;
					return isHitB;
				}
				else if (this->sdf(r.PointTo(resA.t), sdfResult))//HitA in
				{
					result = resA;
					return isHitA;
				}
				return false; //  none in
			}
		}

		/*相交区域的*/
		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			return sdfA->sdf(p, sdfResult)
				&& sdfB->sdf(p, sdfResult);
		}

		Sdf* sdfA;
		Sdf* sdfB;
	};

	class SdfUnion : public Sdf
	{
	public:

		SdfUnion(Sdf* A, Sdf* B)
			:sdfA(A), sdfB(B)
		{

		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result)const
		{
			return true;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			return true;
		}

		Sdf* sdfA;
		Sdf* sdfB;
	};

}

#endif // !SDF_H
