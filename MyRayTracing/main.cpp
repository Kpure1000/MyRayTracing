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
#include"RayMath.h"
#include"Material.h"

using namespace std;
using namespace ry;

Vector3 RayTracer(const Ray& ray, Hitable* world, int maxDepth)
{
	HitRecord rec;
	if (world->Hit(ray, 0.001f, MAX_FLOAT, rec))
	{
		Ray scattered;
		Vector3 attenuation;
		if (maxDepth > 0 && rec.mat->Scatter(ray, rec, attenuation, scattered))
		{
			return attenuation * RayTracer(scattered, world, maxDepth - 1);
		}
		else
		{
			return Vector3::Zero;
		}
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
	int nx = 512; //  宽
	int ny = 288; //  高
	int nChannel = 3; //  颜色通道数量
	int ns = 100; //  抗锯齿相关
	int maxTraceDepth = 50;

	Vector3 left_bottom_corner(-2.0f, -1.0f, -1.0f);
	Vector3 horizontal(4.0f, 0.0f, 0.0f);
	Vector3 vertival(0.0f, 2.0f, 0.0f);
	Vector3 origin(0, 0, 0);

	unsigned char* imageData = (unsigned char*)malloc(nx * ny * nChannel * sizeof(unsigned char));

	HitList world(4);
	world.list[0] = new Sphere({ 0,0,-1 }, 0.5f, new Lambertian({ 1.0f,1.0f,1.0f }));
	world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f, new Metal({ 0.5f,0.5f,0.5f }));
	world.list[2] = new Sphere({ 1,0,-1 }, 0.5f, new Lambertian({ 0.8f,0.6f,0.2f }));
	world.list[3] = new Sphere({ -1,0,-1 }, 0.5f, new Metal({ 0.7f,0.4f,0.9f }));
	Camera camera({ -2.0f,-1.0f,-1.0f }, Vector3::Zero, { 4.0f,0.0f,0.0f }, { 0.0f,2.0f,0.0f });
	Ray r;
	Vector3 color;

	cout << "渲染质量: " << nx << "*" << ny << ", 抗锯齿: "
		<< ns << ", 探测深度: " << maxTraceDepth << ".\n开始渲染...\n";

	auto startTime = clock(); //  开始时间

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
				color += RayTracer(r, &world, maxTraceDepth);
			}
			color /= float(ns);
			color = Vector3(sqrtf(color[0]), sqrtf(color[1]), sqrtf(color[2]));
			imageData[j * nx * nChannel + i * nChannel] = int(TO_RGB * color[0]);
			imageData[j * nx * nChannel + i * nChannel + 1] = int(TO_RGB * color[1]);
			imageData[j * nx * nChannel + i * nChannel + 2] = int(TO_RGB * color[2]);
		}
	}

	cout << "离线渲染结束,用时: " << (float)(clock() - startTime) / 1000.0f << "s. 正在输出图像..." << endl;

	stbi_flip_vertically_on_write(true);
	stbi_write_bmp("outImage.bmp", nx, ny, nChannel, imageData);
	cout << "输出图像完毕" << endl;
	system("outImage.bmp");
	free(imageData);
}