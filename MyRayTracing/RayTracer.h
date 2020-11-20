#ifndef RAYTRACER_H
#define RAYTRACER_H

#include"Color.h"

#include"HitList.h"
#include"Sphere.h"
#include"IntersectionHit.h"
#include"UnionHit.h"
#include"Rect.h"
#include"Skybox.h"

#include"Camera.h"
#include"Material.h"
#include"Texture.h"
#include"Light.h"

#include"BVH.h"

class Scence;

namespace ry
{
	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth);

	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth, int& deep);

}


#endif // !RAYTRACER_H
