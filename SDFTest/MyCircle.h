#pragma once
#include<SFML/Graphics.hpp>
using namespace sf;
#ifndef Pi
#define Pi acosf(-1)
#endif // !Pi

class MyCircle : public Drawable
{
public:
	MyCircle() {}
	MyCircle(float r, Vector2f pos, Color color)
		:radius(r), position(pos), printCount(80)
	{
		verts = new Vertex[printCount];
		for (size_t i = 0; i < printCount-1; i++)
		{
			verts[i]= Vertex(
				Vector2f(
					(r * cos(i * 2 * Pi / printCount-1)),
					(r * sin(i * 2 * Pi / printCount-1))
				) + pos,
				color
			);
		}
		verts[printCount - 1] = verts[0];
	}
	~MyCircle()
	{
		if (verts)
			delete verts;
	}
	int printCount;
	float radius;
	Vector2f position;

private:

	Vertex* verts;

	virtual void draw(RenderTarget& target, RenderStates states)const
	{
		target.draw(verts, printCount, PrimitiveType::LineStrip, states);
	}
};
