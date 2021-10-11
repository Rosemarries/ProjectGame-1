#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<math.h>
#include<SFML/Graphics.hpp>
#include"Animation.h"

#define screen_x 1080
#define screen_y 720
#define PI 3.14159265

struct character_status {
	int hp = 5;
	int myArtifact = 0;
	int vectorX = 0;
	int vectorY = 0;
	int lookside = 2;
};

struct character_bullet {
	int bulletVectorX;
	int bulletVectorY;
	bool bulletState;
	int bulletDamage = 1;
	float bulletAngle;
};

struct artifact {
	int number = 0;
	int boostCharacterHp = 2;
	int boostCharacterSpeed = 1;
	float boostMonsterDropMoney = 0.5;
	int boostCharacterDamage = 1;
	int reduceMonsterDamage = 1;
	int reduceMonsterHp = 1;
	int boostMonsterDropMed = 1;
};

int main()
{
	sf::RenderWindow window(sf::VideoMode(screen_x, screen_y), "GAME START!");
	sf::RectangleShape player(sf::Vector2f(90.0f, 110.0f));
	//player.setFillColor(sf::Color::Blue);
	player.setOrigin(50.0f, 62.5f);
	sf::Texture playerTexture;
	playerTexture.loadFromFile("TheLost-4.png");
	player.setTexture(&playerTexture);

	Animation animation(&playerTexture, sf::Vector2u(4, 10), 0.3f);
	
	float deltaTime = 0.0f;
	int picRow = 0;
	sf::Clock clock;
	
    while (window.isOpen())
    {
		deltaTime = clock.restart().asSeconds();
        sf::Event evnt;
        while (window.pollEvent(evnt))
        {
			switch (evnt.type) {
			case sf::Event::Closed:
				window.close();
				break;
			}
        }

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			player.move(-0.75f, 0.0f);
			picRow = 3;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			player.move(0.75f, 0.0f);
			picRow = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			player.move(0.0f, -0.75f);
			picRow = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			player.move(0.0f, 0.75f);
			picRow = 0;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			player.setPosition((float)mousePos.x, (float)mousePos.y);
		}
		animation.Update(picRow, deltaTime);
		player.setTextureRect(animation.uvRect);

		window.clear(sf::Color(150, 150, 150));
		window.draw(player);
        window.display();
    }

    return 0;
}