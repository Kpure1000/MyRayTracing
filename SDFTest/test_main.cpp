#include<SFML/Graphics.hpp>
#include<iostream>
#include"RayLauncher.h"
#include"Material.h"
#include"Sdf.h"
#include"Sphere.h"
#include"InterHit.h"
#include"UnionHit.h"
#include"DifferenceHit.h"
using namespace ry;
using namespace sdf;
using namespace std;

#define CannotResize  (sf::Style::Titlebar |  sf::Style::Close)
#define KeyPressing sf::Event::KeyPressed
#define KeyReleasing sf::Event::KeyReleased
#define KeyEvent(EV) (sf::Keyboard::isKeyPressed(EV))

int main()
{
	unsigned int width = 1024, height = 768;

	std::cout << "SDF test in ray tracing, 2D, with SFML.\nstart.\n\n";

	sf::RenderWindow App(sf::VideoMode(width, height), "RayTracingTest",
		sf::Style::Close | sf::Style::Titlebar | sf::Style::Resize);

#pragma region worldInit
	HitList* world = new HitList(1);

	/*world->list[0] = new Sphere(SdfSphere({ 300,300 }, 160), new Dielectric(1.3));
	world->list[1] = new Sphere(SdfSphere({ 420,420 }, 200), new Dielectric(1.3));*/


	/*world->list[0] = new Sphere(new SdfSphere({ 600,200 }, 60), new Metal());
	world->list[1] = new Sphere(new SdfSphere({ 330,420 }, 50), new Simple());
	world->list[2] = new Sphere(new SdfSphere({ 630,520 }, 90), new Dielectric(1.1f));
	world->list[3] = new Sphere(new SdfSphere({ 430,420 }, 40), new Dielectric(1.3f));
	world->list[4] = new Sphere(new SdfSphere({ 730,620 }, 70), new Metal());*/
	
	world->list[0] = new DifferenceHit (new Sphere(new SdfSphere({ 400.0f,400.0f }, 230), new Dielectric(1.3f)),
		new Sphere(new SdfSphere({ 500.0f,500.0f }, 230), new Dielectric(1.5f)), new Dielectric(1.9f));

	/*world->list[0] = new UnionHit(new Sphere(new SdfSphere({ 400.0f,400.0f }, 230), new Metal()),
		new Sphere(new SdfSphere({ 500.0f,500.0f }, 230), new Dielectric(1.5f)), new Dielectric(1.9f));*/


	//world->list[1] = new InterHit(SdfSphere({ 420,420 }, 200), new Dielectric(1.3));
#pragma endregion

	RayLauncher rayLauncher({ 300,10 }, { 10,100 }, world,20);

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

		rayLauncher.Update((Vector2f)Mouse::getPosition(App), Mouse::isButtonPressed(Mouse::Button::Left));

		App.clear(sf::Color(40, 40, 40, 255));
		
		//render
		App.draw(*world);

		App.draw(rayLauncher);

		App.display();
	}
	return 0;
}