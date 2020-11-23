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

// release下开启多线程 与 采样优化(可选)
#if !_DEBUG
#define MULTI_THREAD 1
//采样优化，当追踪深度为1时，不进行蒙特卡洛积分采样
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
	out << "线程指数: " << threadIndex << "\n";

	int nx = 512; //  宽
	int ny = 288; //  高
	int nChannel = 3; //  颜色通道数量
	int ns = 150; //  抗锯齿(蒙特卡洛采样)
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
	out << "是否开启采样优化: 是\n";
	cout << "是否开启采样优化: 是\n";
#else
	out << "是否开启采样优化: 否\n";
	cout << "是否开启采样优化: 否\n";
#endif // REDUCE_INEGRATE

	out << "第" << threadIndex << "次渲染准备\n";
	out << "渲染质量: " << nx << "*" << ny << ", 蒙特卡洛采样次数: "
		<< ns << ", 探测深度: " << maxTraceDepth << ".\n";
	cout << "渲染质量: " << nx << "*" << ny << ", 蒙特卡洛采样次数: "
		<< ns << ", 探测深度: " << maxTraceDepth << ".\n";
	out << "场景体数量:" << (*world).size << ", 光圈: " << camera->aperture << ".\n";
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
	time_t startTime;

#ifndef MULTI_THREAD
#else
#endif // !MULTI_THREAD

	if (MULTI_THREAD)
	{
		//多线程模式
		SYSTEM_INFO systemInfo;
		GetSystemInfo(&systemInfo);
		int coreNum = (int)(systemInfo.dwNumberOfProcessors);
		int threadNum = coreNum - 2;
		int taskNum = threadNum * min(max(1, nx / 256), 20);
		//int taskNum = threadNum * threadIndex;
		out << "渲染模式: " << "多线程; ";
		out << "任务数: " << taskNum << "\n"; cout << "任务数: " << taskNum << "\n";
		cout << "准备开始渲染" << endl;
		system("pause");

		out << "开始渲染..." << "\n"; cout << "开始渲染..." << "\n";
		startTime = clock(); //  开始时间

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
				printf("\r进度: %3.0f%%          ", (curRate + curEndTask) / (float)(taskNum) * 100);
				sTime = 0;
			}

			if (endNum == threadNum && curTask == taskNum)
			{
				//system("cls");
				printf("\r渲染完成.%3.0f%%    \n\n", (curTask) / (float)(taskNum) * 100);
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
	}
	else
	{
		//单线程模式
		float curRate = 0.0f;
		cout << "准备开始渲染" << endl;
		system("pause");
		out << "开始渲染..." << "\n"; cout << "开始渲染..." << "\n";
		cout << "开始渲染..." << "\n"; cout << "开始渲染..." << "\n";

		startTime = clock(); //  开始时间

		//进度跟踪线程
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
						printf("\r进度: %3.0f%%        ", (*curRate) * 100);
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
					//先试着采样ns/10次
					for (int k = 0; k < tryNs; k++)
					{
						deep = 1;
						u = float(i + RayMath::Drand48()) / float(nx);
						v = float(j + RayMath::Drand48()) / float(ny);
						r = camera->GetRay(u, v);
						color.rgb += RayTracer(r, &scence, maxTraceDepth, deep);
						if (deep > 1)intersectionTimes++;
					}
					//如果大概率深度大于1
					if (intersectionTimes > tryNs * 0.8)
					{
						//继续采样
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

	testOut << "渲染日志: \n\n";
	
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
	//一些重要的初始化设定
	glfwInit(); // 初始化glfw
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // 
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//创建窗口
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

	glViewport(0, 0, 800, 600); // 设置视口大小

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // 窗口大小调整回调

	while (!glfwWindowShouldClose(window))
	{
		//输入
		processInput(window);

		//渲染
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		//事件处理，交换缓冲
		glfwSwapBuffers(window);//双缓冲交换
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
			//先试着采样ns/10次
			for (int k = 0; k < tryNs; k++)
			{
				deep = 1;
				u = float(i + RayMath::Drand48()) / float(param->nx);
				v = float(j + RayMath::Drand48()) / float(param->ny);
				r = param->camera->GetRay(u, v);
				color.rgb += RayTracer(r, param->scence, param->maxTraceDepth, deep);
				if (deep > 1)intersectionTimes++;
			}
			//如果大概率深度大于1
			if (intersectionTimes > tryNs * 0.8)
			{
				//继续采样
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
	//printf("完成任务:%d\n", task);
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
