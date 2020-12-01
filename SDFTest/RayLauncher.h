#ifndef RAYLAUNCHER_H
#define RAYLAUNCHER_H
#include"Material.h"
#include"HitList.h"
#include"BVH.h"
#include<vector>
using namespace std;
using namespace sf;
namespace ry
{
	class RayLauncher : public Drawable
	{
	public:

		RayLauncher(Vector2f Origin, Vector2f initDir, HitList* World, int MaxDepth)
			: ray(Origin, initDir), world(World), maxDepth(MaxDepth),
			points(new CircleShape[MaxDepth]),
			verts(new Vertex[MaxDepth + 1]), hitType(HitType::LIST)
		{
			for (size_t i = 0; i < MaxDepth; i++)
			{
				points[i].setRadius(4);
				points[i].setFillColor(Color::Red);
				points[i].setOrigin(4, 4);
			}
			verts[0].position = Origin;
			verts[1].position = initDir * 100.0f + Origin;
		}

		RayLauncher(Vector2f Origin, Vector2f initDir, BVH* World, int MaxDepth)
			: ray(Origin, initDir), bvh(World), maxDepth(MaxDepth),
			points(new CircleShape[MaxDepth]),
			verts(new Vertex[MaxDepth + 1]), hitType(HitType::BVH)
		{
			for (size_t i = 0; i < MaxDepth; i++)
			{
				points[i].setRadius(4);
				points[i].setFillColor(Color::Red);
				points[i].setOrigin(4, 4);
			}
			verts[0].position = Origin;
			verts[1].position = initDir * 100.0f + Origin;
			for (size_t i = 0; i < MaxDepth + 1; i++)
			{
				verts[i].color = vertsColor;
			}
		}

		Vector2f origin;

		void Update(const Vector2f& mousePosition, const bool& isPress)
		{
			if (isPress)
			{
				ray.origin = mousePosition;
				verts[0].position = mousePosition;
			}
			Vector2f newDir = mousePosition - ray.origin;

			Ray newRay = ray;

			newRay.direction = newDir;

			Vector2f rec;
			curDepth = 0;
			hitRecs.clear();
			while (1)
			{
				if (hitType == HitType::LIST)
				{
					if (!RayTracer(newRay, rec))break;
				}
				else if (hitType == HitType::BVH)
				{
					if (!BVH_RayTracer(newRay, rec))break;
				}
				hitRecs.push_back(rec);
				points[curDepth].setPosition(rec);
				verts[curDepth + 1].position = rec;
					curDepth++;
			}

			if (hitRecs.size() == 0)
			{
				rec = ray.origin + Normalize(newRay.PointTo(1) - ray.origin) * 900.0f;

				verts[1].position = rec;
			}
			else
			{
				rec = verts[curDepth].position 
					+ Normalize(newRay.PointTo(1) - verts[curDepth].position) * 900.0f;
				verts[curDepth + 1].position = rec;
			}

		}

		Ray ray;

		//碰撞点记录
		vector<Vector2f>hitRecs;

		Vertex* verts;

		CircleShape* points;

		HitList* world;

		BVH* bvh;

		int maxDepth;

		int curDepth;

		sf::Color vertsColor = sf::Color::White;

	private:

		enum class HitType
		{
			LIST,
			BVH
		};

		HitType hitType;

		bool RayTracer(Ray& ray, Vector2f& result)
		{
			HitRecord rec;
			if (world->Hit(ray, 0.001f, MAX_FLOAT, rec))
			{
				//std::cout << "碰撞-";
				Ray scattered;
				if (curDepth < maxDepth && rec.mat->Scatter(ray, rec, scattered))
				{
					//std::cout << "求交  ";
					result = rec.hitPoint;
					ray = scattered;
					return true;
				}
			}
			return false;
		}

		bool BVH_RayTracer(Ray& ray, Vector2f& result)
		{
			HitRecord rec;
			if (bvh->Hit(ray, 0.001f, MAX_FLOAT, rec))
			{
				//std::cout << "碰撞-";

				Ray scattered;
				if (curDepth < maxDepth && rec.mat->Scatter(ray, rec, scattered))
				{
					//std::cout << "求交  ";
					result = rec.hitPoint;
					ray = scattered;
					return true;
				}
			}
			return false;
		}

		//画线
		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			for (size_t i = 0; i < curDepth; i++)
			{
				target.draw(points[i], states);
			}
			if (curDepth < maxDepth)
				target.draw(verts, curDepth + 2, PrimitiveType::LineStrip, states);
			else 
				target.draw(verts, curDepth + 1, PrimitiveType::LineStrip, states);
			
		}
	};
}


#endif // !RAYLAUNCHER_H

