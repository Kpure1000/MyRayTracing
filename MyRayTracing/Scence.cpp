#include "Scence.h"

Scence::Scence()
	: world(nullptr), camera(nullptr), skybox(nullptr)
{
}

Scence::Scence(int Width, int Height, int nChannel, int SampleIndex, int MaxTracingDepth)
	: nx(Width), ny(Height), nChannel(nChannel), ns(SampleIndex), maxTraceDepth(MaxTracingDepth),
	world(nullptr), camera(nullptr), skybox(nullptr)
{
}

Scence::~Scence()
{
	if (world)delete world;
	if (camera)delete camera;
	if (skybox)delete skybox;
}

void Scence::LoadSomeBalls()
{
	Vector3 lookFrom(0.5f, 2.2f, 4.0f);
	Vector3 lookAt(0.0f, 1.5f, 0.0f);
	float dist_to_focus = (lookAt - lookFrom).Magnitude();
	float aperture = 0.0f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 60, float(nx) / float(ny), aperture, dist_to_focus);

	world = randomScence(10, 0);
	world->AddHitable(new Sphere(new SdfSphere({ 0,-1000.5f,0.0f }, 1000.8f), new Metal(
		new Constant_Texture({ 0.6f,0.6f,0.6f }) ,0.1f
	)));

	world->AddHitable(new Sphere(new SdfSphere({ 2.02f,2.2f,-1.3f }, 1.0f), new Lambertian(
		new Constant_Texture({ 0.7f,0.2f,0.6f })
	)));

	world->AddHitable(new Sphere(new SdfSphere({ 0,2.2f,-1.3f }, 1.0f), new Metal(
		new Constant_Texture({ 0.6f,0.8f,0.2f }),0.1f
	)));

	world->AddHitable(new Sphere(new SdfSphere({ -2.02f,2.2f,-1.3f }, 1.0f), new Dielectric(
		new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.4f
	)));

	skybox = new Skybox(new Illumination(
		new Customize_Texture(
		[](const float& u, const float& v, const Vector3& p)->Vector3
		{
			float t = 0.5f * (p[1] + 1.0f);
			return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.3f, 0.5f, 0.9f);
		})
		, 0.8f));
}

void Scence::LoadIntersectionBall()
{
	Vector3 lookFrom(20.0f, 1.5f, -20.0f);
	Vector3 lookAt(0.0f, 1.5f, -13.0f);
	float dist_to_focus = (lookAt - lookFrom).Magnitude();
	float aperture = 0.0f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 80, float(nx) / float(ny), aperture, dist_to_focus);

	world = randomScence(20, 0);

	// Í¸¾µ: 
	float offset = 4.0f;
	world->AddHitable(new IntersectionHit(
		new Sphere(new SdfSphere({ 0.0f, 1.5f,-2.0f + offset },15.0f), new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.617f)),
		new Sphere(new SdfSphere({ 0.0f, 1.5f, -30.0f + offset }, 15.0f), new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.617f)),
		new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.5f)
	));

	world->AddHitable(new Sphere(new SdfSphere({ 2.4f,-4.9f,-33.5 }, 2.1f), new Lambertian(new Constant_Texture({ 1.0,1.0,1.0 }))));
	world->AddHitable(new Sphere(new SdfSphere({ -2.4f,-4.4f,-35.0 }, 2.6f), new Metal(new Constant_Texture({ 0.4,0.4,0.4 }), 0.0f)));

	world->AddHitable(new Rect(new SdfRect_xy(-6, -7, 6, 8.5, -37), new Illumination(new Constant_Texture({ 1,1,1 }),33.0)));
	world->AddHitable(new Rect(new SdfRect_xz(-6, -37, 6, -29, 8.5), new Illumination(new Constant_Texture({ 0.05,0.8,1.0 }),5.0f)));
	world->AddHitable(new Rect(new SdfRect_xz(-6, -37, 6, -29, -7), new Lambertian(new Constant_Texture({ 0.4,0.4,0.1 }))));
	world->AddHitable(new Rect(new SdfRect_yz(-7, -37, 8.5, -29, 6), new Lambertian(new Constant_Texture({ 0,0.6,0 }))));
	world->AddHitable(new Rect(new SdfRect_yz(-7, -37, 8.5, -29, -6), new Lambertian(new Constant_Texture({ 0.6,0,0 }))));

	world->AddHitable(new Rect(new SdfRect_xy(-37, -35.5f, 37, 38.5f, 5.1f), new Lambertian(new Constant_Texture({ 0.1,0.1,0.1 }))));

	skybox = new Skybox(new Illumination(
		new Customize_Texture(
		[](const float& u, const float& v, const Vector3& p)->Vector3
		{
			return { 0,0,0 };
			float t = 0.5f * (p[1] + 1.0f);
			return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.3f, 0.5f, 0.9f);
		}
	)
		//new Image_Texture("red_hill_curve.jpg")
		, 0.8f));
}

