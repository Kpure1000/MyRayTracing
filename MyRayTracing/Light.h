#ifndef LIGHT_H
#define LIGHT_H
#include"Hitable.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{
	class Light : public GeometryHit
	{
	public:

		Light() {}

		Light(Sdf* sdfObj, Material* mat)
			:GeometryHit(sdfObj, mat) {}

	};
}
#endif // !LIGHT_H
