#ifndef RAYTRACER_H
#define RAYTRACER_H

//hit
#include"HitList.h"
#include"IntersectionHit.h"
#include"UnionHit.h"
#include"DifferenceHit.h"

//hit geometry
#include"Sphere.h"
#include"Rect.h"
#include"Light.h"
#include"Box.h"

//render
#include"Skybox.h"
#include"Camera.h"
#include"Material.h"
#include"Texture.h"

//util
#include"Color.h"
#include"BVH.h"

class Scence;

namespace ry
{
	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth);

	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth, int& deep);
}


#endif // !RAYTRACER_H
