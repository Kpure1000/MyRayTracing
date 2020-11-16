#include<SFML/Graphics.hpp>
#include<iostream>

using namespace std;

#define CannotResize  (sf::Style::Titlebar |  sf::Style::Close)
#define KeyPressing sf::Event::KeyPressed
#define KeyReleasing sf::Event::KeyReleased
#define KeyEvent(EV) (sf::Keyboard::isKeyPressed(EV))

int main()
{
	unsigned int width = 1024, height = 768;

	std::cout << "¹â×·2D²âÊÔ\n\n";

	sf::RenderWindow App(sf::VideoMode(width, height),
		"RayTracingTest", sf::Style::Close | sf::Style::Titlebar);

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

		App.clear(sf::Color(40, 40, 40, 255));
		
		//render

		App.display();
	}
	return 0;
}