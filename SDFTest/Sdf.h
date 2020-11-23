#ifndef SDF_H
#define SDF_H
#include"Ray.h"
using namespace ry;

namespace sdf
{
	/*Åö×²¾àÀë³¡¼ÇÂ¼*/
	enum class SdfRecord
	{
		FLASE,
		TRUE,
		A,
		B,
	};

	class Sdf
	{
	public:
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, SdfRecord& sdfRec)const = 0;

		virtual bool sdf(const Vector2f& p, float& sdfResult)const = 0;

		virtual bool sdf(const Vector2f& p, float& sdfResult, const float& offset)const
		{
			return false;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, HitRecord& far, SdfRecord& sdfRec)const
		{
			return false;
		}

	};

	class SdfSphere : public Sdf
	{
	public:

		SdfSphere() {}

		SdfSphere(const Vector2f& Center, const float& Radius)
			: center(Center), radius(Radius) {}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, SdfRecord& sdfRec)const
		{
			Vector2f o2c = r.Origin() - center;
			float a = Dot(r.Direction(), r.Direction());
			float b = 2.0f * Dot(o2c, r.Direction());
			float c = Dot(o2c, o2c) - radius * radius;
			float discriminant = b * b - 4 * a * c;
			if (discriminant > 0)
			{
				near.t = (-b - sqrtf(discriminant)) / (2.0f * a);
				if (near.t > tMin && near.t < tMax)
				{
					near.hitPoint = r.PointTo(near.t);
					near.normal = (near.hitPoint - center) / radius;
					return true;
				}
				near.t = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (near.t > tMin && near.t < tMax)
				{
					near.hitPoint = r.PointTo(near.t);
					near.normal = (near.hitPoint - center) / radius;
					return true;
				}
			}
			return false;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, HitRecord& far, SdfRecord& sdfRec)const
		{
			Vector2f o2c = r.Origin() - center;
			float a = Dot(r.Direction(), r.Direction());
			float b = 2.0f * Dot(o2c, r.Direction());
			float c = Dot(o2c, o2c) - radius * radius;
			float discriminant = b * b - 4 * a * c;
			if (discriminant > 0)
			{
				near.t = (-b - sqrtf(discriminant)) / (2.0f * a);
				far.t = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (near.t > tMin && near.t < tMax)
				{
					near.hitPoint = r.PointTo(near.t);
					near.normal = (near.hitPoint - center) / radius;
					near.isInRange = true;
				}
				if (far.t > tMin && far.t < tMax)
				{
					far.hitPoint = r.PointTo(far.t);
					far.normal = (far.hitPoint - center) / radius;
					far.isInRange = true;
				}
				return true;
			}
			sdfRec = SdfRecord::FLASE;
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

		virtual bool sdf(const Vector2f& p, float& sdfResult, const float& offset)const
		{
			sdfResult = Distance(p, center);
			if (sdfResult <= radius + offset)
			{
				return true;
			}
			return false;
		}

		Vector2f center;

		float radius;
		
	};

	/*AinterB*/
	class SdfIntersection : public Sdf
	{
	public:
		SdfIntersection() {}

		SdfIntersection(Sdf* A, Sdf* B)
			:sdfA(A), sdfB(B)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, SdfRecord& sdfRec)const
		{
			//float disA, disB; //  ¾àÀë
			HitRecord nearA = near, nearB = near; //  Åö×²½á¹û
			bool isHitA, isHitB; //  ÊÇ·ñÅö×²
			float discriminant;
			isHitA = sdfA->Hit(r, tMin, tMax, nearA, sdfRec);
			isHitB = sdfB->Hit(r, tMin, tMax, nearB, sdfRec);
			float sdfResult;
			if (isHitA || isHitB)
			{
				if (this->sdf(r.PointTo(nearA.t), sdfResult))//HitA in
				{
					near = nearA;
					sdfRec = SdfRecord::A;
					return isHitA;
				}
				if (this->sdf(r.PointTo(nearB.t), sdfResult))//HitB in
				{
					near = nearB;
					sdfRec = SdfRecord::B;
					return isHitB;
				}
				sdfRec = SdfRecord::FLASE;
				return false; //  none in
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

	/*AunionB*/
	class SdfUnion : public Sdf
	{
	public:

		SdfUnion() {}

		SdfUnion(Sdf* A, Sdf* B)
			:sdfA(A), sdfB(B)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, SdfRecord& sdfRec)const
		{
			//float disA, disB; //  ¾àÀë
			HitRecord nearA = near, nearB = near; //  Åö×²½á¹û
			HitRecord farA ,farB; //  Åö×²½á¹û
			SdfRecord sdfRA, sdfRB;
			bool isHitA, isHitB; //  ÊÇ·ñÅö×²
			float discriminant;
			isHitA = sdfA->Hit(r, tMin, tMax, nearA, farA, sdfRA);
			isHitB = sdfB->Hit(r, tMin, tMax, nearB, farB, sdfRB);
			float sdfResult;
			if (isHitA || isHitB)
			{
				if (nearA.isInRange && this->sdf(r.PointTo(nearA.t), sdfResult))//HitA in
				{
					near = nearA;
					sdfRec = SdfRecord::A;
					return isHitA;
				}
				if (nearB.isInRange && this->sdf(r.PointTo(nearB.t), sdfResult))//HitB in
				{
					near = nearB;
					sdfRec = SdfRecord::B;
					return isHitB;
				}
				if(farA.isInRange && this->sdf(r.PointTo(farA.t),sdfResult))
				{
					near = farA;
					sdfRec = SdfRecord::A;
					return isHitA;
				}
				if(farB.isInRange && this->sdf(r.PointTo(farB.t),sdfResult))
				{
					near = farB;
					sdfRec = SdfRecord::B;
					return isHitB;
				}
				sdfRec = SdfRecord::FLASE;
				return false; //  none in
			}
			sdfRec = SdfRecord::FLASE;
			return false;
		}

		virtual bool sdf(const Vector2f& p, float& sdfResult)const
		{
			return !(sdfA->sdf(p, sdfResult) && sdfB->sdf(p, sdfResult));
		}

		Sdf* sdfA;
		Sdf* sdfB;

	};


	/*A-B*/
	class SdfDifference : public Sdf
	{
	public:

		SdfDifference() {}

		SdfDifference(Sdf* A, Sdf* B)
			:sdfA(A), sdfB(B)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& near, SdfRecord& sdfRec)const
		{
			//float disA, disB; //  ¾àÀë
			HitRecord nearA = near, nearB = near; //  Åö×²½á¹û
			HitRecord farA, farB; //  Åö×²½á¹û
			SdfRecord sdfRA, sdfRB;
			bool isHitA, isHitB; //  ÊÇ·ñÅö×²
			float discriminant;
			isHitA = sdfA->Hit(r, tMin, tMax, nearA, farA, sdfRA);
			isHitB = sdfB->Hit(r, tMin, tMax, nearB, farB, sdfRB);
			float sdfResult;
			if (isHitA || isHitB)
			{
				if (nearA.isInRange && this->sdf(r.PointTo(nearA.t), sdfResult))//HitA in
				{
					near = nearA;
					sdfRec = SdfRecord::A;
					return isHitA;
				}
				if (nearB.isInRange && this->sdf(r.PointTo(nearB.t), sdfResult))//HitB in
				{
					near = nearB;
					sdfRec = SdfRecord::B;
					return isHitB;
				}
				if (farB.isInRange && this->sdf(r.PointTo(farB.t), sdfResult))
				{
					near = farB;
					sdfRec = SdfRecord::B;
					return isHitB;
				}
				if (farA.isInRange && this->sdf(r.PointTo(farA.t), sdfResult))
				{
					near = farA;
					sdfRec = SdfRecord::A;
					return isHitA;
				}
				sdfRec = SdfRecord::FLASE;
				return false; //  none in
			}
			sdfRec = SdfRecord::FLASE;
			return false;
		}

		virtual bool sdf(const Vector2f& p, float& sdfResult)const
		{
			return (sdfA->sdf(p, sdfResult) && !sdfB->sdf(p, sdfResult, -1e-2f));
		}

		Sdf* sdfA;
		Sdf* sdfB;

	};

}

#endif // !SDF_H
