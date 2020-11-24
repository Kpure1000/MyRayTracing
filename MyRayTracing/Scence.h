#ifndef SCENCE_H
#define SCENCE_H

#include"RayTracer.h"

using namespace ry;

class Scence
{
public:

	Scence();

	Scence(int Width, int Height, int nChannel, int SampleIndex, int MaxTracingDepth);

	~Scence();

	unsigned char* GetImageBuffer()
	{
		return imageBuffer;
	}

	HitList* GetWorld()
	{
		return world;
	}

	BVH* GetBVH()
	{
		return bvh;
	}

	Camera* GetCamera()
	{
		return camera;
	}

	Skybox* GetSkybox()
	{
		return skybox;
	}

	void LoadSomeBalls();

	void LoadIntersectionBall();

	void LoadUnionBall();

	void LoadDifferenceBall();

	void LoadRandomBall();

	void LoadCheckingTexture();

	void LoadPerlinNoise();

	void LoadCornellBox();

private:

	HitList* randomScence(int maxSize, int randomIndex);

public:

	int width;
	int height;
	int channel;
	int sample;
	int maxTraceDepth;

private:

	//  buffer of image
	unsigned char* imageBuffer;

	HitList* world;

	BVH* bvh;

	Camera* camera;
	
	Skybox* skybox;

};

#endif // !SCENCE_H
