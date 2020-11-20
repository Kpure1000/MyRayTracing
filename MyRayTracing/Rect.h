#ifndef RECT_H
#define RECT_H
#include "GeometryHit.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{

	class Rect : public GeometryHit
	{
	public:

		Rect() {}

		Rect(SdfRect* rect, Material* mat)
			: GeometryHit(rect, mat)
		{
		}

	};

}

#endif // !RECT_H