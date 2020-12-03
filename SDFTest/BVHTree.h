#ifndef BVHTREE_H
#define BVHTREE_H

#include<iostream>
#include<vector>

#include"Hitable.h"
#include"BNode.h"
#include"Stack.h"

using std::cerr;
using std::vector;

namespace ry
{
	class BVHTree : public Drawable
	{
	public:
	public:

		/// <summary>
		/// Construct a BVH tree
		/// <para>1. Split primitive list and build a ptr BVH tree</para>
		/// <para>2. Compute representation of DFT of BVH</para> 
		/// </summary>
		/// <param name="list">primitive list</param>
		/// <param name="size">size of the list</param>
		BVHTree(Hitable** pList, int size) :m_root(nullptr)
		{
			m_root = BuildTree(pList, (size_t)size);
			//  TODO make linear tree
			BuildLinearTree(m_root);
			int tmpSize = m_ltree.size();
			todo.SetSize(tmpSize);
		}

		~BVHTree()
		{
		}

		bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)
		{
			HitRecord tmpRec = rec;
			rec.t = tMax + 1.0f;
			int index = 0;
			todo.clear();
			todo.push(index);

			float closet_tmax = tMax;
			
			LBNode* tmpNode;

			bool isHited = false;

			while (!todo.isEmpty())
			{
				index = todo.top();
				todo.pop();
				tmpNode = &m_ltree[index];
				if (tmpNode->box.Hit(r, tMin, tMax))
				{
					if (tmpNode->left != -1)
					{
						todo.push(tmpNode->left);
					}
					if (tmpNode->right != -1)
					{
						todo.push(tmpNode->right);
					}
					else if (tmpNode->left == -1)//Ò¶½Úµã
					{
						if (tmpNode->obj->Hit(r, tMin, tMax, tmpRec))
						{
							rec = tmpRec.t < rec.t ? tmpRec : rec;
							isHited = true;
							
						}
					}
				}
			}
			return isHited;
		}

	private:

		BNode* BuildTree(Hitable** list, size_t size)
		{
			float minX = 0, maxX = 0, minY = 0, maxY = 0;
			AABB tmpBox;
			for (int i = 0; i < size; i++)
			{
				if (!list[i]->GetBBox(tmpBox))return nullptr;
				minX = tmpBox.GetCentroid().x < minX ? tmpBox.GetCentroid().x : minX;
				minY = tmpBox.GetCentroid().y < minY ? tmpBox.GetCentroid().y : minY;
				maxX = tmpBox.GetCentroid().x > maxX ? tmpBox.GetCentroid().x : maxX;
				maxY = tmpBox.GetCentroid().y > maxY ? tmpBox.GetCentroid().y : maxY;
			}
			SortList(list, size, maxX - minX > maxY - minY);
			//SortList(list, size, rand() % 100 >= 50);

			BNode* newRoot = new BNode();
			if (size == 1)
			{
				newRoot->obj = list[0];
				AABB b;
				if (newRoot->obj->GetBBox(b))
				{
					newRoot->box = b;
					newRoot->box.SetColor(sf::Color(255, 0, 0, 100));
				}
				else
				{
					std::cerr << "No bounding in self node\n";
				}
			}
			else if (size == 2)
			{
				newRoot->left = new BNode;
				newRoot->right = new BNode;
				newRoot->left->obj = list[0];
				newRoot->right->obj = list[1];
				AABB bl, br;
				if (!newRoot->left->obj->GetBBox(bl) || !newRoot->right->obj->GetBBox(br))
				{
					std::cerr << "No bounding in sub node\n";
				}
				newRoot->left->box = bl;
				newRoot->left->box.SetColor(sf::Color(255, 0, 0, 100));
				newRoot->right->box = br;
				newRoot->right->box.SetColor(sf::Color(255, 0, 0, 100));
				newRoot->box = AABB::UnionBox(bl, br);
			}
			else
			{
				size_t mid = size / 2;
				newRoot->left = BuildTree(list, mid);
				newRoot->right = BuildTree(list + mid, size - mid);
				newRoot->box = AABB::UnionBox(newRoot->left->box, newRoot->right->box);
			}

			return newRoot;
		}

		int BuildLinearTree(BNode* node)
		{
			if (node)
			{
				LBNode newLb;
				newLb.obj = node->obj;
				newLb.box = node->box;
				m_ltree.push_back(newLb);
				int curIndex = m_ltree.size() - 1;
				int tmp = BuildLinearTree(node->left);
				m_ltree[curIndex].left = tmp;
				tmp = BuildLinearTree(node->right);
				m_ltree[curIndex].right = tmp;
				return curIndex;
			}
			return -1;
		}

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			Stack<int>in_todo(m_ltree.size());
			int index = 0;
			in_todo.push(index);
			const LBNode* tmpNode;
			tmpNode = &m_ltree[index];
			while (!in_todo.isEmpty())
			{
				if (!in_todo.isEmpty())
					index = in_todo.top();
				tmpNode = &m_ltree[index];
				in_todo.pop();

				//  draw
				target.draw(tmpNode->box, states);

				if (tmpNode->right != -1)
				{
					in_todo.push(tmpNode->right);
				}
				if (tmpNode->left != -1)
				{
					in_todo.push(tmpNode->left);
				}
			}
		}

		/// <summary>
		/// Sort the list by centroid.x and then centroid.y
		/// </summary>
		/// <param name="list">list</param>
		/// <param name="size">size of list</param>
		static void SortList(Hitable** list, size_t size, bool byX)
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
						if (xbox.GetCentroid().x - ybox.GetCentroid().x < 0.0f)
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
						if (xbox.GetCentroid().y - ybox.GetCentroid().y < 0.0f)
							return -1;
						return 1;
					});
			}
		}

		/***********************************************************/

		BNode* m_root;

		vector<LBNode> m_ltree;

		//struct TodoRecord
		//{
		//	TodoRecord() {}
		//	TodoRecord(const int& Index) :index(Index) {}
		//	int index = -1;
		//	bool isHited = false;
		//	HitRecord rec;
		//};

		Stack<int> todo;

	};
}
#endif // !BVHTREE_H
