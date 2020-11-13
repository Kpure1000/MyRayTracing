#include<iostream>
#include<fstream>
#include<ctime>

#include"Vector3.h"
#include"Ray.h"
#include"Sphere.h"
#include"HitList.h"
#include<stb/stb_image_write.h>

using namespace std;
using namespace ry;

Vector3 GetColorFromRay(Ray& ray, Hitable* world)
{
	HitRecord rec;
	if (world->Hit(ray, 0.0001f, 1000.0f, rec))
	{
		return 0.5f * Vector3(rec.normal[0] + 1, rec.normal[1] + 1, rec.normal[2] + 1);
	}
	else
	{
		Vector3 space = ray.Direction().Normalize();
		float t = 0.5f * (space[1] + 1.0f);
		return (1.0f - t) * Vector3::One + t * Vector3(0.5f, 0.7f, 1.0f);
	}
}

int main()
{
	auto startTime = clock();
	int nx = 512, ny = 288, nChannel = 3;

	Vector3 left_bottom_corner(-2.0f, -1.0f, -1.0f);
	Vector3 horizontal(4.0f, 0.0f, 0.0f);
	Vector3 vertival(0.0f, 2.0f, 0.0f);
	Vector3 origin(0, 0, 0);

	unsigned char* imageData = (unsigned char*)malloc(nx * ny * nChannel * sizeof(unsigned char));

	HitList world(2);
	world.list[0] = new Sphere({ 0,0,-1 }, 0.5f);
	world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f);

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);
			Ray r(origin, left_bottom_corner + u * horizontal + v * vertival);
			Vector3 color = GetColorFromRay(r, &world);

			imageData[j * nx * nChannel + i * nChannel] = int(255.99 * color[0]);
			imageData[j * nx * nChannel + i * nChannel + 1] = int(255.99 * color[1]);
			imageData[j * nx * nChannel + i * nChannel + 2] = int(255.99 * color[2]);
		}
	}

	cout << "离线渲染结束,用时: " << (float)(clock() - startTime) / 1000.0f << "s. 准备输出图像..." << endl;

	stbi_flip_vertically_on_write(true);
	stbi_write_bmp("outImage.bmp", nx, ny, nChannel, imageData);
	cout << "输出图像完毕" << endl;
	free(imageData);
}