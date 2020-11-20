#include"Scence.h"

namespace ry
{
	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth)
	{
		HitRecord rec;
		//注意这个tMin不能太小，否则可能出现自相交问题（见pbrt_cn p88)
		if (world->GetWorld()->Hit(ray, 0.001f, MAX_FLOAT, rec))
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
			/*Vector3 sky = ray.Direction().Normalize();
			float t = 0.5f * (sky[1] + 1.0f);
			return (1.0f - t) * Vector3(0.9f, 0.9f, 0.9f) + t * Vector3(0.3f, 0.5f, 0.9f);*/
			
			return world->GetSkybox()->GetSky(Vector3::Normalize(ray.direction));
		}

	}

	Vector3 RayTracer(const Ray& ray, Scence* world, const int& maxDepth, int& deep)
	{
		HitRecord rec;
		if (world->GetWorld()->Hit(ray, 0.001f, MAX_FLOAT, rec))
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
			/*Vector3 sky = ray.Direction().Normalize();
			float t = 0.5f * (sky[1] + 1.0f);
			return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.5f, 0.7f, 1.0f);*/

			return world->GetSkybox()->GetSky(Vector3::Normalize(ray.direction));
		}

	}


}
