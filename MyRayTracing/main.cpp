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

#include"IntersectionHit.h"

#ifndef __linux__
#include<Windows.h>
#endif // !__linux__

// �������߳�
#define MULTI_THREAD 

// �����۵����ִ���
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

HitList *randomScence(int maxSize,int randomIndex)
{
	Hitable** list = new Hitable * [maxSize + 1];
	list[0] = new Sphere(SdfSphere({ 0,-1000.5f,-1 }, 1000.0f), new Lambertian({ 0.3f,0.3f,0.3f }));
	int i = 1;
	for (int x = -randomIndex; x < randomIndex; x++)
	{
		for (int y = -randomIndex; y < randomIndex; y++)
		{
			float choose_mat = Drand48();
			Vector3 center(x + 0.9f * Drand48(), 0.2f, y + 0.9f * Drand48());
			if ((center - Vector3(4.0f, 0.2f, 0.0f)).Magnitude() > 0.9f)
			{
				if (choose_mat < 0.8f)
				{
					list[i++] = new Sphere(SdfSphere(center, 0.2f), new Lambertian(
						{ (float)Drand48() * (float)Drand48() ,
						(float)Drand48() * (float)Drand48() ,
						(float)Drand48() * (float)Drand48() }));
				}
				else if (choose_mat < 0.95f)
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
	out << "�߳�ָ��: " << threadIndex << "\n";

	int nx = 512; //  ��
	int ny = 288; //  ��
	int nChannel = 3; //  ��ɫͨ������
	int ns = 50; //  �����(���ؿ������)
	int maxTraceDepth = 50;

	unsigned char* imageData = (unsigned char*)malloc(sizeof(unsigned char) * nx * ny * nChannel);

#pragma region worldInit

	//int hitableNum = 4;
	//HitList* world = new HitList(hitableNum);

	HitList* world = NULL;
	int MaxWorldSize = 500;
	world = randomScence(MaxWorldSize, 0);
	(*world).size += 2;
	/*(*world).list[(*world).size - 3] = new Sphere(SdfSphere({ 0,0,-1 }, 0.5f), new Lambertian({ 0.1f,0.2f,0.5f }));

	(*world).list[(*world).size - 2] = new Sphere(SdfSphere({ 1.0f,0,-1 }, 0.5f), new Metal({ 0.8f,0.6f,0.2f }, 0.0f));

	(*world).list[(*world).size - 1] = new Sphere(SdfSphere({ -1,0,-1 }, 0.5f), new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));*/

	/*world->list[world->size - 1] = new IntersectionHit(
		new SdfSphere({ 0.0f,0.0f,0.0f }, 1.0f),
		new SdfSphere({ 0.0f, 0.0f, 0.7f }, 1.0f),
		new Dielectric({ 1.0f,1.0f,1.0f },1.5f)
	);*/

	world->list[world->size - 2] = new Sphere(SdfSphere({ 0.0f,0.0f,0.0f }, 1.0f), new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));
	world->list[world->size - 1] = new Sphere(SdfSphere({ 0.0f,0.0f,0.7f }, 1.0f), new Dielectric({ 1.0f,1.0f,1.0f }, 1.5f));

#pragma endregion
	Vector3 lookFrom(-6.0f,2.9f, 3.8f);
	Vector3 lookAt(0.0f, 0.5f, -1.0f);
	float dist_to_focus = (lookFrom - lookAt).Magnitude();
	float aperture = 0.1f;
	//Camera camera({ -2.0f,-1.0f,-1.0f }, { 0,0,0.5f }, { 4.0f,0.0f,0.0f }, { 0.0f,2.0f,0.0f });
	Camera camera(lookFrom, lookAt,
		{ 0,1,0 }, 30, float(nx) / float(ny), aperture, dist_to_focus);
	Ray r;
	Color color;
	out << "��" << threadIndex << "����Ⱦ׼��\n";
	out << "��Ⱦ����: " << nx << "*" << ny << ", ���ؿ����������: "
		<< ns << ", ̽�����: " << maxTraceDepth << ".\n";
	cout << "��Ⱦ����: " << nx << "*" << ny << ", ���ؿ����������: "
		<< ns << ", ̽�����: " << maxTraceDepth << ".\n";
	out << "����������:" << (*world).size << ", ��Ȧ: " << aperture << ".\n";
	if (nChannel > 4)
	{
		out << "��ɫͨ������4, �˳�.\n";
		cout << "��ɫͨ������4, �˳�.\n";
		return 1;
	}
	if (imageData == nullptr)
	{
		out << "����ͼ�οռ����, �˳�.\n";
		cout << "����ͼ�οռ����, �˳�.\n";
		return 1;
	}


#ifdef MULTI_THREAD
	SYSTEM_INFO systemInfo;
	GetSystemInfo(&systemInfo);
	int coreNum = (int)(systemInfo.dwNumberOfProcessors);
	int threadNum = coreNum - 2;
	int taskNum = threadNum * min(max(1, nx / 256), 20);
	//int taskNum = threadNum * threadIndex;
	out << "��Ⱦģʽ: " << "���߳�; ";
	out << "������: " << taskNum << "\n"; cout << "������: " << taskNum << "\n";
	cout << "׼����ʼ��Ⱦ" << endl;
	system("pause");

	out << "��ʼ��Ⱦ..." << "\n"; cout << "��ʼ��Ⱦ..." << "\n";
	auto startTime = clock(); //  ��ʼʱ��

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
			printf("����: %3.0f%%\n", (curRate + curEndTask) / (float)(taskNum) * 100);
			sTime = 0;
		}

		if (endNum == threadNum && curTask == taskNum)
		{
			system("cls");
			printf("��Ⱦ���.%3.0f%%\n\n", (curTask) / (float)(taskNum) * 100);
			out << "��Ⱦ���.\n";
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
	cout << "׼����ʼ��Ⱦ" << endl;
	system("pause");
	out << "��ʼ��Ⱦ..." << "\n"; cout << "��ʼ��Ⱦ..." << "\n";
	cout << "��ʼ��Ⱦ..." << "\n"; cout << "��ʼ��Ⱦ..." << "\n";
	auto startTime = clock(); //  ��ʼʱ��

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
					printf("����: %3.0f%%\n", (*curRate) * 100);
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

		out << "��" << threadIndex << "��������Ⱦ����,��ʱ: "
			<< (float)(clock() - startTime) / 1000.0f
			<< "s.\n--------------------------\n";
		cout << "��" << threadIndex << "��������Ⱦ����,��ʱ: "
			<< (float)(clock() - startTime) / 1000.0f
			<< "s.\n--------------------------\n";
		stbi_flip_vertically_on_write(true);
		stbi_write_bmp("outImage.bmp", nx, ny, nChannel, imageData);
		cout << "���ͼ�����" << endl;
		system("outImage.bmp");
		free(imageData);

		delete world;

		return 0;
}

int main()
{
	Srand48((unsigned int)time(NULL));
	ifstream testReader("renderLog.txt");
	string oldLog((std::istreambuf_iterator<char>(testReader)),
		std::istreambuf_iterator<char>());
	testReader.close();

	ofstream testOut("renderLog.txt");

	testOut << oldLog;

	time_t now = time(0);
	char dt[30];
	ctime_s(dt, sizeof dt, &now);
	testOut << dt << "\n";
	
	testOut << "��Ⱦ��־: \n\n";
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
	//printf("�������:%d\n", task);
}