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

	HitList* GetWorld()
	{
		return world;
	}

	Camera* GetCamera()
	{
		return camera;
	}

	void LoadTriBall();

	void LoadIntersectionBall();

	void LoadRandomBall();

	void LoadCheckingTexture();

	void LoadPerlinNoise();

	void LoadCornellBox();

private:

	HitList* randomScence(int maxSize, int randomIndex);

	int nx;
	int ny;
	int nChannel;
	int ns;
	int maxTraceDepth;

	HitList* world;

	Camera* camera;

};

#endif // !SCENCE_H
