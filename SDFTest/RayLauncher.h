#pragma once
#include"Ray.h"
#include"Material.h"
#include"HitList.h"
#include<vector>
using namespace std;
class RayLauncher : public Drawable
{
public:

	RayLauncher(Vector2f Origin, Vector2f initDir, HitList* World, int MaxDepth)
		: ray(Origin, initDir), world(World)
	{
		
	}

	Vector2f origin;

	void Update(Vector2f mousePosition)
	{
		Vector2f newDir = mousePosition - ray.origin;

		ray.direction = newDir;

		Vector2f hitrec;
		while (RayTracer(ray, hitrec))
		{
			hitRecs.clear();
			hitRecs.append(Vertex(hitrec));
		}

	}

	inline bool RayTracer(const Ray& ray, Vector2f& result)
	{
		HitRecord rec;
		if (world->Hit(ray, 0.001f, MAX_FLOAT, rec))
		{
			Ray scattered;
			if (maxDepth > 0 && rec.mat->Scatter(ray, rec, scattered))
			{

			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	Ray ray;

	//Åö×²µã¼ÇÂ¼
	VertexArray hitRecs;

	HitList* world;

	int maxDepth;

private:

	//»­Ïß
	virtual void draw(RenderTarget& target, RenderStates states) const
	{
		target.draw(hitRecs, states);
	}
};

