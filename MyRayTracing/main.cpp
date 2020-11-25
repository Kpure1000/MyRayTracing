#include "RayTracer.h"
#include "Scence.h"
#include"rtx/render/RenderWindow.h"

#include<iostream>
#include<fstream>
#include<ctime>
#include<random>
#include<thread>
#include<algorithm>
#include<Windows.h>
#include<stb/stb_image_write.h>

using namespace std;
using namespace rtx;

/*--------------------------------------------*/

// multi-thread on under release
#if !_DEBUG
#define MULTI_THREAD 1
//Sample Optimization (When tracing depth = 1, do NOT sample
//#define SAMPLE_OPTIMIZATION
#else
#define MULTI_THREAD 0
#endif // _DEBUG

#ifndef CannotResize
#define CannotResize  (sf::Style::Titlebar |  sf::Style::Close)
#endif // !CannotResize
#ifndef KeyPressing
#define KeyPressing sf::Event::KeyPressed
#endif // !KeyPressing
#ifndef KeyReleasing
#define KeyReleasing sf::Event::KeyReleased
#endif // !KeyReleasing
#ifndef KeyEvent
#define KeyEvent(EV) (sf::Keyboard::isKeyPressed(EV))
#endif // !KeyEvent

//  the param of ray tracing thread call back
struct RayTraceParam
{
	RayTraceParam(int Start, int End, Scence* scencE, float* EndNumber)
		: start(Start), end(End), scence(scencE),
		endNumber(EndNumber), isMainExit(nullptr)
	{}
	RayTraceParam(int Start, int End, int Nx, int Ny, int NChannel, int Ns,
		Camera* camerA, Scence* scencE, int MaxTraceDepth, float* EndNumber, bool* IsMainExit)
		: start(Start), end(End), scence(scencE),
		endNumber(EndNumber), isMainExit(IsMainExit)
	{}
	int start, end;
	Scence* scence;
	float* endNumber;
	bool* isMainExit;
};

//void framebuffer_size_callback(GLFWwindow* window, int width, int height);
//void processInput(GLFWwindow* window);

int DrawWindow(int w, int h, int ch, unsigned char* data);

void RayTraceThread(RayTraceParam* param);

