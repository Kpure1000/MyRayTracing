#ifndef TESTBVH_H
#define TESTBVH_H
#include"Hitable.h"
#include"BVH.h"
#include"AABB.h"
#include<iomanip>
#include<vector>
#include<iostream>
using std::cout;
using std::setw;
using std::setfill;

namespace ry
{
	class TestBVH : public Hitable
	{
	public:
		TestBVH(Hitable** list,int n)
		{
			//BVH::SortList(list, n, true);
			if (n == 1)
			{
				left = right = list[0];
			}
			else if (n == 2)
			{
				left = list[0];
				right = list[1];
			}
			else
			{
				left = new TestBVH(list, n / 2);
				right = new TestBVH(list + n / 2, n - n / 2);
			}
			AABB boxL, boxR;
			if (!left->GetBBox(boxL) || !right->GetBBox(boxR))
			{
				std::cerr << "No bounding box in TestBVH constructor\n";
			}
			box = AABB::UnionBox(boxL, boxR);
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (box.Hit(r, tMin, tMax))
			{
				HitRecord recL, recR;
				bool hitL = left->Hit(r, tMin, tMax, recL);
				bool hitR = right->Hit(r, tMin, tMax, recR);
				if (hitL && hitR)
				{
					if (recL.t < recR.t)
						rec = recL;
					else 
						rec = recR;
					return true;
				}
				else if (hitL)
				{
					rec = recL;
					return true;
				}
				else if (hitR)
				{
					rec = recR;
					return true;
				}
				return false;
			}
			return false;
		}

	private:

		Hitable* left;
		Hitable* right;

		AABB box;

		virtual void draw(RenderTarget& target, RenderStates states)const
		{
		}
	};

}

#endif // !TESTBVH_H