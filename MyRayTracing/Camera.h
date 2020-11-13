#ifndef CAMERA_H
#define CAMERA_H
#include"Ray.h"
namespace ry
{
	class Camera
	{
	public:

		Camera() {}

		Camera(Vector3 Left_bottom, Vector3 Origin, Vector3 Horizontal, Vector3 Vertical)
			: left_bottom_corner(Left_bottom), origin(Origin), horizontal(Horizontal), vertical(Vertical)
		{}

		Ray GetRay(const float& u, const float& v)
		{
			return Ray(origin, left_bottom_corner + u * horizontal + v * vertical - origin);
		}

		Vector3 left_bottom_corner;

		Vector3 origin;

		Vector3 horizontal;

		Vector3 vertical;
	};
}
#endif