#pragma once
#include"Hitable.h"
#include"AABB.h"
namespace ry
{
	class HitList : public Hitable
	{
	public:

		HitList() : list(NULL), maxSize(0), curSize(0) {}

		HitList(int listSize) : maxSize(listSize), curSize(0)
		{
			list = (Hitable**)malloc(sizeof(Hitable*) * maxSize);
		}

		HitList(Hitable** List, int listSize) :list(List), maxSize(listSize)
		{}

		~HitList()
		{
			if (list != nullptr)
			{
				free(list);
			}
		}

		void AddHitable(Hitable* hit)
		{
			if (curSize <= maxSize)
			{
				curSize++;
				list[curSize - 1] = hit;
			}
		}

		virtual bool Hit(const Ray& r, const float& tMin, const float& tMax, HitRecord& rec)const
		{
			HitRecord recTmp;
			bool isHited = false;
			float closet_so_far = tMax;
			for (size_t i = 0; i < curSize; i++)
			{
				if (list[i]->Hit(r, tMin, closet_so_far, recTmp))
				{
					isHited = true;
					rec = recTmp;
					closet_so_far = rec.t; // 被Hitable挡住,更新射线最远点
				}
			}
			return isHited;
		}

		virtual bool GetBBox(AABB& box)const
		{
			if (curSize < 1)return false;
			AABB tmpbox;
			if (!list[0]->GetBBox(tmpbox))return false;
			box = tmpbox;
			for (int i = 1; i < curSize; i++)
			{
				if (list[i]->GetBBox(tmpbox))
				{
					box = AABB::UnionBox(box, tmpbox);
				}
				else
				{
					return false;
				}
			}
			return true;
		}

		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

		Hitable** list;
		int maxSize;
		int curSize;

	private:

		virtual void draw(RenderTarget& target, RenderStates states)const
		{
			for (size_t i = 0; i < maxSize; i++)
			{
				target.draw(*list[i], states);
			}
		}
	};
}