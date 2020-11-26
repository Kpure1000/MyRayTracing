#ifndef BVH_H
#define BVH_H
#include"Hitable.h"
#include"AABB.h"
#include<memory>
#include<vector>
#include<iostream>
using std::shared_ptr;
using std::vector;
namespace ry
{
	class BTNode : public Hitable //, public Drawable
	{
	public:
		BTNode() :left(nullptr), right(nullptr)
		{}

		/// <summary>
		/// Calculate the intersection of node'box
		/// </summary>
		/// <paramo name="r">ray</param>
		/// <param name="tMin">min</param>
		/// <param name="tMax">max</param>
		/// <param name="rec">record</param>
		/// <returns>hit or not</returns>
		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			//  if intersected the box
			if (this->box.Hit(r, tMin, tMax))
			{
				HitRecord recl, recr;
				bool isHitl = false, isHitr = false;
				//  do intersection for sub node
				if (left)isHitl = left->Hit(r, tMin, tMax, recl);
				if (right)isHitr = right->Hit(r, tMin, tMax, recr);
				if (isHitl && isHitr)
				{
					rec = recl.t < recr.t ? recl : recr;
					return true;
				}
				else if (isHitl)
				{
					rec = recl; 
					return true;
				}
				else if (isHitr)
				{
					rec = recr;
					return true;
				}
			}
			return false;
		}

		virtual bool GetBBox(AABB& box)const
		{
			box = this->box; return true;
		}

		AABB box;

		Hitable* left, * right;

	private:

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			//target.draw(box, states);
		}

	};

	/// <summary>
	/// BVH node in a array
	/// </summary>
	/*class BNode : public Hitable
	{
	public:
		BNode()
		{

		}

		//  offset left
		int left;
		//  offset right
		int right;

	};*/

	/// <summary>
	/// Bounding Volume Hierarchy
	/// </summary>
	class BVH : public Hitable
	{
	public:

		/// <summary>
		/// Construct a BVH tree
		/// <para>1. Split primitive list and build a ptr BVH tree</para>
		/// <para>2. Compute representation of DFT of BVH</para> 
		/// </summary>
		/// <param name="list">primitive list</param>
		/// <param name="size">size of the list</param>
		BVH(Hitable** pList, int size) :m_root(nullptr)
		{
			m_tree = new Hitable * [size];
			m_root = BuildTree(pList, 0, (size_t)size - 1);
		}

		~BVH()
		{
			if (m_tree)delete[] m_tree;
		}

		virtual bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)const
		{
			//  TODO: use m_tree to travel
			if (m_root)
				return m_root->Hit(r, tMin, tMax, rec);
			return false;
		}

		virtual bool GetBBox(AABB& box)const
		{
			if (m_root)
				return m_root->GetBBox(box);
			return false;
		}

	private:

		/// <summary>
		/// Sort the list by centroid.x and then centroid.y
		/// </summary>
		/// <param name="list">list</param>
		/// <param name="size">size of list</param>
		void SortList(Hitable** list, size_t size, bool byX)
		{
			if (byX)
			{
				//  sort by centroid.x
				std::qsort(list, size, sizeof(Hitable*), [](const void* x, const void* y)->int
					{
						AABB xbox, ybox;
						Hitable* hx = *(Hitable**)x;
						Hitable* hy = *(Hitable**)y;
						if (!hx->GetBBox(xbox) || !hy->GetBBox(ybox))
						{
							std::cerr << "No bounding in bnode\n";
						}
						if (xbox.centroid.x - ybox.centroid.x < 0.0f)
							return -1;
						return 1;
					});
			}
			else
			{
				//  sort by centroid.y
				std::qsort(list, size, sizeof(Hitable*), [](const void* x, const void* y)->int
					{
						AABB xbox, ybox;
						Hitable* hx = *(Hitable**)x;
						Hitable* hy = *(Hitable**)y;
						if (!hx->GetBBox(xbox) || !hy->GetBBox(ybox))
						{
							std::cerr << "No bounding in bnode\n";
						}
						if (xbox.centroid.y - ybox.centroid.y < 0.0f)
							return -1;
						return 1;
					});
			}
		}

		Hitable* BuildTree(Hitable** list, size_t start, size_t end)
		{
			//  split
			float minX = 0, maxX = 0, minY = 0, maxY = 0;
			AABB tmpBox;
			for (int i = start; i <= end; i++)
			{
				if (!list[i]->GetBBox(tmpBox))return nullptr;
				minX = tmpBox.centroid.x < minX ? tmpBox.centroid.x : minX;
				minY = tmpBox.centroid.y < minY ? tmpBox.centroid.y : minY;
				maxX = tmpBox.centroid.x > maxX ? tmpBox.centroid.x : maxX;
				maxY = tmpBox.centroid.y > maxY ? tmpBox.centroid.y : maxY;
			}

			//  split by x or y, determinded by distance of x or y
			SortList(list + start, end - start + 1, maxX - minX > maxY - minY);

			BTNode* newRoot = new BTNode();
			if (end == start)
			{
				newRoot->left = newRoot->right = list[start];
			}
			else if (end == start + 1)
			{
				newRoot->left = list[start];
				newRoot->right = list[end];
			}
			else
			{
				size_t mid = start >> 2 + end >> 2;
				newRoot->left = BuildTree(list, start, mid);
				newRoot->right = BuildTree(list, mid, end);
			}

			AABB bl, br;
			if (!newRoot->left->GetBBox(bl) || !newRoot->right->GetBBox(br))
			{
				std::cerr << "No bounding in sub node\n";
			}

			newRoot->box = AABB::UnionBox(bl, br);
			return newRoot;
		}

		Hitable* DFTBVH(Hitable* tree)
		{
			return nullptr;
		}

		Hitable* m_root;

		Hitable** m_tree;

		/*struct Primitive
		{
			Primitive(const int& Index, AABB Box)
				:index(Index), bounds(Box) {}
			AABB bounds;
			int index;
		};

		vector<Primitive> buildData;*/

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			//target.draw((*m_root), states);
		}

	};

}
#endif // !BVH_H
