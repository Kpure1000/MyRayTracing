#ifndef RAYTRACER_H
#define RAYTRACER_H

#include"Color.h"

#include"HitList.h"
#include"Sphere.h"
#include"IntersectionHit.h"
#include"UnionHit.h"
#include"Rect.h"

#include"Camera.h"
#include"Material.h"
#include"Texture.h"
#include"Light.h"

#include"BVH.h"

namespace ry
{

	Vector3 RayTracer(const Ray& ray, Hitable* world, const int& maxDepth, int& deep);

	Vector3 RayTracer(const Ray& ray, Hitable* world, const int& maxDepth);

}


#endif // !RAYTRACER_H
