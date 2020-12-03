#ifndef BVHNODE_H
#define BVHNODE_H
#include"AABB.h"
#include"Hitable.h"
namespace ry
{
	class BNode
	{
	public:
		BNode() :obj(nullptr), left(nullptr), right(nullptr)
		{}

		bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (box.Hit(r, tMin, tMax))
			{
				HitRecord recL, recR;
				bool isHitL = false, isHitR = false;

				if (left)isHitL = left->Hit(r, tMin, tMax, recL);
				if (right)isHitR = right->Hit(r, tMin, tMax, recR);

				if (isHitL && isHitR)
				{
					rec = recL.t < recR.t ? recL : recR;
					if (rec.t<tMax && rec.t>tMin)
						return true;
				}
				else if (isHitL)
				{
					if (recL.t<tMax && recL.t>tMin)
					{
						rec = recL;
						return true;
					}
				}
				else if (isHitR)
				{
					if (recR.t<tMax && recR.t>tMin)
					{
						rec = recR;
						return true;
					}
				}
				else if (obj != nullptr)
				{
					bool isHit = obj->Hit(r, tMin, tMax, rec);
					if (isHit)
					{
						if (rec.t<tMax && rec.t>tMin)
						{
							return true;
						}
					}
				}
			}
			return false;
		}

		Hitable* obj;

		AABB box;

		BNode* left, * right;

	};

	class LBNode
	{
	public:

		LBNode() :obj(nullptr), left(-1), right(-1) {}

		/// <summary>
		/// Box of Obj
		/// </summary>
		AABB box;

		/// <summary>
		/// Hitable Object
		/// </summary>
		Hitable* obj;

		/// <summary>
		/// Offset
		/// </summary>
		int left, right;

		int fix_data;

	};

}
#endif // !BVHNODE_H