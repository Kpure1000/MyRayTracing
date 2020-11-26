#ifndef AABB_H
#define AABB_H
#include"Ray.h"
#include<algorithm>
namespace ry
{

	inline float ffMin(const float& a, const float& b) { return a < b ? a : b; }

	inline float ffMax(const float& a, const float& b) { return a > b ? a : b; }

	/*Axis-aligned bounding box*/
	class AABB : public Drawable
	{
	public:

		AABB() {}

		AABB(const sf::Vector2f& pMin, const sf::Vector2f& pMax)
		{
			vMin = pMin;
			vMax = pMax;

			rect.setFillColor(sf::Color(0, 0, 0, 0));
			rect.setOutlineColor(sf::Color::White);
			rect.setOutlineThickness(2);
			rect.setPosition(pMin);
			rect.setSize(pMax);
		}

		bool Hit(const Ray& r, float tMin, float tMax)const
		{
			float div, t0, t1;
			div = 1.0f / r.direction.x;
			t0 = (vMin.x - r.origin.x) * div;
			t1 = (vMax.x - r.origin.x) * div;
			if (div < 0.0f)std::swap(t0, t1);
			tMin = t0 > tMin ? t0 : tMin;
			tMax = t1 < tMax ? t1 : tMax;
			if (tMax <= tMin)return false;

			div = 1.0f / r.direction.y;
			t0 = (vMin.y - r.origin.y) * div;
			t1 = (vMax.y - r.origin.y) * div;
			if (div < 0.0f)std::swap(t0, t1);
			tMin = t0 > tMin ? t0 : tMin;
			tMax = t1 < tMax ? t1 : tMax;
			if (tMax <= tMin)return false;

			return true;
		}

		static AABB Surrounding(const AABB& boxA, const AABB& boxB)
		{
			sf::Vector2f pMin(
				std::fmin(boxA.vMin.x, boxB.vMin.x),
				std::fmin(boxA.vMin.y, boxB.vMin.y)
			);
			sf::Vector2f pMax(
				std::fmax(boxA.vMax.x, boxB.vMax.x),
				std::fmax(boxA.vMax.y, boxB.vMax.y)
			);
			return AABB(pMin, pMax);
		}

		sf::Vector2f vMin, vMax;

		void SetColor(sf::Color cl)
		{
			rect.setOutlineColor(cl);
		}

	private:

		sf::RectangleShape rect;

		virtual void draw(RenderTarget& target, RenderStates states) const
		{
			target.draw(rect, states);
		}

	};

}

#endif // !AABB_H