int run(ofstream& out, Scence& scence, const std::string& imageFileName)
{
	Ray r;
	Color color;
#ifdef SAMPLE_OPTIMIZATION
	out << "Is the sampling optimization allowed? : Y\n";
	cout << "Is the sampling optimization allowed? : Y\n";
#else
	out << "Is the sampling optimization allowed? : N\n";
	cout << "Is the sampling optimization allowed? : N\n";
#endif // REDUCE_INEGRATE

	out << "Render quality: " << scence.width << "*" << scence.height << ", sample numbers: "
		<< scence.sample << ", tracing depth: " << scence.maxTraceDepth << ".\n";
	cout << "Render quality: " << scence.width << "*" << scence.height << ", sample numbers: "
		<< scence.sample << ", tracing depth: " << scence.maxTraceDepth << ".\n";
	out << "Scence objects number(s):" << scence.GetWorld()->size << ", aperture: " << scence.GetCamera()->aperture << ".\n";
	if (scence.channel > 4)
	{
		out << "Channels of color are more than 4, exit.\n";
		cout << "Channels of color are more than 4, exit.\n";
		return EXIT_FAILURE;
	}
	time_t startTime;
	time_t endTime = 0;

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
		int taskNum = threadNum * min(max(1, scence.width / 256), 20);

		out << "Render mode: " << "Multi-thread";
		out << "Task number(s): " << taskNum << "\n"; cout << "Task number(s): " << taskNum << "\n";
		cout << "Render will start in 2s...\n\n" << endl;

		std::this_thread::sleep_for(std::chrono::seconds(2));

		out << "Start rendering..." << "\n"; cout << "Start rendering..." << "\n";
		startTime = clock(); //  rendering start time
		thread** rtThread = (thread**)malloc(sizeof(thread*) * threadNum);

		float* endFlag = (float*)malloc(sizeof(float) * threadNum);

		int curTask = 0;

		for (; curTask < threadNum; curTask++)
		{
			endFlag[curTask] = 0.0f;

			rtThread[curTask] = new thread(RayTraceThread, new RayTraceParam(
				(int)(curTask * scence.height / taskNum), (int)((curTask + 1) * scence.height / taskNum),
				&scence, &endFlag[curTask])
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
							(int)(curTask * scence.height / taskNum), (int)((curTask + 1) * scence.height / taskNum),
							&scence, &endFlag[i])
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
				float curLoading = (curRate + curEndTask) / (float)(taskNum);
				time_t renderTime = clock() - startTime;
				//if (curLoading < 1e-3f)curLoading = 1e-3f;
				printf("\rLoading: %3.2f/100.00 %%, took time of %ld/%ld s           ",
					curLoading * 100, renderTime / 1000, (long)(renderTime / max(0.015f, curLoading) / 1000));
				sTime = 0;
			}

			if (endNum == threadNum && curTask == taskNum)
			{
				//system("cls");
				printf("\rrendering compeleted. %3.0f%%                                 \n\n", (curTask) / (float)(taskNum) * 100);
				out << "rendering compeleted.\n";
				break;
			}
		}
		endTime = clock();
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
						printf("\r进度: %3.0f%%          ", (*curRate) * 100);
						sTime = 0;
					}
				}
			}, &curRate).detach();
			int deep = 1;
			int intersectionTimes = 0;
			int tryNs = min(scence.sample, max(20, scence.sample / 10));
			float u, v;
			for (int j = 0; j < scence.height; j++)
			{
				for (int i = 0; i < scence.width; i++)
				{
					color.rgb = Vector3::Zero;
#ifdef SAMPLE_OPTIMIZATION
					intersectionTimes = 0;
					//先试着采样scence.sample/10次
					for (int k = 0; k < tryNs; k++)
					{
						deep = 1;
						u = float(i + RayMath::Drand48()) / float(scence.width);
						v = float(j + RayMath::Drand48()) / float(scence.height);
						r = scence.GetCamera()->GetRay(u, v);
						color.rgb += RayTracer(r, &scence, scence.maxTraceDepth, deep);
						if (deep > 1)intersectionTimes++;
					}
					//如果大概率深度大于1
					if (intersectionTimes > tryNs * 0.8)
					{
						//继续采样
						for (int k = 0; k < scence.sample - tryNs; k++)
						{
							u = float(i + RayMath::Drand48()) / float(scence.width);
							v = float(j + RayMath::Drand48()) / float(scence.height);
							r = scence.GetCamera()->GetRay(u, v);
							color.rgb += RayTracer(r, &scence, scence.maxTraceDepth);
						}
						color.rgb /= float(scence.sample);
					}
					else
					{
						color.rgb /= float(tryNs);
					}
#else
					float u, v;
					for (int k = 0; k < scence.sample; k++)
					{
						u = float(i + RayMath::Drand48()) / float(scence.width);
						v = float(j + RayMath::Drand48()) / float(scence.height);
						r = scence.GetCamera()->GetRay(u, v);
						color.rgb += RayTracer(r, &scence, scence.maxTraceDepth);
					}
					color.rgb /= float(scence.sample);
#endif // REDUCE_INEGRATE
					color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));
					for (int ch = 0; ch < scence.channel; ch++)
					{
						scence.GetImageBuffer()[j * scence.width * scence.channel + i * scence.channel + ch] = (unsigned char)(TO_RGB * color[ch]);
					}
				}
				curRate = j / (float)scence.height;
			}

			curRate = 2.0f;
	}

	out << "CPU rendering compeleted, with time of "
		<< (float)(endTime - startTime) / 1000.0f
		<< "s.\n--------------------------\n";
	cout << "CPU rendering compeleted, with time of "
		<< (float)(endTime - startTime) / 1000.0f
		<< "s.\n--------------------------\n";
	stbi_flip_vertically_on_write(true);
	stbi_write_jpg(imageFileName.c_str(), scence.width, scence.height, scence.channel, scence.GetImageBuffer(), 0);
	cout << "Image Output Compeleted" << endl;

	system(imageFileName.c_str());

	std::this_thread::sleep_for(std::chrono::seconds(1));

	return 0;
}

