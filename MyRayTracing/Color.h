#ifndef COLOR_H
#define COLOR_H
#include"Vector3.h"
#include"RayMath.h"
namespace ry
{

	class Color
	{

	public:

		Color() : rgb({ 0,0,0 }), alpha(1) {}

		Color(float r, float g, float b) :rgb({ r,g,b }), alpha(1) {}

		Color(Vector3 color) :rgb(color), alpha(1) {}

		Color(float r, float g, float b, float a) :rgb({ r,g,b }), alpha(a) {}

		Color(Vector3 color, float a) :rgb(color), alpha(a) {}

		Color(unsigned long color)
			:rgb({ (color >> 8 & 0xff) / TO_RGB,
				(color >> 4 & 0xff) / TO_RGB,
				(color >> 2 & 0xff) / TO_RGB }),
			alpha((color & 0xff) / TO_RGB)
		{}

		float operator[](const int& index)
		{
			if (index >= 0 && index < 3)
			{
				return rgb[index];
			}
			if (index == 3)
			{
				return alpha;
			}
			return 0.0f;
		}

		float r() { return rgb[0]; }
		float g() { return rgb[1]; }
		float b() { return rgb[2]; }

		Vector3 rgb;
		float alpha;
	};
}
#endif // !COLOR_H