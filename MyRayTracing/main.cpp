#include<iostream>
#include<fstream>
#include<ctime>
#include<random>
#include<stb/stb_image_write.h>

#include"Vector3.h"
#include"Ray.h"
#include"Sphere.h"
#include"HitList.h"
#include"Camera.h"

using namespace std;
using namespace ry;
#define SAD 255.99

#define MAX_FLOAT 1e3f

Vector3 GetColorFromRay(const Ray& ray, Hitable* world)
{
	HitRecord rec;
	if (world->Hit(ray, 0.001f, MAX_FLOAT, rec))
	{
		Vector3 target = rec.vertex + rec.normal + random_in_unit_ball();
		return 0.5f * GetColorFromRay(Ray(rec.vertex, target - rec.vertex), world);
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
	int nx = 409; int ny = 216;int nChannel = 3;
	int ns=100;

	Vector3 left_bottom_corner(-2.0f, -1.0f, -1.0f);
	Vector3 horizontal(4.0f, 0.0f, 0.0f);
	Vector3 vertival(0.0f, 2.0f, 0.0f);
	Vector3 origin(0, 0, 0);

	unsigned char* imageData = (unsigned char*)malloc(nx * ny * nChannel * sizeof(unsigned char));

	HitList world(2);
	world.list[0] = new Sphere({ 0,0,-1 }, 0.5f);
	world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f);
	Camera camera({ -2.0f,-1.0f,-1.0f }, Vector3::Zero, { 4.0f,0.0f,0.0f }, { 0.0f,2.0f,0.0f });
	Ray r;
	Vector3 color;

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			color = Vector3::Zero;
			float u, v;
			for (int k = 0; k < ns; k++)
			{
				u = float(i + rand() % 100 / (float)100) / float(nx);
				v = float(j + rand() % 100 / (float)100) / float(ny);
				r = camera.GetRay(u, v);
				color += GetColorFromRay(r, &world);
			}
			color /= float(ns);
			color = Vector3(sqrtf(color[0]), sqrtf(color[1]), sqrtf(color[2]));
			imageData[j * nx * nChannel + i * nChannel] = int(SAD * color[0]);
			imageData[j * nx * nChannel + i * nChannel + 1] = int(SAD * color[1]);
			imageData[j * nx * nChannel + i * nChannel + 2] = int(SAD * color[2]);
		}
	}

	cout << "离线渲染结束,用时: " << (float)(clock() - startTime) / 1000.0f << "s. 准备输出图像..." << endl;

	stbi_flip_vertically_on_write(true);
	stbi_write_bmp("outImage.bmp", nx, ny, nChannel, imageData);
	cout << "输出图像完毕" << endl;
	system("outImage.bmp");
	free(imageData);
}