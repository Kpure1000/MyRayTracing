#ifndef BOX_H
#define BOX_H
#include"GeometryHit.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
	class Box : public GeometryHit
	{
	public:
		
		Box() {}

		Box(Sdf* box, Material* mat)
			:GeometryHit(box, mat)
		{

		}

	};
}

#endif // !BOX_H
