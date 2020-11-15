#ifndef CAMERA_H
#define CAMERA_H
#include"Ray.h"
#include"RayMath.h"

namespace ry
{
	class Camera
	{
	public:

		Camera() {}

		Camera(Vector3 Left_bottom, Vector3 Origin, Vector3 Horizontal, Vector3 Vertical)
			: left_bottom_corner(Left_bottom),
			origin(Origin), horizontal(Horizontal), vertical(Vertical)
		{}

		/*
		* lookFrom 视线出发点
		* lookAt 视平面中心
		* upDir 旋转
		* fov 视角
		* aspect 宽高比
		*/
		Camera(const Vector3& lookFrom, const Vector3& lookAt,const Vector3& upDir,
			const float& fov, const float& aspect)
		{
			origin = lookFrom;
			float theta = fov * Pi / 180;
			float half_height = tan(theta / 2);
			float half_width = aspect * half_height;
			Vector3 w = Vector3::Normalize((lookAt - lookFrom));
			Vector3 u = Vector3::Normalize(Vector3::Cross(upDir, w));
			Vector3 v = Vector3::Normalize(Vector3::Cross(w, u));
			left_bottom_corner = origin + w - half_width * u - half_height * v;
			horizontal = 2 * half_width * u;
			vertical = 2 * half_height * v;
		}

		Ray GetRay(const float& u, const float& v)
		{
			return Ray(origin, left_bottom_corner 
				+ u * horizontal + v * vertical - origin);
		}

		Vector3 left_bottom_corner;

		Vector3 origin;

		Vector3 horizontal;

		Vector3 vertical;
	};
}
#endif