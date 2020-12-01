#ifndef BVH_H
#define BVH_H
#include"Hitable.h"
#include"AABB.h"
#include<iomanip>
#include<memory>
#include<vector>
#include<iostream>
#include<functional>
using std::cout;
using std::setw;
using std::setfill;
using std::shared_ptr;
using std::vector;
namespace ry
{

	std::ostream& operator<<(std::ostream& s, const sf::Vector2f& v)
	{
		return s << "(" << v.x << ", " << v.y << ")";
	}

	class BTNode : public Drawable
	{
	public:
		BTNode() :obj(nullptr), left(nullptr), right(nullptr)
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
			if (box.Hit(r, tMin, tMax))
			{
				HitRecord recL, recR;
				bool isHitL = false, isHitR = false;

				if (left)isHitL = left->Hit(r, tMin, tMax, recL);
				if (right)isHitR = right->Hit(r, tMin, tMax, recR);

				if (isHitL && isHitR)
				{
					rec = recL.t < recR.t ? recL : recR;
					if (rec.t<tMax && rec.t>tMin)
						return true;
				}
				else if (isHitL)
				{
					if (recL.t<tMax && recL.t>tMin)
					{
						rec = recL;
						return true;
					}
				}
				else if (isHitR)
				{
					if (recR.t<tMax && recR.t>tMin)
					{
						rec = recR;
						return true;
					}
				}
				else if (obj != nullptr)
				{
					bool isHit = obj->Hit(r, tMin, tMax, rec);
					if (isHit)
					{
						if (rec.t<tMax && rec.t>tMin)
						{
							return true;
						}
					}
				}
				return false;
			}
			return false;
		}

		virtual bool GetBBox(AABB& box)const
		{
			box = this->box; return true;
		}

		static void Travle(BTNode* node, int depth, std::function<void(const int&, BTNode*)>const Visit)
		{
			if (node != nullptr)
			{
				if (node->left != nullptr)
					Travle((BTNode*)node->left, depth + 1, Visit);
				Visit(depth, node);
				if (node->right != nullptr)
					Travle((BTNode*)node->right, depth + 1, Visit);
			}
		}

		AABB box;
		Hitable* obj;
		BTNode* left, * right;

	private:

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			target.draw(box, states);
		}

	};

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
			m_root = BuildTree(pList, (size_t)size);
			/*BTNode::Travle(m_root, 1, [](const int& depth, BTNode* node)
				{
					cout << setw(6 * depth) << setfill(' ') << node->box.GetMax() - node->box.GetMin() << "\n";
				});*/
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

		BTNode* BuildTree(Hitable** list, size_t size)
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

			BTNode* newRoot = new BTNode();
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
				newRoot->left = new BTNode;
				newRoot->right = new BTNode;
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

		Hitable* DFTBVH(Hitable* tree)
		{
			return nullptr;
		}

		BTNode* m_root;

		Hitable** m_tree;

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			BTNode::Travle(m_root, 0, [&](const int& depth, BTNode* node)
				{
					target.draw(*node, states);
				});
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

	};

}
#endif // !BVH_H
