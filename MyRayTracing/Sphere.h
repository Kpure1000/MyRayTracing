#ifndef SPHERE_H
#define SPHERE_H
#include"GeometryHit.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{

	class Sphere : public GeometryHit
	{
	public:

		Sphere() {}

		Sphere(SdfSphere* sphere, Material* mat)
			: GeometryHit(sphere, mat)
		{
		}

	};

}

#endif // !SPHERE_H