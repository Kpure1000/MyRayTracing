#ifndef SKYBOX_H
#define SKYBOX_H
#include"Material.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{

	/*实际上是一个球（以后考虑投影到box上，方便别人做的天空盒兼容）*/
	class Skybox final
	{
	public:
		Skybox() :material(nullptr) {}

		Skybox(Material* mat) :material(mat)
		{

		}

		Vector3 GetSky(const Vector3& normal)
		{
			if (material)
			{
				float u, v;
				//SdfSphere::GetSphereUV(u, v, normal);
				return material->Emitted(u, v, normal);
			}
			return { 0,0,0 };
		}

		Material* material;

	};

}
#endif // !SKYBOX_H
