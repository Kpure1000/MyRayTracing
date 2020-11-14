#include<iostream>
#include<fstream>
#include<ctime>
#include<random>
#include<thread>
#include<stb/stb_image_write.h>

#include"Vector3.h"
#include"Ray.h"
#include"Sphere.h"
#include"HitList.h"
#include"Camera.h"
#include"RayMath.h"
#include"Material.h"
#include"Color.h"

#ifndef __linux__
#include<Windows.h>
#endif // !__linux__

#define MULTI_THREAD 

using namespace std;
using namespace ry;

Vector3 RayTracer(const Ray& ray, Hitable* world, const int& maxDepth)
{
	//Vector3 result;
	//Ray newRay = ray;
	//int MaxDeep = maxDepth;
	//while (1)
	//{
	//	HitRecord rec;
	//	if (world->Hit(ray, 0.001f, MAX_FLOAT, rec))
	//	{
	//		Ray scattered;
	//		Vector3 attenuation;
	//		if (MaxDeep > 0 && rec.mat->Scatter(newRay, rec, attenuation, scattered))
	//		{
	//			newRay = scattered;
	//			result *= attenuation;
	//		}
	//		else
	//		{
	//			//result *= Vector3::Zero;
	//			break;
	//		}
	//	}
	//	else
	//	{
	//		Vector3 sky = newRay.Direction().Normalize();
	//		float t = 0.5f * (sky[1] + 1.0f);
	//		//result *= ((1.0f - t) * Vector3::One + t * Vector3(0.5f, 0.7f, 1.0f));
	//		break;
	//	}
	//	MaxDeep--;
	//}
	//return result;

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
		Vector3 sky = ray.Direction().Normalize();
		float t = 0.5f * (sky[1] + 1.0f);
		return (1.0f - t) * Vector3::One + t * Vector3(0.5f, 0.7f, 1.0f);
	}

}

void RayTraceThread(int start, int end, unsigned char* imageData, int nx, int ny, int nChannel, int ns,
	Camera* camera, HitList* world, int maxTraceDepth, float* endNumber);

void funcTest(int n1, int n2, int n3, int n4, int n5, int n6, int n7, int n8, int n9, int n10, int n11) {}

int main()
{
	int nx = 2560; //  宽
	int ny = 1440; //  高
	int nChannel = 3; //  颜色通道数量
	int ns = 10000; //  抗锯齿(蒙特卡洛采样)
	int maxTraceDepth = 70;

	Vector3 left_bottom_corner(-2.0f, -1.0f, -1.0f);
	Vector3 horizontal(4.0f, 0.0f, 0.0f);
	Vector3 vertival(0.0f, 2.0f, 0.0f);
	Vector3 origin(0, 0, 0);

	unsigned char* imageData = (unsigned char*)malloc(sizeof(unsigned char) * nx * ny * nChannel);

	HitList world(10);
	//world.list[0] = new Sphere({ 0,0,-1 }, 0.5f, new Lambertian({ 0.8f,0.3f,0.3f }));
	//world.list[0] = new Sphere({ 0,0.0f,-1 }, 0.5f, new Metal({ 1.0f,1.0f,1.0f }, 1.8f));
	world.list[0] = new Sphere({ 0,0.0f,-1 }, 0.5f, new Dielectric({ 1,1,1 }, 1.3f));

	world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f, new Lambertian({ 0.8f,0.8f,0.0f }));
	//world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f, new Metal({ 0.5f,0.5f,0.5f }, 0.4f));
	//world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f, new Dielectric({ 1,1,1 },1.4f));

	world.list[2] = new Sphere({ 1,0,-1 }, 0.5f, new Lambertian({ 0.8f,0.6f,0.2f }));
	//world.list[2] = new Sphere({ 1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.6f,0.2f }, 0.2f));
	//world.list[2] = new Sphere({ 1,0,-1 }, 0.5f, new Dielectric({ 1,1,1 }, 1.3f));

	//world.list[3] = new Sphere({ -1,0,-1 }, 0.5f, new Lambertian({ 0.7f,0.4f,0.9f }));
	world.list[3] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	//world.list[3] = new Sphere({ -1,0,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.3f));

	//world.list[4] = new Sphere({ 1,1,-1 }, 0.5f, new Lambertian({ 0.4f,0.9f,0.3f }));
	//world.list[4] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	world.list[4] = new Sphere({ 1,1,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.3f));

	//world.list[5] = new Sphere({ 0,1,-1 }, 0.5f, new Lambertian({ 0.6f,0.6f,0.2f }));
	//world.list[5] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	world.list[5] = new Sphere({ 0,1,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.3f));

	//world.list[6] = new Sphere({ -1,1,-1 }, 0.5f, new Lambertian({ 0.6f,0.1f,0.8f }));
	//world.list[6] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	world.list[6] = new Sphere({ -1,1,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.3f));

	world.list[7] = new Sphere({ 1,0.5f,-2 }, 0.5f, new Lambertian({ 0.4f,0.9f,0.3f }));
	//world.list[7] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	//world.list[7] = new Sphere({ -1,0,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));

	world.list[8] = new Sphere({ 0,0.5f,-2 }, 0.5f, new Lambertian({ 0.6f,0.6f,0.2f }));
	//world.list[8] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	//world.list[8] = new Sphere({ -1,0,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));

	world.list[9] = new Sphere({ -1,0.5f,-2 }, 0.5f, new Lambertian({ 0.6f,0.1f,0.8f }));
	//world.list[9] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	//world.list[9] = new Sphere({ -1,0,-1 }, 0.5f, new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));

	Camera camera({ -2.0f,-1.0f,-1.0f }, { 0,0,0.4f }, { 4.0f,0.0f,0.0f }, { 0.0f,2.0f,0.0f });
	Ray r;
	Color color;

	cout << "渲染质量: " << nx << "*" << ny << ", 蒙特卡洛采样次数: "
		<< ns << ", 探测深度: " << maxTraceDepth << ".\n";

	auto startTime = clock(); //  开始时间

	if (nChannel > 4)
	{
		cout << "颜色通道大于4, 退出.\n";
		return 1;
	}
	if (imageData == nullptr)
	{
		cout << "申请图形空间错误, 退出.\n";
		return 1;
	}

