#include<iostream>
#include<fstream>
#include<ctime>
#include<random>
#include<thread>
#include<stb/stb_image_write.h>
#include<algorithm>

#include "RayTracer.h"
#include "Scence.h"

#ifndef __linux__
#include<Windows.h>
#endif // !__linux__

// release�¿������߳� �� �����Ż�(��ѡ)
#if !_DEBUG
#define MULTI_THREAD 1
//�����Ż�����׷�����Ϊ1ʱ�����������ؿ�����ֲ���
//#define REDUCE_INEGRATE
#else
#define MULTI_THREAD 0
#endif // _DEBUG

#include<glad/glad.h>
#include<GLFW/glfw3.h>

using namespace std;

struct RayTraceParam
{
	RayTraceParam(int Start, int End, unsigned char* Data, int Nx, int Ny, int NChannel, int Ns,
		Camera* camerA, Scence* scencE, int MaxTraceDepth, float* EndNumber)
		: start(Start), end(End), imageData(Data), nx(Nx), ny(Ny), nChannel(NChannel), ns(Ns),
		camera(camerA), scence(scencE),
		maxTraceDepth(MaxTraceDepth), endNumber(EndNumber), isMainExit(nullptr)
	{}
	RayTraceParam(int Start, int End, unsigned char* Data, int Nx, int Ny, int NChannel, int Ns,
		Camera* camerA, Scence* scencE, int MaxTraceDepth, float* EndNumber, bool* IsMainExit)
		: start(Start), end(End), imageData(Data), nx(Nx), ny(Ny), nChannel(NChannel), ns(Ns),
		camera(camerA), scence(scencE),
		maxTraceDepth(MaxTraceDepth), endNumber(EndNumber), isMainExit(IsMainExit)
	{}
	int start, end;
	unsigned char* imageData;
	int nx, ny, nChannel, ns;
	Camera* camera;
	Scence* scence;
	int maxTraceDepth;
	float* endNumber;
	bool* isMainExit;
};

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

int DrawWindow();

void RayTraceThread(RayTraceParam* param);

int run(int threadIndex, ofstream& out)
{
	out << "�߳�ָ��: " << threadIndex << "\n";

	int nx = 512; //  ��
	int ny = 288; //  ��
	int nChannel = 3; //  ��ɫͨ������
	int ns = 150; //  �����(���ؿ������)
	int maxTraceDepth = 10;
	unsigned char* imageData = (unsigned char*)malloc(sizeof(unsigned char) * nx * ny * nChannel);

	HitList* world = NULL;
	Scence scence(nx, ny, nChannel, nx, maxTraceDepth);
	
	//scence.LoadSomeBalls();
	scence.LoadIntersectionBall();
	//scence.LoadUnionBall();
	//scence.LoadDifferenceBall();
	//scence.LoadRandomBall();
	//scence.LoadCornellBox();

	world = scence.GetWorld();
	Camera* camera = scence.GetCamera();

	if (camera == nullptr || world == nullptr)
		return -1;

	Ray r;
	Color color;
#ifdef REDUCE_INEGRATE
	out << "�Ƿ��������Ż�: ��\n";
	cout << "�Ƿ��������Ż�: ��\n";
#else
	out << "�Ƿ��������Ż�: ��\n";
	cout << "�Ƿ��������Ż�: ��\n";
#endif // REDUCE_INEGRATE

	out << "��" << threadIndex << "����Ⱦ׼��\n";
	out << "��Ⱦ����: " << nx << "*" << ny << ", ���ؿ����������: "
		<< ns << ", ̽�����: " << maxTraceDepth << ".\n";
	cout << "��Ⱦ����: " << nx << "*" << ny << ", ���ؿ����������: "
		<< ns << ", ̽�����: " << maxTraceDepth << ".\n";
	out << "����������:" << (*world).size << ", ��Ȧ: " << camera->aperture << ".\n";
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
	time_t startTime;

#ifndef MULTI_THREAD
#else
#endif // !MULTI_THREAD

	if (MULTI_THREAD)
	{
		//���߳�ģʽ
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
		startTime = clock(); //  ��ʼʱ��

		thread** rtThread = (thread**)malloc(sizeof(thread*) * threadNum);

		float* endFlag = (float*)malloc(sizeof(float) * threadNum);

		int curTask = 0;

		for (; curTask < threadNum; curTask++)
		{
			endFlag[curTask] = 0.0f;
			
			rtThread[curTask] = new thread(RayTraceThread, new RayTraceParam(
				(int)(curTask * ny / taskNum), (int)((curTask + 1) * ny / taskNum), imageData,
				nx, ny, nChannel, ns, camera, &scence, maxTraceDepth, &endFlag[curTask])
			);
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
						rtThread[i] = new thread(RayTraceThread, new RayTraceParam(
							(int)(curTask * ny / taskNum), (int)((curTask + 1) * ny / taskNum), imageData,
							nx, ny, nChannel, ns, camera, &scence, maxTraceDepth, &endFlag[i])
						);
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
				//system("cls");
				printf("\r����: %3.0f%%          ", (curRate + curEndTask) / (float)(taskNum) * 100);
				sTime = 0;
			}

			if (endNum == threadNum && curTask == taskNum)
			{
				//system("cls");
				printf("\r��Ⱦ���.%3.0f%%    \n\n", (curTask) / (float)(taskNum) * 100);
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
	}
	else
	{
		//���߳�ģʽ
		float curRate = 0.0f;
		cout << "׼����ʼ��Ⱦ" << endl;
		system("pause");
		out << "��ʼ��Ⱦ..." << "\n"; cout << "��ʼ��Ⱦ..." << "\n";
		cout << "��ʼ��Ⱦ..." << "\n"; cout << "��ʼ��Ⱦ..." << "\n";

		startTime = clock(); //  ��ʼʱ��

		//���ȸ����߳�
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
						printf("\r����: %3.0f%%        ", (*curRate) * 100);
						sTime = 0;
					}
				}
			}, &curRate).detach();
			int deep = 1;
			int intersectionTimes = 0;
			int tryNs = min(ns, max(20, ns / 10));
			float u, v;
			for (int j = 0; j < ny; j++)
			{
				for (int i = 0; i < nx; i++)
				{
					color.rgb = Vector3::Zero;
#ifdef REDUCE_INEGRATE
					intersectionTimes = 0;
					//�����Ų���ns/10��
					for (int k = 0; k < tryNs; k++)
					{
						deep = 1;
						u = float(i + RayMath::Drand48()) / float(nx);
						v = float(j + RayMath::Drand48()) / float(ny);
						r = camera->GetRay(u, v);
						color.rgb += RayTracer(r, &scence, maxTraceDepth, deep);
						if (deep > 1)intersectionTimes++;
					}
					//����������ȴ���1
					if (intersectionTimes > tryNs * 0.8)
					{
						//��������
						for (int k = 0; k < ns - tryNs; k++)
						{
							u = float(i + RayMath::Drand48()) / float(nx);
							v = float(j + RayMath::Drand48()) / float(ny);
							r = camera->GetRay(u, v);
							color.rgb += RayTracer(r, &scence, maxTraceDepth);
						}
						color.rgb /= float(ns);
					}
					else
					{
						color.rgb /= float(tryNs);
					}
#else
					float u, v;
					for (int k = 0; k < ns; k++)
					{
						u = float(i + RayMath::Drand48()) / float(nx);
						v = float(j + RayMath::Drand48()) / float(ny);
						r = camera->GetRay(u, v);
						color.rgb += RayTracer(r, &scence, maxTraceDepth);
					}
					color.rgb /= float(ns);
#endif // REDUCE_INEGRATE
					color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));
					for (int ch = 0; ch < nChannel; ch++)
					{
						imageData[j * nx * nChannel + i * nChannel + ch] = (unsigned char)(TO_RGB * color[ch]);
					}
				}
				curRate = j / (float)ny;
			}

			curRate = 2.0f;
	}

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

	return 0;
}

