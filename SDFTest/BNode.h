#ifndef BNODE_H
#define BNODE_H
#include"AABB.h"
#include"Hitable.h"
namespace ry
{
	class BNode
	{
	public:
		BNode() :obj(nullptr), left(nullptr), right(nullptr)
		{}

		Hitable* obj;

		AABB box;

		BNode* left, * right;

	};

	class LBNode : public sf::Drawable
	{
	public:

		LBNode() :obj(nullptr), left(-1), right(-1) {}

		/// <summary>
		/// Box of Obj
		/// </summary>
		AABB box;

		/// <summary>
		/// Hitable Object
		/// </summary>
		Hitable* obj;

		/// <summary>
		/// Offset
		/// </summary>
		int left, right;

		int fix_data;

	private:

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			target.draw(box, states);
		}

	};

}
#endif // !BNODE_H