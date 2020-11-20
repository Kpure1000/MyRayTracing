#ifndef SKYBOX_H
#define SKYBOX_H
#include"Material.h"
#include"Sdf.h"
using namespace sdf;
namespace ry
{

	/*ʵ������һ�����Ժ���ͶӰ��box�ϣ��������������պм��ݣ�*/
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
