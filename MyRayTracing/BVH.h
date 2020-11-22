#ifndef BVH_H
#define BVH_H
#include "Hitable.h"
#include"RayMath.h"
#include <memory>
namespace ry
{

	class BNode : public Hitable
	{
	public:
		BNode() 
		{
			sub[0] = sub[1] = nullptr;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if (box.Hit(r, tMin, tMax))
			{
				HitRecord recL, recR;
				bool hitL = false; 
				bool hitR = false; 
				if (sub[0])hitL = sub[0]->Hit(r, tMin, tMax, recL);
				if (sub[1])hitR = sub[1]->Hit(r, tMin, tMax, recR);
				if (hitL && hitR)
				{
					if (recL.t < recR.t)
						rec = recL;
					else
						rec = recR;
					return true;
				}
				else if(hitL)
				{
					rec = recL;
					return true;
				}
				else if (hitR)
				{
					rec = recR;
					return true;
				}
			}
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			box = this->box;
			return true;
		}

		AABB box;

		Hitable* sub[2];
	};

	class BVH : public Hitable
	{
	public:

		BVH() :root(nullptr) {}

		BVH(Hitable** list, int n, float t0, float t1) :root(nullptr)
		{
			root = BuildBVH(list, n, t0, t1);

			//测试树:
			/*int count = 0;
			VisitBVHTree(root, count);
			printf("BVH有节点: %d个\n", count);*/
		}

		void VisitBVHTree(BNode* root,int& count)
		{
			if (root != nullptr)
			{
				count++;
					VisitBVHTree((BNode*)root->sub[0], count);
					VisitBVHTree((BNode*)root->sub[1], count);
			}
		}

		/*~BVH()
		{
			
		}*/

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			if(root)
				return root->Hit(r, tMin, tMax, rec);
			return false;
		}

		virtual bool GetBBox(float t0, float t1, AABB& box)const
		{
			if (root)
				return root->GetBBox(t0, t1, box);
			return false;
		}

	private:

		BNode* BuildBVH(Hitable** list, int n, float t0, float t1)
		{
			int axis = int(3 * RayMath::Drand48());
			if (axis == 0)
				qsort(list, n, sizeof(Hitable*), [](const void* a, const void* b)->int
					{
						AABB boxL, boxR;
						Hitable* hitA = *(Hitable**)a;
						Hitable* hitB = *(Hitable**)b;
						if (!hitA->GetBBox(0, 0, boxL) || !hitB->GetBBox(0, 0, boxR))
						{
							std::cerr << "No bounding box in BNode constructor.\n";
						}
						if (boxL.rect[0][0] - boxR.rect[0][0] < 0.0f)
							return -1;
						else
							return 1;
					});
			else if (axis == 1)
				qsort(list, n, sizeof(Hitable*), [](const void* a, const void* b)->int
					{
						AABB boxL, boxR;
						Hitable* hitA = *(Hitable**)a;
						Hitable* hitB = *(Hitable**)b;
						if (!hitA->GetBBox(0, 0, boxL) || !hitB->GetBBox(0, 0, boxR))
						{
							std::cerr << "No bounding box in BNode constructor.\n";
						}
						if (boxL.rect[0][1] - boxR.rect[0][1] < 0.0f)
							return -1;
						else
							return 1;
					});
			else
				qsort(list, n, sizeof(Hitable*), [](const void* a, const void* b)->int
					{
						AABB boxL, boxR;
						Hitable* hitA = *(Hitable**)a;
						Hitable* hitB = *(Hitable**)b;
						if (!hitA->GetBBox(0, 0, boxL) || !hitB->GetBBox(0, 0, boxR))
						{
							std::cerr << "No bounding box in BNode constructor.\n";
						}
						if (boxL.rect[0][2] - boxR.rect[0][2] < 0.0f)
							return -1;
						else
							return 1;
					});
			BNode* broot = new BNode();

			if (n == 1)
			{
				broot->sub[0] = broot->sub[1] = (BNode*)list[0];
			}
			else if (n == 2)
			{
				broot->sub[0] = list[0];
				broot->sub[1] = list[1];
			}
			else
			{
				broot->sub[0] = BuildBVH(list, n / 2, t0, t1);
				broot->sub[1] = BuildBVH(list + n / 2, n - n / 2, t0, t1);
			}
			AABB boxL, boxR;
			if (!broot->sub[0]->GetBBox(0, 0, boxL) || !broot->sub[0]->GetBBox(0, 0, boxR))
			{
				std::cerr << "No bounding box in BNode constructor.!\n";
			}

			broot->box = AABB::Surrounding(boxL, boxR);

			return broot;
		}

		BNode* root;

	};



}

#endif // !BVH_H
