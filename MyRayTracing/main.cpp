#include<iostream>
#include<fstream>
#include<ctime>

#include"Vector3.h"
#include"Ray.h"
#include<stb/stb_image_write.h>

using namespace std;
using namespace ry;


float GetRayParamFromHitSphere(Vector3 center, float radius, Ray ray)
{
	auto oc = ray.Origin() - center;
	float a = Vector3::Dot(ray.Direction(), ray.Direction());
	float b = 2.0f * Vector3::Dot(oc, ray.Direction());
	float c = Vector3::Dot(oc, oc) - radius * radius;
	float discriminant = b * b - 4 * a * c; //  判别式
	if (discriminant >= 0)
	{
		return (-b - sqrt(discriminant)) / (2.0f * a);
	}
	return -1.0f;
}
Vector3 GetColorFromRay(Ray& ray)
{
	Vector3 center = { 0,0,-100 };
	float redius = 50.0f;
	float t = GetRayParamFromHitSphere(center, redius, ray);
	Vector3 normal;
	if (t > 0.0f)
	{
		normal = (ray.PointTo(t) - center).Normalize();
		return 0.5f * Vector3(normal[0] + 1, normal[1] + 1, normal[2] + 1);
	}
	normal = ray.Direction().Normalize();
	t = 0.5f * (normal.y() + 1.0f);
	return (1.0f - t) * Vector3::one() + t * Vector3(0.5f, 0.7f, 1.0f);
}

int main()
{
	auto startTime = clock();
	int nx = 1024, ny = 768, nChannel = 3;

	Vector3 lower_left_corner(-2.0f, -1.0f, -1.0f);
	Vector3 horizontal(4.0f, 0.0f, 0.0f);
	Vector3 vertival(0.0f, 2.0f, 0.0f);
	Vector3 origin(0, 0, 0);

	unsigned char* imageData = (unsigned char*)malloc(nx * ny * nChannel * sizeof(unsigned char));

	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);
			Ray r(origin, lower_left_corner + u * horizontal + v * vertival);
			Vector3 color = GetColorFromRay(r);

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