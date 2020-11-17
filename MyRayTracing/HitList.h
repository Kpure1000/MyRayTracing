#ifndef HITLIST_H
#define HITLIST_H
#include "Hitable.h"
namespace ry
{
	class HitList : public Hitable
	{
	public:

		HitList() : list(NULL), size(0) {}

		HitList(int listSize) : size(listSize)
		{
			list = (Hitable**)malloc(sizeof(Hitable*) * size);
		}

		HitList(Hitable** List, int listSize) :list(List), size(listSize)
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
			size++;
			list[size - 1] = hit;
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

		Hitable** list;
		int size;

	};
}
#endif