int main()
{

	RayMath::Srand48((unsigned int)time(NULL));

#pragma region Load Log

	ifstream testReader("cpuRenderLog.txt");
	string oldLog((std::istreambuf_iterator<char>(testReader)),
		std::istreambuf_iterator<char>());
	testReader.close();

	ofstream testOut("cpuRenderLog.txt");

	testOut << oldLog;

	time_t now = time(0);
	char dt[30];
	ctime_s(dt, sizeof dt, &now);
	testOut << dt << "\n";

	testOut << "Rendering Log: \n\n";

#pragma endregion

	int nx = 512; //  width
	int ny = 288; //  height
	int nChannel = 3; //  channel of color (rgb or rgba)
	int ns = 100; //  sample times 
	int maxTraceDepth = 5; //  max ray tracing depth

	Scence scence(nx, ny, nChannel, ns, maxTraceDepth);

	//scence.LoadSomeBalls();
	//scence.LoadIntersectionBall();
	//scence.LoadUnionBall();
	//scence.LoadDifferenceBall();
	scence.LoadRandomBall();
	//scence.LoadCornellBox();

	if (scence.GetCamera() == nullptr || scence.GetWorld() == nullptr)
	{
		std::cerr << "Scence Load Failed.\n";
		return EXIT_FAILURE;
	}

	new thread(DrawWindow, scence.width, scence.height, scence.channel, scence.GetImageBuffer());
	std::this_thread::sleep_for(std::chrono::seconds(2));

	run(testOut, scence, "outImage.bmp");

	testOut.close();
	system("pause");

	return EXIT_SUCCESS;
}

/// <summary>
/// render the image to window in real-time
/// </summary>
/// <returns></returns>
int DrawWindow(int w, int h, int ch, unsigned char* data)
{
	render::RenderWindow window(w, h, "Ray-Tracing CPU-Renderer @Kpure1000");
	window.renderTarget.LoadFromMemory(w, h, ch, data);

	window.Run();

	return 0;
}

/// <summary>
/// thread of ray tracing
/// </summary>
/// <param name="param">the scence and some other data to thread</param>
void RayTraceThread(RayTraceParam* param)
{
	Color color;
	Ray r;
	int deep = 1;
	int intersectionTimes = 0;
	int tryNs = min(param->scence->sample, max(20, param->scence->sample / 10));
	float u, v;
	for (int j = param->start; j < param->end; j++)
	{
		for (int i = 0; i < param->scence->width; i++)
		{
			color.rgb = Vector3::Zero;
#ifdef SAMPLE_OPTIMIZATION
			intersectionTimes = 0;
			//先试着采样ns/10次
			for (int k = 0; k < tryNs; k++)
			{
				deep = 1;
				u = float(i + RayMath::Drand48()) / float(param->scence->width);
				v = float(j + RayMath::Drand48()) / float(param->scence->height);
				r = param->scence->GetCamera()->GetRay(u, v);
				color.rgb += RayTracer(r, param->scence, param->scence->maxTraceDepth, deep);
				if (deep > 1)intersectionTimes++;
			}
			//如果大概率深度大于1
			if (intersectionTimes > tryNs * 0.8)
			{
				//继续采样
				for (int k = 0; k < param->scence->sample - tryNs; k++)
				{
					u = float(i + RayMath::Drand48()) / float(param->scence->width);
					v = float(j + RayMath::Drand48()) / float(param->scence->height);
					r = param->scence->GetCamera()->GetRay(u, v);
					color.rgb += RayTracer(r, param->scence, param->scence->maxTraceDepth);
				}
				color.rgb /= float(param->scence->sample);
			}
			else
			{
				color.rgb /= float(tryNs);
			}
#else
			float u, v;
			for (int k = 0; k < param->scence->sample; k++)
			{
				u = float(i + RayMath::Drand48()) / float(param->scence->width);
				v = float(j + RayMath::Drand48()) / float(param->scence->height);
				r = param->scence->GetCamera()->GetRay(u, v);
				color.rgb += RayTracer(r, param->scence, param->scence->maxTraceDepth);
			}
			color.rgb /= float(param->scence->sample);
#endif // REDUCE_INEGRATE

			color[0] = min(color[0], 1.0f);
			color[1] = min(color[1], 1.0f);
			color[2] = min(color[2], 1.0f);
			color.rgb = Vector3(sqrtf(color.r()), sqrtf(color.g()), sqrtf(color.b()));

			for (int ch = 0; ch < param->scence->channel; ch++)
			{
				param->scence->GetImageBuffer()
					[j * param->scence->width * param->scence->channel +
					i * param->scence->channel + ch] = (unsigned char)(TO_RGB * color[ch]);
			}
		}
		(*param->endNumber) = (float)(j - param->start) / (float)(param->end - param->start);
	}
	(*param->endNumber) = 1.1f;
}