void Scence::LoadUnionBall()
{
	Vector3 lookFrom(10.0f, -0.5f, -8.0f);
	Vector3 lookAt(0.0f, 0.5f, -8.0f);
	float dist_to_focus = (lookAt - lookFrom).Magnitude();
	float aperture = 0.0f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 80, float(nx) / float(ny), aperture, dist_to_focus);

	world = randomScence(20, 0);

	/*world->AddHitable(new UnionHit(
		new Sphere(new SdfSphere({ 0.0f,2.5f,-2.0f }, 3.0f), new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.1f)),
		new Sphere(new SdfSphere({ 0.0f, 2.5f, -7.0f }, 3.0f), new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }), 0.0f)),
		new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.5f)
	));*/

	world->AddHitable(new UnionHit(
		new Sphere(new SdfSphere({ 0.0f,2.5f,-2.0f }, 3.0f), new Transparent()),
		new Sphere(new SdfSphere({ 0.0f, 2.5f, -7.0f }, 3.0f), new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }), 0.0f)),
		new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.5f)
	));

	world->AddHitable(new Sphere(new SdfSphere({ -0.0f,-2.5f,-2.0f }, 3.0f),
		new Transparent()));
	world->AddHitable(new Sphere(new SdfSphere({ -0.0f, -2.5f, -7.0f }, 3.0f),
		new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }), 0.0f)));

	skybox = new Skybox(new Illumination(
		new Image_Texture("skybox.jpg")
		, 0.8f));
}

void Scence::LoadDifferenceBall()
{
	Vector3 lookFrom(10.0f, -0.5f, -14.0f);
	Vector3 lookAt(0.0f, 2.5f, -8.0f);
	float dist_to_focus = (lookAt - lookFrom).Magnitude();
	float aperture = 0.0f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 45, float(nx) / float(ny), aperture, dist_to_focus);

	world = randomScence(20, 0);

	/*world->AddHitable(new UnionHit(
		new Sphere(new SdfSphere({ 0.0f,2.5f,-2.0f }, 3.0f), new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.1f)),
		new Sphere(new SdfSphere({ 0.0f, 2.5f, -7.0f }, 3.0f), new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }), 0.0f)),
		new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.5f)
	));*/

	world->AddHitable(new DifferenceHit(
		new Sphere(new SdfSphere({ 0.0f,2.5f,-2.0f }, 3.0f), new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }),0.0f)),
		new Sphere(new SdfSphere({ 0.0f, 2.5f, -7.0f }, 3.0f), new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }),0.0f)),
		new Dielectric(new Constant_Texture({ 1.0f,1.0f,1.0f }), 1.5f)
	));

	/*world->AddHitable(new Sphere(new SdfSphere({ -0.0f,-2.5f,-2.0f }, 3.0f),
		new Transparent()));
	world->AddHitable(new Sphere(new SdfSphere({ -0.0f, -2.5f, -7.0f }, 3.0f),
		new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }), 0.0f)));*/

	skybox = new Skybox(new Illumination(
		new Image_Texture("red_hill_curve.jpg")
		, 1.0f));
}

