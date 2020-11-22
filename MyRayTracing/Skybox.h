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
		Skybox() :material(nullptr)
		{
			for (size_t i = 0; i < 6; i++)
			{
				box[i] = nullptr;
			}
		}

		Skybox(Material* mat) :material(mat)
		{
			for (size_t i = 0; i < 6; i++)
			{
				box[i] = nullptr;
			}
		}

		Skybox(const char* left,
			const char* right,
			const char* top,
			const char* bottom,
			const char* front,
			const char* back)
			:material(nullptr)
		{
			box[0] = new Illumination(new Image_Texture(left), 1.0f);
			box[1] = new Illumination(new Image_Texture(right), 1.0f);
			box[2] = new Illumination(new Image_Texture(top), 1.0f);
			box[3] = new Illumination(new Image_Texture(bottom), 1.0f);
			box[4] = new Illumination(new Image_Texture(front), 1.0f);
			box[5] = new Illumination(new Image_Texture(back), 1.0f);
		}

		Vector3 GetSky(const Vector3& normal)
		{
			if (material)
			{
				float u, v;
				SdfSphere::GetSphereUV(u, v, normal);
				return material->Emitted(u, v, normal);
			}
			else
			{
				float u, v;
				for (size_t i = 0; i < 6; i++)
				{
					if (box[i])box[i]->Emitted(u, v, normal);
				}
			}
			return { 0,0,0 };
		}

		Material* material;

		Material* box[6];

	};

}
#endif // !SKYBOX_H
