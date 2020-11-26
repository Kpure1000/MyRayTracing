#ifndef HITLIST_H
#define HITLIST_H
#include "Hitable.h"
#include"AABB.h"
namespace ry
{
	class HitList : public Hitable
	{
	public:

		HitList() : list(NULL), size(0) {}

		HitList(int MaxSize) : size(0), maxSize(MaxSize)
		{
			list = (Hitable**)malloc(sizeof(Hitable*) * maxSize);
		}

		HitList(Hitable** List, int CurSize, int MaxSize)
			:list(List), size(CurSize), maxSize(MaxSize)
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
			if (size < maxSize)
			{
				size++;
				list[size - 1] = hit;
			}
		}

		virtual bool Hit(const Ray& r, const float& tMin, const float& tMax, HitRecord& rec)const
		{
			HitRecord recTmp;
			bool isHited = false;
			float closet_so_far = tMax;
			for (size_t i = 0; i < size; i++)
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

		virtual void SetMaterial(Material* mat)
		{
			material = mat;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			if (size < 1)return false;
			AABB tmpBox;
			bool first_true = list[0]->GetBBox(t0, t1, tmpBox);
			if (!first_true)
				return false;
			else
				box = tmpBox;
			for (int i = 0; i < size; i++)
			{
				if (list[0]->GetBBox(t0, t1, tmpBox)) 
				{
					box = AABB::UnionBox(box, tmpBox);
				}
				else
				{
					return false;
				}
			}
			return true;
		}

		Hitable** list;

		int size;
		int maxSize;

	};
}
#endif