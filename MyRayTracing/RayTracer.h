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
		if (maxDepth > 0 && rec.mat->Scatter(ray, rec, attenuation, scattered))
		{
			if (rec.isStoped)
			{
				return attenuation;
			}
			return attenuation * RayTracer(scattered, world, maxDepth - 1);
		}
		else
		{
			return Vector3::Zero;
		}
	}
	else
	{
		/*Vector3 sky = ray.Direction().Normalize();
		float t = 0.5f * (sky[1] + 1.0f);
		return (1.0f - t) * Vector3::One + t * Vector3(0.5f, 0.7f, 1.0f);*/
		return Vector3::Zero;
	}

}

#endif // !RAYTRACER_H
