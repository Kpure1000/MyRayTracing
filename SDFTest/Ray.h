#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;

namespace ry
{
	class Ray
	{
	public:

		Ray() {}

		Ray(const Vector2f& a, const Vector2f& b)
			: origin(a), direction(b)
		{}

		Vector2f Origin()const { return origin; }

		Vector2f Direction()const { return direction; }

		Vector2f PointTo(const float& t)const { return origin + t * direction; }

		Vector2f origin;
		Vector2f direction;
	};


}