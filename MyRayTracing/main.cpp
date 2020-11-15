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

// 开启多线程
#define MULTI_THREAD 

// 用来折叠部分代码
#ifndef CODE_FOLDER
#define CODE_FOLDER
#endif // !CODE_FOLDER


using namespace std;
using namespace ry;

Vector3 RayTracer(const Ray& ray, Hitable* world, const int& maxDepth)
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
		Vector3 sky = ray.Direction().Normalize();
		float t = 0.5f * (sky[1] + 1.0f);
		return (1.0f - t) * Vector3::One + t * Vector3(0.5f, 0.7f, 1.0f);
	}

}

void RayTraceThread(int start, int end, unsigned char* imageData, int nx, int ny, int nChannel, int ns,
	Camera* camera, HitList* world, int maxTraceDepth, float* endNumber);

HitList *randomScence()
{
	int n = 500;
	Hitable** list = new Hitable * [n + 1];
	list[0] = new Sphere(SdfSphere({ 0,-1000,0 }, 1000), new Lambertian({ 0.2f,0.9f,0.2f }));
	int i = 1;
	for (int i = -11; i < 11; i++)
	{
		for (int j = -11; j < 11; j++)
		{
			float choose_mat = Drand48();
			Vector3 center(i + 0.9f * Drand48(), 0.2f, j + 0.9f * Drand48());
			if ((center - Vector3(4.0f, 0.2f, 0.0f)).Magnitude() > 0.9f)
			{
				if (choose_mat < 0.8f)
				{
					list[i++] = new Sphere(SdfSphere(center, 0.2), new Lambertian(
						{ (float)Drand48() * (float)Drand48() ,
						(float)Drand48() * (float)Drand48() ,
						(float)Drand48() * (float)Drand48() }));
				}
				if (choose_mat < 0.95f)
				{
					list[i++] = new Sphere(SdfSphere(center, 0.2f), new Metal({
						0.5f * (float)(1 + Drand48()),0.5f * (float)(1 + Drand48()) ,0.5f * (float)(1 + Drand48()) },
							0.5f * Drand48()));
				}
				else
				{
					list[i++] = new Sphere(SdfSphere(center, 0.2), new Dielectric({ 1,1,1 }, 1.5));
				}
			}
		}
	}

	return new HitList(list, i);
}

int run(int threadIndex, ofstream& out)
{
	out << "线程指数: " << threadIndex << "\n";

	int nx = 512; //  宽
	int ny = 288; //  高
	int nChannel = 3; //  颜色通道数量
	int ns = 100; //  抗锯齿(蒙特卡洛采样)
	int maxTraceDepth = 50;

	unsigned char* imageData = (unsigned char*)malloc(sizeof(unsigned char) * nx * ny * nChannel);

#pragma region worldInit
	HitList* world = new HitList(4);
	(*world).list[0] = new Sphere(SdfSphere({ 0,0,-1 }, 0.5f), new Lambertian({ 0.1f,0.2f,0.5f }));
	//world.list[0] = new Sphere({ 0,0.0f,-1 }, 0.5f, new Metal({ 1.0f,1.0f,1.0f }, 1.8f));
	//world.list[0] = new Sphere({ 0,0.0f,-1 }, 0.5f, new Dielectric({ 1,1,1 }, 1.3f));

	(*world).list[1] = new Sphere(SdfSphere({ 0,-100.5f,-1 }, 100.0f), new Lambertian({ 0.8f,0.8f,0.0f }));
	//world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f, new Metal({ 0.5f,0.5f,0.5f }, 0.3f));
	//world.list[1] = new Sphere({ 0,-100.5f,-5 }, 100.0f, new Dielectric({ 1,1,1 },1.4f));

	//world.list[2] = new Sphere({ 1,0,-1 }, 0.5f, new Lambertian({ 0.8f,0.6f,0.2f }));
	(*world).list[2] = new Sphere(SdfSphere({ 1.0f,0,-1 }, 0.5f), new Metal({ 0.8f,0.6f,0.2f }, 0.0f));
	//world.list[2] = new Sphere({ 1,0,-1 }, 0.5f, new Dielectric({ 1,1,1 }, 1.3f));

	//world.list[3] = new Sphere({ -1,0,-1 }, 0.5f, new Lambertian({ 0.7f,0.4f,0.9f }));
	//world.list[3] = new Sphere({ -1.0f,0,-1 }, 0.5f, new Metal({ 0.8f,0.8f,0.8f }, 0.0f));
	(*world).list[3] = new Sphere(SdfSphere({ -1,0,-1 }, 0.5f), new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));

	//HitList* world = randomScence();


