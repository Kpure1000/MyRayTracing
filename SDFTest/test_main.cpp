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
void RandomWorld(HitList* world, sf::Vector2f origin, int width, int height, int maxSize);

int main()
{
	Srand48(5);
	unsigned int width = 800, height = 600;

	std::cout << "SDF test in ray tracing, 2D, with SFML.\nstart.\n\n";

	sf::RenderWindow App(sf::VideoMode(width, height), "RayTracingTest",
		sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize);

	sf::View camera(sf::FloatRect(0, 0, width, height));

	camera.setViewport(sf::FloatRect(0, 0, 1.0f, 1.0f));

#pragma region worldInit
	int maxSize = 40;
	HitList* world = new HitList(maxSize);

	RandomWorld(world, sf::Vector2f(width * 0.0, height * 0.0), width * 1.0, height * 1.0, maxSize);

#pragma endregion

	shared_ptr<BVH> bvh = make_shared<BVH>(BVH(world->list, world->curSize));

	//RayLauncher rayLauncher({ 300,10 }, { 10,100 }, world, 2000);
	RayLauncher rayLauncher({ 300,10 }, { 10,100 }, bvh.get(), 2000);

	while (App.isOpen())
	{
		sf::Event ev;
		while (App.pollEvent(ev))
		{
			if (ev.type == sf::Event::Closed || (KeyPressing && KeyEvent(sf::Keyboard::Key::Escape)))
			{
				App.close();
			}
			else if (ev.type == sf::Event::Resized)
			{
				float wScale = camera.getSize().x / (float)App.getSize().x;
				float hScale = camera.getSize().y / (float)App.getSize().y;
				camera.setViewport(sf::FloatRect(0, 0, wScale, hScale));

			}
		}

		rayLauncher.Update((Vector2f)Mouse::getPosition(App), Mouse::isButtonPressed(Mouse::Button::Left));

		App.setView(camera);

		App.clear(sf::Color(40, 40, 40, 255));

		//render
		App.draw(*world);

		if(bvh)
			App.draw(*bvh);

		App.draw(rayLauncher);

		App.display();
	}

	delete world;
	return 0;
}

void RandomWorld(HitList* world, sf::Vector2f origin, int width, int height, int maxSize)
{
	for (size_t i = 0; i < maxSize; i++)
	{
		int r = rand() % 10;
		if (r < 5)
		{
			world->AddHitable(new Sphere(new SdfSphere(
				sf::Vector2f((float)width - rand() % width, (float)height - rand() % height) + origin, 20.0f - rand() % 10
			), new Dielectric(1.5f)));
		}
		else
		{
			world->AddHitable(new Sphere(new SdfSphere(
				sf::Vector2f((float)width - rand() % width, (float)height - rand() % height) + origin, 20.0f - rand() % 10
			), new Metal()));
		}
	}

}