void Scence::LoadRandomBall()
{
	Vector3 lookFrom(-2.0f, 1.5f, 4.5f);
	Vector3 lookAt(-0.5f, 0.7f, -1.0f);
	float dist_to_focus = (lookAt - lookFrom).Magnitude();
	float aperture = 0.01f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 70, float(nx) / float(ny), aperture, dist_to_focus);

	world = randomScence(500, 10);

	world->AddHitable(new Sphere(
		new SdfSphere(Vector3(0, -1000.1f, 1.0f), 1000.0f),
		new Lambertian(new Constant_Texture(Vector3(1.0f, 1.0f, 1.0f)))
	));
	world->AddHitable(new Sphere(
		new SdfSphere(Vector3(3.02f, 1.0f, -1.0f), 1.0f),
		new Lambertian(new Constant_Texture(Vector3(0.3f, 0.3f, 1.0f)))
	));
	world->AddHitable(new Sphere(
		new SdfSphere(Vector3(1.01f, 1.0f, -1.0f), 1.0f),
		new Metal(new Constant_Texture(Vector3(1.0f, 1.0f, 1.0f)), 0.0f)
	));
	world->AddHitable(new Sphere(
		new SdfSphere(Vector3(-1.01f, 1.0f, -1.0f), 1.0f),
		new Dielectric(new Constant_Texture(Vector3(1.0f, 1.0f, 1.0f)),1.4f)
	));
	world->AddHitable(new Sphere(
		new SdfSphere(Vector3(-3.02f, 1.0f, -1.0f), 1.0f),
		new Phong(new Constant_Texture(Vector3(1.0f, 0.3f, 0.3f)))
	));

	bvh = new BVH(world->list, world->size, 0, 0);

	skybox = new Skybox(new Illumination(
		/*new Customize_Texture(
		[](const float& u, const float& v, const Vector3& p)->Vector3
		{
			float t = 0.5f * (p[1] + 1.0f);
			return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.3f, 0.5f, 0.9f);
		}
	)*/
		new Image_Texture("skybox.jpg")
		, 0.8f));
}

void Scence::LoadCheckingTexture()
{
	Vector3 lookFrom(278.0f, 278.0f, -900.0f);
	Vector3 lookAt(278.0f, 278.0f, 0.0f);
	float dist_to_focus = 10.0f;
	float aperture = 0.0f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 78, float(nx) / float(ny), aperture, dist_to_focus);

	world = randomScence(10, 0);
	world->AddHitable(new Sphere(
		new SdfSphere({ 0, 1000.5f, 1.0f }, 1000.0f), new Lambertian(
			new Customize_Texture([](const float& u, const float& v, const Vector3& p)->Vector3 {
				float sine = sin(4.0f * p[0]) * sin(4.0f * p[1]) * sin(4.0f * p[2]);
				if (sine > 0)
				{
					return { 0.25f,0.65f,0.1f }; //  DarkGreen
				}
				else
				{
					return { 0.0f,0.0f,0.0f }; //  Black
				}
				})
		)));

	//world->AddHitable()
}

void Scence::LoadPerlinNoise()
{
	world = randomScence(10, 0);
}

