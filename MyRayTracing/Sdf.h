#ifndef SDF_H
#define SDF_H
#include"Ray.h"
#include"AABB.h"
#include"RayMath.h"
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

		virtual bool GetBBox(float t0, float t1, AABB& box)const
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
					GetSphereUV(result.u, result.v, result.normal);
					return true;
				}
				result.t = (-b + sqrtf(discriminant)) / (2.0f * a);
				if (result.t > tMin && result.t < tMax)
				{
					result.hitPoint = r.PointTo(result.t);
					result.normal = (result.hitPoint - center) / radius;
					GetSphereUV(result.u, result.v, result.normal);
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
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
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

		inline static void GetSphereUV(float& u, float& v, const Vector3& normal)
		{
			float cosY = normal[1];
			float cosX = normal[0] / sqrt(normal[0] * normal[0] + normal[2] * normal[2]);
			u = normal[2] > 0 ? acos(cosX) / Pi / 2 : 1 - acos(cosX) / Pi / 2;
			v = acos(cosY) / Pi;
		}

	};

	class SdfRect : public Sdf
	{
	public:

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const = 0;

		virtual bool sdf(const Vector3& p, float& sdfResult)const = 0;

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			return true;
		}

		Vector3	rect[2];

	protected:

		virtual bool isInRange(const float&, const float&)const = 0;

	};

	class SdfRect_xy : public SdfRect
	{
	public:

		SdfRect_xy() {}

		SdfRect_xy(const float& xMin, const float& yMin, const float& xMax, const float& yMax, const float& k)
		{
			rect[0][0] = xMin;
			rect[0][1] = yMin;
			rect[1][0] = xMax;
			rect[1][1] = yMax;
			rect[0][2] = rect[1][2] = k;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			float t = (rect[0][2] - r.origin[2]) / r.direction[2];
			if (t<tMin || t>tMax)return false;
			float x = r.origin[0] + r.direction[0] * t;
			float y = r.origin[1] + r.direction[1] * t;
			bool isHited = isInRange(x, y);
			if (isHited == false)return false;
			result.u = (x - rect[0][0]) / (rect[1][0] - rect[0][0]);
			result.v = (y - rect[0][1]) / (rect[1][1] - rect[0][1]);
			result.t = t;
			result.normal = r.direction[2] < 0 ? Vector3(0, 0, 1) : Vector3(0, 0, -1);
			result.hitPoint = Vector3(x,y,rect[0][2]);
			return isHited;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			sdfResult = Vector3::Distance(p, (rect[0] + rect[1]) * 0.5f);
			if (std::fabsf(p[2] - rect[0][2]) < 0.01f && isInRange(p[0], p[1]))
			{
				return true;
			}
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			box.rect[0] = rect[0] - Vector3(0, 0, 0.01f);
			box.rect[1] = rect[1] + Vector3(0, 0, 0.01f);
			box.rect[2] = (box.rect[0] + box.rect[1]) * 0.5f;
			return true;
		}


	protected:

		virtual bool isInRange(const float& x, const float& y)const
		{
			return (x > rect[0][0] && x < rect[1][0] && y>rect[0][1] && y < rect[1][1]);
		}

	};

	class SdfRect_xz : public SdfRect
	{
	public:

		SdfRect_xz() {}

		SdfRect_xz(const float& xMin, const float& zMin, const float& xMax, const float& zMax, const float& k)
		{
			rect[0][0] = xMin;
			rect[0][2] = zMin;
			rect[1][0] = xMax;
			rect[1][2] = zMax;
			rect[0][1] = rect[1][1] = k;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			float t = (rect[0][1] - r.origin[1]) / r.direction[1];
			if (t<tMin || t>tMax)return false;
			float x = r.origin[0] + r.direction[0] * t;
			float z = r.origin[2] + r.direction[2] * t;
			bool isHited = isInRange(x, z);
			if (!isHited)return false;
			result.u = (x - rect[0][0]) / (rect[1][0] - rect[0][0]);
			result.v = (z - rect[0][2]) / (rect[1][2] - rect[0][2]);
			result.t = t;
			result.normal = r.direction[1] < 0 ? Vector3(0, 1, 0) : Vector3(0, -1, 0);
			result.hitPoint = { x,rect[0][1],z };
			return isHited;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			sdfResult = Vector3::Distance(p, (rect[0] + rect[1]) * 0.5f);
			if (std::fabsf(p[1] - rect[0][1]) < 0.01f && isInRange(p[0], p[2]))
			{
				return true;
			}
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			box.rect[0] = rect[0] - Vector3(0, 0.01f, 0);
			box.rect[1] = rect[1] + Vector3(0, 0.01f, 0);
			box.rect[2] = (box.rect[0] + box.rect[1]) * 0.5f;
			return true;
		}

	protected:

		virtual bool isInRange(const float& x, const float& z)const
		{
			return (x > rect[0][0] && x < rect[1][0] && z>rect[0][2] && z < rect[1][2]);
		}

	};

	class SdfRect_yz : public SdfRect
	{
	public:

		SdfRect_yz() {}

		SdfRect_yz(const float& yMin, const float& zMin, const float& yMax, const float& zMax, const float& k)
		{
			rect[0][1] = yMin;
			rect[0][2] = zMin;
			rect[1][1] = yMax;
			rect[1][2] = zMax;
			rect[0][0] = rect[1][0] = k;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			float t = (rect[0][0] - r.origin[0]) / r.direction[0];
			if (t<tMin || t>tMax)return false;
			float y = r.origin[1] + r.direction[1] * t;
			float z = r.origin[2] + r.direction[2] * t;
			bool isHited = isInRange(y, z);
			if (!isHited)return false;
			result.u = (y - rect[0][1]) / (rect[1][1] - rect[0][1]);
			result.v = (z - rect[0][2]) / (rect[1][2] - rect[0][2]);
			result.t = t;
			result.normal = r.direction[0] < 0 ? Vector3(1, 0, 0) : Vector3(-1, 0, 0);
			result.hitPoint = { rect[0][0],y,z };
			return isHited;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			sdfResult = Vector3::Distance(p, (rect[0] + rect[1]) * 0.5f);
			if (std::fabsf(p[0] - rect[0][0]) < 0.01f && isInRange(p[1], p[2]))
			{
				return true;
			}
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			box.rect[0] = rect[0] - Vector3(0.01f, 0, 0);
			box.rect[1] = rect[1] + Vector3(0.01f, 0, 0);
			box.rect[2] = (box.rect[0] + box.rect[1]) * 0.5f;
			return true;
		}

	protected:

		virtual bool isInRange(const float& y, const float& z)const
		{
			return (y > rect[0][1] && y < rect[1][1] && z>rect[0][2] && z < rect[1][2]);
		}

	};

	class SdfBox : public Sdf
	{
	public:

		SdfBox(Vector3 pMin, Vector3 pMax)
		{
			face[0] = new SdfRect_yz(pMin[1], pMin[2], pMax[1], pMax[2], pMax[0]);
			face[1] = new SdfRect_yz(pMin[1], pMin[2], pMax[1], pMax[2], pMin[0]);

			face[2] = new SdfRect_xz(pMin[0], pMin[2], pMax[0], pMax[2], pMax[1]);
			face[3] = new SdfRect_xz(pMin[0], pMin[2], pMax[0], pMax[2], pMin[1]);

			face[4] = new SdfRect_xy(pMin[0], pMin[1], pMax[0], pMax[1], pMax[2]);
			face[5] = new SdfRect_xy(pMin[0], pMin[1], pMax[0], pMax[1], pMin[2]);

		}

		~SdfBox()
		{
			for (size_t i = 0; i < 6; i++)
			{
				delete face[i];
			}
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			HitRecord recTmp;
			recTmp.mat = result.mat;
			float closet_so_far = tMax;
			bool isHited = false;
			for (int i = 0; i < 6; i++)
			{
				if (face[i]->Hit(r, tMin, closet_so_far, recTmp, sdfRec))
				{
					isHited = true;
					result = recTmp;
					closet_so_far = result.t;
				}
			}
			return isHited;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			return true;
		}

		SdfRect* face[6];

	protected:

		virtual bool isInRange(const float& y, const float& z)const
		{
			return false;
		}

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
			HitRecord resA = result, resB = result; //  碰撞结果
			bool isHitA, isHitB; //  是否碰撞
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

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			AABB boxA, boxB;
			sdfs[0]->GetBBox(t0, t1, boxA);
			sdfs[1]->GetBBox(t0, t1, boxB);
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

		SdfUnion(Sdf* A, Sdf* B) :sdfs()
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

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			AABB boxA, boxB;
			sdfs[0]->GetBBox(t0, t1, boxA);
			sdfs[1]->GetBBox(t0, t1, boxB);
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

	class SdfTranslate : public Sdf
	{
	public:

		SdfTranslate() :target(nullptr) {}

		SdfTranslate(Sdf* Target, const Vector3& Offset)
			:target(Target), offset(Offset) {}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			Ray translateR = Ray(r.origin - offset, r.direction);
			if (target && target->Hit(translateR, tMin, tMax, result, sdfRec))
			{
				result.hitPoint += offset;
				return true;
			}
			return false;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			if (target)
				return target->sdf(p, sdfResult);
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			if (target && target->GetBBox(t0, t1, box))
			{
				box = AABB(box.rect[0] + offset, box.rect[1] + offset);
				return true;
			}
			return false;
		}

		Sdf* target;

		Vector3 offset;
	};

	class SdfRotate_Y : public Sdf
	{
	public:

		SdfRotate_Y() :target(nullptr) {}

		SdfRotate_Y(Sdf* Target, const float& Angle)
			:target(Target)
		{
			float radians = (Pi / 180.0f) * Angle;
			sinTheta = sin(radians);
			cosTheta = cos(radians);
			if (target)
			{
				hasBox = target->GetBBox(0, 1, bBox);
				Vector3 vMin(MAX_FLOAT, MAX_FLOAT, MAX_FLOAT);
				Vector3 vMax(-MAX_FLOAT, -MAX_FLOAT, -MAX_FLOAT);
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							float x = i * bBox.rect[1][0] + (1 - i) * bBox.rect[0][0];
							float y = j * bBox.rect[1][1] + (1 - j) * bBox.rect[0][1];
							float z = k * bBox.rect[1][2] + (1 - k) * bBox.rect[0][2];
							float newX = cosTheta * x + sinTheta * z;
							float newZ = -sinTheta * x + cosTheta * z;
							Vector3 tester(newX, y, newZ);
							for (int c = 0; c < 3; c++)
							{
								if (tester[c] > vMax[c])
									vMax[c] = tester[c];
								if (tester[c] < vMin[c])
									vMin[c] = tester[c];
							}
						}
					}
				}
				bBox = AABB(vMin, vMax);
			}
			else
			{
				hasBox = false;
			}
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& result, SdfRecord& sdfRec)const
		{
			Vector3 origin = r.origin;
			Vector3 direction = r.direction;
			origin[0] = cosTheta * r.origin[0] - sinTheta * r.origin[2];
			origin[2] = sinTheta * r.origin[0] + cosTheta * r.origin[2];
			direction[0] = cosTheta * r.direction[0] - sinTheta * r.direction[2];
			direction[2] = sinTheta * r.direction[0] + cosTheta * r.direction[2];
			Ray rotateR(origin, direction);
			if (target && target->Hit(rotateR, tMin, tMax, result, sdfRec))
			{
				Vector3 hitP = result.hitPoint;
				Vector3 nor = result.normal;
				hitP[0] = cosTheta * result.hitPoint[0] + sinTheta * result.hitPoint[2];
				hitP[2] = -sinTheta * result.hitPoint[0] + cosTheta * result.hitPoint[2];
				nor[0] = cosTheta * result.normal[0] + sinTheta * result.normal[2];
				nor[2] = -sinTheta * result.normal[0] + cosTheta * result.normal[2];
				result.hitPoint = hitP;
				result.normal = nor;
				return true;
			}
			return false;
		}

		virtual bool sdf(const Vector3& p, float& sdfResult)const
		{
			if (target)
				return target->sdf(p, sdfResult);
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			if (target)
				box = bBox;
			return hasBox;
		}

		Sdf* target;

		float sinTheta, cosTheta;
		bool hasBox;
		AABB bBox;
	};

}

#endif // !SDF_H
