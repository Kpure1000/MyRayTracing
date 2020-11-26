#ifndef BVH_H
#define BVH_H
#include"Hitable.h"
#include"AABB.h"
namespace ry
{
	class BNode : public Hitable, public Drawable
	{
	public:
		BNode() :left(nullptr), right(nullptr)
		{}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (box.Hit(r, tMin, tMax))
			{
				HitRecord recl, recr;
				bool isHitl = false, isHitr = false;
				if (left)isHitl = left->Hit(r, tMin, tMax, recl);
				if (right)isHitr = right->Hit(r, tMin, tMax, recr);
				if (isHitl && isHitr)
				{
					if (recl.t < recr.t)
						rec = recl;
					else
						rec = recr;
					return true;
				}
				else if (isHitl)
				{
					rec = recl; return true;
				}
				else if (isHitr)
				{
					rec = recr; return true;
				}
			}
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			box = this->box; return true;
		}

		AABB box;

		Hitable* left, * right;

	private:

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			target.draw(box, states);
		}

	};

	class BVH : public Hitable
	{
	public:

	};

}
#endif // !BVH_H