#ifdef MULTI_THREAD
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int coreNum = (int)(systemInfo.dwNumberOfProcessors);
	int threadNum = coreNum - 1;
	int taskNum = threadNum * 6;

	thread** rtThread = (thread**)malloc(sizeof(thread*) * threadNum);

	float* endFlag = (float*)malloc(sizeof(float) * threadNum);

	int curTask = 0;

	for (; curTask < threadNum; curTask++)
	{
		printf("开启第%d个任务\n", curTask + 1);
		endFlag[curTask] = 0.0f;
		rtThread[curTask] = new thread(RayTraceThread,
			(int)(curTask * ny / taskNum), (int)((curTask + 1) * ny / taskNum), imageData,
			nx, ny, nChannel, ns, &camera, &world, maxTraceDepth, &endFlag[curTask]);
	}

	for (int i = 0; i < threadNum; i++)
	{
		rtThread[i]->detach();
	}

	int endNum;

	int curEndTask = 0;

	float curRate = 0.0f;
	auto sTime = 0;
	int cTime = sTime;
	while (1)
	{
		endNum = 0;
		curRate = 0.0f;
		for (int i = 0; i < threadNum; i++)
		{
			if (endFlag[i] > 1.01f)
			{
				if (curTask < taskNum)
				{
					//printf("开启第%d个任务\n", curTask + 1);
					delete rtThread[i];
					endFlag[i] = 0.0f;
					rtThread[i] = new thread(RayTraceThread,
						(int)(curTask * ny / taskNum), (int)((curTask + 1) * ny / taskNum), imageData,
						nx, ny, nChannel, ns, &camera, &world, maxTraceDepth, &endFlag[i]);
					curTask++;
					curEndTask++;
					rtThread[i]->detach();
				}
				else
				{
					endNum++;
					if (endFlag[i] < 1.8f)
						curEndTask++;
					endFlag[i] = 2.0f;
				}
			}
			else
			{
				curRate += endFlag[i];
			}
		}
		sTime += clock() - cTime;
		cTime = clock();
		if (sTime >= 100)
		{
			printf("进度: %3.0f%%\n", (curRate + curEndTask) / (float)(taskNum) * 100);
			sTime = 0;
		}

		if (endNum == threadNum && curTask == taskNum)
		{
			printf("完成: %3.0f%%\n", (curTask) / (float)(taskNum) * 100);
			break;
		}
	}

	for (int i = 0; i < threadNum; i++)
	{
		delete rtThread[i];
	}

	free(rtThread);
	free(endFlag);

#else
	for (int j = 0; j < ny; j++)
	{
		for (int i = 0; i < nx; i++)
		{
			color.rgb = Vector3::Zero;
			float u, v;
			for (int k = 0; k < ns; k++)
			{
				u = float(i + rand() % 100 / (float)100) / float(nx);
				v = float(j + rand() % 100 / (float)100) / float(ny);
				r = camera.GetRay(u, v);
				color.rgb += RayTracer(r, &world, maxTraceDepth);
	}
			color.rgb /= float(ns);
			color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));
			for (int ch = 0; ch < nChannel; ch++)
			{
				imageData[j * nx * nChannel + i * nChannel + ch] = (unsigned char)(TO_RGB * color[ch]);
			}
}
	}
#endif // MULTI_THREAD

	cout << "离线渲染结束,用时: " << (float)(clock() - startTime) / 1000.0f << "s. 正在输出图像..." << endl;

	stbi_flip_vertically_on_write(true);
	stbi_write_bmp("outImage.bmp", nx, ny, nChannel, imageData);
	cout << "输出图像完毕" << endl;
	system("outImage.bmp");
	system("git add -A;git commit -m \"add:multi-thread and processing show\"");
	system("git push origin master");
	system("shutdown -s -t 5");
	free(imageData);

}

void RayTraceThread(int start, int end, unsigned char* imageData, int nx, int ny, int nChannel, int ns,
	Camera* camera, HitList* world, int maxTraceDepth, float* endNumber)
{
	Color color;
	Ray r;
	//int task = start * (float)(end - start);
	for (int j = start; j < end; j++)
	{
		for (int i = 0; i < nx; i++)
		{
			color.rgb = Vector3::Zero;
			float u, v;
			for (int k = 0; k < ns; k++)
			{
				u = float(i + rand() % 100 / (float)100) / float(nx);
				v = float(j + rand() % 100 / (float)100) / float(ny);
				r = (*camera).GetRay(u, v);
				color.rgb += RayTracer(r, world, maxTraceDepth);
			}
			color.rgb /= float(ns);
			color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));
			for (int ch = 0; ch < nChannel; ch++)
			{
				imageData[j * nx * nChannel + i * nChannel + ch] = (unsigned char)(TO_RGB * color[ch]);
			}
		}
		(*endNumber) = (float)(j - start) / (float)(end - start);
	}
	(*endNumber) = 1.1f;
	//printf("完成任务:%d\n", task);
}