void Scence::LoadCornellBox()
{
	// camera init
	Vector3 lookFrom(278.0f, 278.0f, -800.0f);
	Vector3 lookAt(278.0f, 278.0f, 0.0f);
	float dist_to_focus = 10.0f;
	float aperture = 0.0f;
	camera = new Camera(lookFrom, lookAt, { 0,1,0 }, 38, float(nx) / float(ny), aperture, dist_to_focus);

	// world init
	world = randomScence(40, 0);

	world->AddHitable(new Rect(new SdfRect_xy(0.0f, 0.0f, 555.0f, 555.0f, 555.0f),
		new Lambertian(new Constant_Texture({ 0.73f,0.73f,0.73f }))));

	world->AddHitable(new Rect(new SdfRect_xz(0.0f, 0.0f, 555.0f, 555.0f, 555.0f),
		new Lambertian(new Constant_Texture({ 0.73f,0.73f,0.73f }))));

	world->AddHitable(new Rect(new SdfRect_xz(0.0f, 0.0f, 555.0f, 555.0f, 0.0f),
		new Lambertian(new Constant_Texture({ 0.73f,0.73f,0.73f }))));

	world->AddHitable(new Rect(new SdfRect_yz(0.0f, 0.0f, 555.0f, 555.0f, 555.0f),
		new Lambertian(new Constant_Texture({ 0.12f,0.45f,0.15f }))));

	world->AddHitable(new Rect(new SdfRect_yz(0.0f, 0.0f, 555.0f, 555.0f, 0.0f),
		new Lambertian(new Constant_Texture({ 0.65f,0.05f,0.05f }))));

	world->AddHitable(new Rect(new SdfRect_xz(213, 227, 343, 332, 554),
		new Illumination(new Constant_Texture({ 1.0f,1.0f,1.0f }), 10.0f)));

	world->AddHitable(new Box(
		new SdfTranslate(new SdfRotate_Y(new SdfBox({ 0,0,0 }, { 165,165,165 }), -18), { 130,0,65 })
		, new Lambertian(new Constant_Texture({ 1.0f,1.0f,1.0f }))));

	world->AddHitable(new Box(
		new SdfTranslate(new SdfRotate_Y(new SdfBox({ 0,0,0 }, { 165,330,165 }), 15), { 265,0,295 })
		, new Lambertian(new Constant_Texture({ 1.0f,1.0f,1.0f }))));

	/*world->AddHitable(new Sphere(new SdfSphere({ 212.5f,235.0f,147.5f }, 70.0f),
		new Metal(new Constant_Texture({ 1.0f,1.0f,1.0f }), 0.0f)));

	world->AddHitable(new Sphere(new SdfSphere({ 347.5f,420.0f,377.5f }, 90.0f),
		new Lambertian(new Constant_Texture({ 0.4f,0.4f,1.0f }))));*/

	// skybox init
	skybox = new Skybox(new Illumination(
		new Customize_Texture(
			[](const float& u, const float& v, const Vector3& p)->Vector3
			{
				float t = 0.5f * (p[1] + 1.0f);
				return (1.0f - t) * Vector3(1.0f, 1.0f, 1.0f) + t * Vector3(0.3f, 0.5f, 0.9f);
				//return { 0,0,0 };
			})
		, 1.0f));
}

HitList* Scence::randomScence(int maxSize, int randomIndex)
{
	Hitable** list = new Hitable * [maxSize + 1];

	int i = 0;
	for (int x = -randomIndex; x < randomIndex; x++)
	{
		for (int y = -randomIndex; y < randomIndex; y++)
		{
			float choose_mat = RayMath::Drand48();
			Vector3 center(x + 0.9f * RayMath::Drand48(), 0.2f, y + 0.9f * RayMath::Drand48());
			if ((center - Vector3(4.0f, 0.2f, 0.0f)).Magnitude() > 0.9f)
			{
				if (choose_mat < 0.7f)
				{
					list[i++] = new Sphere(new SdfSphere(center, 0.2f), new Lambertian(
						new Constant_Texture({ (float)RayMath::Drand48() * (float)RayMath::Drand48() ,
						(float)RayMath::Drand48() * (float)RayMath::Drand48() ,
						(float)RayMath::Drand48() * (float)RayMath::Drand48() })));
				}
				else if (choose_mat < 0.85f)
				{
					list[i++] = new Sphere(new SdfSphere(center, 0.2f), new Metal(
						new Constant_Texture({
						0.5f * (float)(1 + RayMath::Drand48()),
						0.5f * (float)(1 + RayMath::Drand48()),
						0.5f * (float)(1 + RayMath::Drand48()) }),
						0.5f * RayMath::Drand48()));
				}
				else
				{
					list[i++] = new Sphere(new SdfSphere(center, 0.2f),
						new Dielectric(new Constant_Texture({ 1,1,1 }), 1.5f));
				}
			}
		}
	}

	return new HitList(list, i, maxSize);
}