int main()
{
	RayMath::Srand48((unsigned int)time(NULL));
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
	
	//new thread(DrawWindow);

	for (int i = 1; i <= 1; i++)
	{
		if (run(i, testOut) == 1)
			return 1;
	}

	testOut.close();
	system("pause");

	return 0;


}

int DrawWindow()
{/************************************************************************/
	//һЩ��Ҫ�ĳ�ʼ���趨
	glfwInit(); // ��ʼ��glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//��������
	GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, 800, 600); // �����ӿڴ�С

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // ���ڴ�С�����ص�

	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window);

		//��Ⱦ
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//�¼�������������
		glfwSwapBuffers(window);//˫���彻��
		glfwPollEvents();
	}
	glfwTerminate();
}

void RayTraceThread(RayTraceParam* param)
{
	Color color;
	Ray r;
	int deep = 1;
	int intersectionTimes = 0;
	int tryNs = min(param->ns, max(20, param->ns / 10));
	float u, v;
	for (int j = param->start; j < param->end; j++)
	{
		for (int i = 0; i < param->nx; i++)
		{
			color.rgb = Vector3::Zero;
#ifdef REDUCE_INEGRATE
			intersectionTimes = 0;
			//�����Ų���ns/10��
			for (int k = 0; k < tryNs; k++)
			{
				deep = 1;
				u = float(i + RayMath::Drand48()) / float(param->nx);
				v = float(j + RayMath::Drand48()) / float(param->ny);
				r = param->camera->GetRay(u, v);
				color.rgb += RayTracer(r, param->scence, param->maxTraceDepth, deep);
				if (deep > 1)intersectionTimes++;
			}
			//����������ȴ���1
			if (intersectionTimes > tryNs * 0.8)
			{
				//��������
				for (int k = 0; k < param->ns - tryNs; k++)
				{
					u = float(i + RayMath::Drand48()) / float(param->nx);
					v = float(j + RayMath::Drand48()) / float(param->ny);
					r = param->camera->GetRay(u, v);
					color.rgb += RayTracer(r, param->scence, param->maxTraceDepth);
				}
				color.rgb /= float(param->ns);
			}
			else
			{
				color.rgb /= float(tryNs);
			}
#else
			float u, v;
			for (int k = 0; k < param->ns; k++)
			{
				u = float(i + RayMath::Drand48()) / float(param->nx);
				v = float(j + RayMath::Drand48()) / float(param->ny);
				r = param->camera->GetRay(u, v);
				color.rgb += RayTracer(r, param->scence, param->maxTraceDepth);
			}
			color.rgb /= float(param->ns);
#endif // REDUCE_INEGRATE
			
			color[0] = min(color[0], 1.0f);
			color[1] = min(color[1], 1.0f);
			color[2] = min(color[2], 1.0f);
			color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));
			
			for (int ch = 0; ch < param->nChannel; ch++)
			{
				param->imageData[j * param->nx * param->nChannel + i * param->nChannel + ch] = (unsigned char)(TO_RGB * color[ch]);
			}
		}
		(*param->endNumber) = (float)(j - param->start) / (float)(param->end - param->start);
	}
	(*param->endNumber) = 1.1f;
	//printf("�������:%d\n", task);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}
