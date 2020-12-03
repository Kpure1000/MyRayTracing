#ifndef BVH_H
#define BVH_H
#include "Hitable.h"
#include"BVHNode.h"
#include"Stack.h"
#include"RayMath.h"
#include <memory>
namespace ry
{

	class BVH
	{
	public:

		BVH() :m_root(nullptr) {}

		BVH(Hitable** list, int n, float t0, float t1) :m_root(nullptr)
		{
			m_root = BuildBVH(list, n, t0, t1);
			BuildLinearTree(m_root);
			todo.SetSize(m_ltree.size());
		}

		bool Hit(const Ray& r, const float& tMin,
			const float& tMax, HitRecord& rec)
		{
			HitRecord tmpRec = rec;
			rec.t = tMax + 1.0f;
			int index = 0;
			todo.clear();
			todo.push(index);

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

			/*if (m_root)
				return m_root->Hit(r, tMin, tMax, rec);
			return false;*/

		}

		bool GetBBox(float t0, float t1, AABB& box)
		{
			if (m_root)
			{
				box = m_root->box;
				return true;
			}
			return false;
		}

	private:

		BNode* BuildBVH(Hitable** list, int n, float t0, float t1)
		{
			/*Vector3 vMin, vMax;
			AABB tmpBox;
			for (size_t i = 0; i < n; i++)
			{
				if (!list[i]->GetBBox(t0, t1, tmpBox))return nullptr;
				vMin[0] = tmpBox.rect[2][0] < vMin[0] ? tmpBox.rect[2][0] : vMin[0];
				vMin[1] = tmpBox.rect[2][1] < vMin[1] ? tmpBox.rect[2][1] : vMin[1];
				vMin[2] = tmpBox.rect[2][2] < vMin[2] ? tmpBox.rect[2][2] : vMin[2];
				vMax[0] = tmpBox.rect[2][0] > vMax[0] ? tmpBox.rect[2][0] : vMax[0];
				vMax[1] = tmpBox.rect[2][1] > vMax[1] ? tmpBox.rect[2][1] : vMax[1];
				vMax[2] = tmpBox.rect[2][2] > vMax[2] ? tmpBox.rect[2][2] : vMax[2];
			}
			if (vMax[0] - vMin[0] > vMax[1] - vMin[1]
				&& vMax[0] - vMin[0] > vMax[2] - vMin[2])
			{
				SortList(list, n, SortAxis::X_SORT);
			}
			else if (vMax[1] - vMin[1] > vMax[0] - vMin[0]
				&& vMax[1] - vMin[1] > vMax[2] - vMin[2])
			{
				SortList(list, n, SortAxis::Y_SORT);
			}
			else if (vMax[2] - vMin[2] > vMax[0] - vMin[0]
				&& vMax[2] - vMin[2] > vMax[1] - vMin[1])
			{
				SortList(list, n, SortAxis::Z_SORT);
			}*/

			BNode* newRoot = new BNode();
			if (n == 1)
			{
				newRoot->obj = list[0];
				AABB b;
				if (newRoot->obj->GetBBox(t0, t1, b))
				{
					newRoot->box = b;
				}
				else
				{
					std::cerr << "No bounding in self node\n";
				}
			}
			else if (n == 2)
			{
				newRoot->left = new BNode;
				newRoot->right = new BNode;
				newRoot->left->obj = list[0];
				newRoot->right->obj = list[1];
				AABB bl, br;
				if (!newRoot->left->obj->GetBBox(t0, t1, bl) 
					|| !newRoot->right->obj->GetBBox(t0, t1, br))
				{
					std::cerr << "No bounding in sub node\n";
				}
				newRoot->left->box = bl;
				newRoot->right->box = br;
				newRoot->box = AABB::UnionBox(bl, br);
			}
			else
			{
				int mid = n / 2;
				newRoot->left = BuildBVH(list, mid, t0, t1);
				newRoot->right = BuildBVH(list + mid, n - mid, t0, t1);
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

		enum class SortAxis
		{
			X_SORT,
			Y_SORT,
			Z_SORT
		};

		static void SortList(Hitable** list, size_t size, SortAxis sortAxis)
		{
			switch (sortAxis)
			{
			case SortAxis::X_SORT:
				std::qsort(list, size, sizeof(Hitable*), [](const void* x, const void* y)->int
					{
						AABB xbox, ybox;
						Hitable* hx = *(Hitable**)x;
						Hitable* hy = *(Hitable**)y;
						if (!hx->GetBBox(0, 0, xbox) || !hy->GetBBox(0, 0, ybox))
						{
							std::cerr << "No bounding in bnode\n";
						}
						if (xbox.rect[2][0] - ybox.rect[2][0] < 0.0f)
							return -1;
						return 1;
					});
				break;
			case SortAxis::Y_SORT:
				std::qsort(list, size, sizeof(Hitable*), [](const void* x, const void* y)->int
					{
						AABB xbox, ybox;
						Hitable* hx = *(Hitable**)x;
						Hitable* hy = *(Hitable**)y;
						if (!hx->GetBBox(0, 0, xbox) || !hy->GetBBox(0, 0, ybox))
						{
							std::cerr << "No bounding in bnode\n";
						}
						if (xbox.rect[2][1] - ybox.rect[2][1] < 0.0f)
							return -1;
						return 1;
					});
				break;
			case SortAxis::Z_SORT:
				std::qsort(list, size, sizeof(Hitable*), [](const void* x, const void* y)->int
					{
						AABB xbox, ybox;
						Hitable* hx = *(Hitable**)x;
						Hitable* hy = *(Hitable**)y;
						if (!hx->GetBBox(0, 0, xbox) || !hy->GetBBox(0, 0, ybox))
						{
							std::cerr << "No bounding in bnode\n";
						}
						if (xbox.rect[2][2] - ybox.rect[2][2] < 0.0f)
							return -1;
						return 1;
					});
				break;
			default:
				break;
			}
		}


		/// <summary>
		/// Root of BVH Bsp
		/// </summary>
		BNode* m_root;

		/// <summary>
		/// Linear BVH Bsp
		/// </summary>
		vector<LBNode> m_ltree;

		/// <summary>
		/// Todo stack
		/// </summary>
		Stack<int>todo;
	};



}

#endif // !BVH_H
