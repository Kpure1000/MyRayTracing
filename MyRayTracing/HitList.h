#ifndef HITLIST_H
#define HITLIST_H
#include "Hitable.h"
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

		Hitable** list;
		int size;
		int maxSize;

	};
}
#endif