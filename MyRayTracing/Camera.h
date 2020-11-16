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
		Camera(const Vector3& lookFrom, const Vector3& lookAt, const Vector3& upDir,
			const float& fov, const float& aspect,
			const float& aperture, const float& focus_dist)
		{
			lensRadius = aperture / 2.;
			origin = lookFrom;
			float theta = fov * Pi / 180;
			float half_height = tan(theta / 2);
			float half_width = aspect * half_height;
			w = Vector3::Normalize((lookFrom - lookAt));
			u = Vector3::Normalize(Vector3::Cross(upDir, w));
			v = Vector3::Normalize(Vector3::Cross(w, u));
			left_bottom_corner = origin - w * focus_dist - half_width * focus_dist * u
				- half_height * focus_dist * v;
			horizontal = 2 * half_width * u * focus_dist;
			vertical = 2 * half_height * v * focus_dist;
		}

		Ray GetRay(const float& x, const float& y)
		{
			Vector3 rd = lensRadius * random_in_unit_disk();
			Vector3 offset = rd[0] * u + rd[1] * v;
			return Ray(origin + offset, left_bottom_corner
				+ x * horizontal + y * vertical - origin - offset);
		}

		Vector3 left_bottom_corner;

		Vector3 origin;

		Vector3 horizontal;

		Vector3 vertical;

		Vector3 u, v, w;

		float lensRadius;

	private:

		Vector3 random_in_unit_disk()
		{
			Vector3 p;
			do
			{
				p = 2.0f * Vector3(Drand48(), Drand48(), 0) - Vector3(1, 1, 0);
			} while (Vector3::Dot(p, p) >= 1.0f);
			return p;
		}
	};
}
#endif