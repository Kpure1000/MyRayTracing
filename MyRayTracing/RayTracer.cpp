#include"Scence.h"

namespace ry
{
	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth)
	{
		HitRecord rec;
		//ע�����tMin����̫С��������ܳ������ཻ���⣨��pbrt_cn p88)
		if (world->GetBVH()->Hit(ray, 0.001f, MAX_FLOAT, rec))
		{
			Ray scattered;
			Vector3 attenuation;
			Vector3 emitted = rec.mat->Emitted(rec.u, rec.v, rec.hitPoint);
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
			return world->GetSkybox()->GetSky(Vector3::Normalize(ray.direction));
		}
	}

	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth, int& deep)
	{
		HitRecord rec;
		if (world->GetBVH()->Hit(ray, 0.001f, MAX_FLOAT, rec))
		{
			Ray scattered;
			Vector3 attenuation;
			Vector3 emitted = rec.mat->Emitted(rec.u, rec.v, rec.hitPoint);
			if (maxDepth > 0 && rec.mat->Scatter(ray, rec, attenuation, scattered))
			{
				deep++;
				return emitted + attenuation * RayTracer(scattered, world, maxDepth - 1, deep);
			}
			else
			{
				return emitted;
			}
		}
		else
		{
			return world->GetSkybox()->GetSky(Vector3::Normalize(ray.direction));
		}
	}


}
