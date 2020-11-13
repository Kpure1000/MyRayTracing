#ifndef HITLIST_H
#define HITLIST_H
#include "Hitable.h"
namespace ry
{
	class HitList : public Hitable
	{
	public:

		HitList() {}

		HitList(int listSize) : size(listSize)
		{
			list = (Hitable**)malloc(sizeof(Hitable*) * size);
		}

		~HitList()
		{
			if (list != nullptr)
			{
				free(list);
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
					closet_so_far = rec.t; // ��Hitable��ס,����������Զ��
				}
			}
			return isHited;
		}

		Hitable** list;
		int size;

	};
}
#endif