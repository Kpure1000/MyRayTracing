#include<SFML/Graphics.hpp>
#include<iostream>
#include"RayLauncher.h"
#include"Material.h"
#include"Sdf.h"
#include"Sphere.h"
#include"InterHit.h"
#include"UnionHit.h"
#include"DifferenceHit.h"
#include"BVH.h"
using namespace ry;
using namespace sdf;
using namespace std;

#define CannotResize  (sf::Style::Titlebar |  sf::Style::Close)
#define KeyPressing sf::Event::KeyPressed
#define KeyReleasing sf::Event::KeyReleased
#define KeyEvent(EV) (sf::Keyboard::isKeyPressed(EV))

int main()
{
	Srand48((unsigned int)time(NULL));
	unsigned int width = 1920, height = 1080;

	std::cout << "SDF test in ray tracing, 2D, with SFML.\nstart.\n\n";

	sf::RenderWindow App(sf::VideoMode(width, height), "RayTracingTest",
		sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize);

#pragma region worldInit
	int maxSize = 40;
	HitList* world = new HitList(maxSize);

	for (size_t i = 0; i < maxSize; i++)
	{
		int r = rand() % 10;
		if (r < 6)
		{
			world->AddHitable(new Sphere(new SdfSphere(
				{ (float)width - rand() % width,(float)height - rand() % height }, 10.0f - rand() % 7
			), new Dielectric(1.5f)));
		}
		else if (r < 8)
		{
			world->AddHitable(new Sphere(new SdfSphere(
				{ (float)width - rand() % width ,(float)height - rand() % height }, 10.0f - rand() % 7
			), new Metal()));
		}
		else
		{
			world->AddHitable(new Sphere(new SdfSphere(
				{ (float)width - rand() % width,(float)height - rand() % height }, 10.0f - rand() % 7
			), new Metal()));
		}
	}

#pragma endregion

	//BVH bvh(world->list, world->curSize);

	TestBVH testBvh(world->list, world->curSize);

	//RayLauncher rayLauncher({ 300,10 }, { 10,100 }, world, 2000);
	//RayLauncher rayLauncher({ 300,10 }, { 10,100 }, &bvh, 2000);
	RayLauncher rayLauncher({ 300,10 }, { 10,100 }, &testBvh, 2000);


	while (App.isOpen())
	{
		sf::Event ev;
		while (App.pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed || (KeyPressing && KeyEvent(sf::Keyboard::Key::Escape)))
			{
				App.close();
			}
		}

		//rayLauncher.Update((Vector2f)Mouse::getPosition(App), Mouse::isButtonPressed(Mouse::Button::Left));

		//rayLauncher.BVH_Update((Vector2f)Mouse::getPosition(App), Mouse::isButtonPressed(Mouse::Button::Left));

		rayLauncher.TestBVH_Update((Vector2f)Mouse::getPosition(App), Mouse::isButtonPressed(Mouse::Button::Left));

		App.clear(sf::Color(40, 40, 40, 255));

		//render
		App.draw(*world);

		App.draw(rayLauncher);

		App.display();
	}
	return 0;
}