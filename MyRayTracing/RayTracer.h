#ifndef RAYTRACER_H
#define RAYTRACER_H
#include"HitList.h"
#include"RayMath.h"
#include"Material.h"
#include"Sphere.h"
#include"Light.h"
#include"Camera.h"
#include"Color.h"

#include"IntersectionHit.h"

using namespace ry;

Vector3 RayTracer(const Ray& ray, Hitable* world, const int& maxDepth)
{
	HitRecord rec;
	if (world->Hit(ray, 0.001f, MAX_FLOAT, rec))
	{
		Ray scattered;
		Vector3 attenuation;
		Vector3 emitted = rec.mat->Emitted(0, 0, { 0,0,0 });
		if (maxDepth > 0 && rec.mat->Scatter(ray, rec, attenuation, scattered))
		{
			return emitted + attenuation * RayTracer(scattered, world, maxDepth - 1);
		}
		else
		{
			return emitted;
		}
	}
	else
	{
		Vector3 sky = ray.Direction().Normalize();
		float t = 0.9f * (sky[1] + 1.0f);
		return (1.0f - t) * Vector3(0.5f, 0.5f, 0.0f) + t * Vector3(0.01f, 0.01f, 0.0f);
		//return Vector3::Zero;
	}

}

#endif // !RAYTRACER_H
