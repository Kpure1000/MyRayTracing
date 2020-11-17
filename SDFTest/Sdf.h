#ifndef SDF_H
#define SDF_H
#include"Ray.h"
using namespace ry;

namespace sdf
{

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
			const float& tMax, HitRecord& result,SdfRecord& sdfRec)const = 0;

		virtual bool sdf(const Vector2f& p, float& sdfResult)const = 0;

	};

	class SdfSphere : public Sdf
	{
	public:

		SdfSphere() {}

		SdfSphere(const Vector2f& Center, const float& Radius)
			: center(Center), radius(Radius) {}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec, SdfRecord& sdfRec)const
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
			if (sdfResult <= radius + 1e-2f)
			{
				return true;
			}
			return false;
		}

		Vector2f center;

		float radius;
	};

	class SdfIntersection : public Sdf
	{
	public:
		SdfIntersection() {}

		SdfIntersection(Sdf* A, Sdf* B)
			:sdfA(A), sdfB(B)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			//float disA, disB; //  ¾àÀë
			HitRecord resA = result, resB = result; //  Åö×²½á¹û
			bool isHitA, isHitB; //  ÊÇ·ñÅö×²
			float discriminant;
			isHitA = sdfA->Hit(r, tMin, tMax, resA, sdfRec);
			isHitB = sdfB->Hit(r, tMin, tMax, resB, sdfRec);
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
					if (this->sdf(r.PointTo(resB.t), sdfResult))//HitB in
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
					if (this->sdf(r.PointTo(resA.t), sdfResult))//HitA in
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


		virtual bool sdf(const Vector2f& p, float& sdfResult)const
		{
			return sdfA->sdf(p, sdfResult)
				&& sdfB->sdf(p, sdfResult);
		}

		Sdf* sdfA;
		Sdf* sdfB;

	};
}

#endif // !SDF_H