#pragma endregion
	Vector3 lookFrom(-20,20, 10);
	Vector3 lookAt(0, 0, -1);
	float dist_to_focus = (lookFrom - lookAt).Magnitude();
	float aperture = 0.0f;
	//Camera camera({ -2.0f,-1.0f,-1.0f }, { 0,0,0.5f }, { 4.0f,0.0f,0.0f }, { 0.0f,2.0f,0.0f });
	Camera camera(lookFrom, lookAt,
		{ 0,1,0 }, 50, float(nx) / float(ny), aperture, dist_to_focus);
	Ray r;
	Color color;
	out << "第" << threadIndex << "次渲染准备\n";
	out << "渲染质量: " << nx << "*" << ny << ", 蒙特卡洛采样次数: "
		<< ns << ", 探测深度: " << maxTraceDepth << ".\n";
	cout << "渲染质量: " << nx << "*" << ny << ", 蒙特卡洛采样次数: "
		<< ns << ", 探测深度: " << maxTraceDepth << ".\n";
	if (nChannel > 4)
	{
		out << "颜色通道大于4, 退出.\n";
		cout << "颜色通道大于4, 退出.\n";
		return 1;
	}
	if (imageData == nullptr)
	{
		out << "申请图形空间错误, 退出.\n";
		cout << "申请图形空间错误, 退出.\n";
		return 1;
	}


#ifdef MULTI_THREAD
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int coreNum = (int)(systemInfo.dwNumberOfProcessors);
	int threadNum = coreNum - 2;
	int taskNum = threadNum * min(max(1, nx / 256), 20);
	//int taskNum = threadNum * threadIndex;
	out << "任务数: " << taskNum << "\n"; cout << "任务数: " << taskNum << "\n";
	cout << "准备开始渲染" << endl;
	system("pause");

	out << "开始渲染..." << "\n"; cout << "开始渲染..." << "\n";
	auto startTime = clock(); //  开始时间

	thread** rtThread = (thread**)malloc(sizeof(thread*) * threadNum);

	float* endFlag = (float*)malloc(sizeof(float) * threadNum);

	int curTask = 0;

	for (; curTask < threadNum; curTask++)
	{
		endFlag[curTask] = 0.0f;
		rtThread[curTask] = new thread(RayTraceThread,
			(int)(curTask * ny / taskNum), (int)((curTask + 1) * ny / taskNum), imageData,
			nx, ny, nChannel, ns, &camera, world, maxTraceDepth, &endFlag[curTask]);
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
					delete rtThread[i];
					endFlag[i] = 0.0f;
					rtThread[i] = new thread(RayTraceThread,
						(int)(curTask * ny / taskNum), (int)((curTask + 1) * ny / taskNum), imageData,
						nx, ny, nChannel, ns, &camera, world, maxTraceDepth, &endFlag[i]);
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
			system("cls");
			printf("进度: %3.0f%%\n", (curRate + curEndTask) / (float)(taskNum) * 100);
			sTime = 0;
		}

		if (endNum == threadNum && curTask == taskNum)
		{
			system("cls");
			printf("渲染完成.%3.0f%%\n\n", (curTask) / (float)(taskNum) * 100);
			out << "渲染完成.\n";
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

	float curRate = 0.0f;
	cout << "准备开始渲染" << endl;
	system("pause");
	out << "开始渲染..." << "\n"; cout << "开始渲染..." << "\n";
	cout << "开始渲染..." << "\n"; cout << "开始渲染..." << "\n";
	auto startTime = clock(); //  开始时间

	thread([](float* curRate)->void
		{
			auto sTime = 0;
			auto cTime = sTime;
			while ((*curRate) < 1.5f)
			{
				sTime += clock() - cTime;
				cTime = clock();
				if (sTime >= 100 && (*curRate) < 1.5f)
				{
					system("cls");
					printf("进度: %3.0f%%\n", (*curRate) * 100);
					sTime = 0;
				}
			}
		}, &curRate).detach();

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
					color.rgb += RayTracer(r, world, maxTraceDepth);
				}
				color.rgb /= float(ns);
				color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));
				for (int ch = 0; ch < nChannel; ch++)
				{
					imageData[j * nx * nChannel + i * nChannel + ch] = (unsigned char)(TO_RGB * color[ch]);
				}
			}
			curRate = j / (float)ny;
		}

		curRate = 2.0f;

#endif // MULTI_THREAD

		out << "第" << threadIndex << "次离线渲染结束,用时: "
			<< (float)(clock() - startTime) / 1000.0f
			<< "s.\n--------------------------\n";
		cout << "第" << threadIndex << "次离线渲染结束,用时: "
			<< (float)(clock() - startTime) / 1000.0f
			<< "s.\n--------------------------\n";
		stbi_flip_vertically_on_write(true);
		stbi_write_bmp("outImage.bmp", nx, ny, nChannel, imageData);
		cout << "输出图像完毕" << endl;
		system("outImage.bmp");
		free(imageData);

		return 0;
}

int main()
{
	Srand48((unsigned int)time(NULL));
	ofstream testOut("renderLog.txt");
	testOut << "渲染日志: \n\n";
	for (int i = 1; i <= 1; i++)
	{
		if (run(i, testOut) == 1)
			return 1;
	}
	testOut.close();
	system("pause");
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
				/*u = float(i + rand() % 100 / (float)100) / float(nx);
				v = float(j + rand() % 100 / (float)100) / float(ny);*/
				u = float(i + Drand48()) / float(nx);
				v = float(j + Drand48()) / float(ny);
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