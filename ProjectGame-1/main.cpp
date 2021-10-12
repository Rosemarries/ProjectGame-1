#define _CRT_SECURE_NO_WARNINGS 1
#include<stdio.h>
#include<math.h>
#include<SFML/Graphics.hpp>
#include"Animation.h"

#define screen_x 1080
#define screen_y 720
#define PI 3.14159265

struct player_status {
	int hp = 5;
	int myArtifact = 0;
	float vectorX = 0;
	float vectorY = 0;
	int lookside = 2;
};

struct player_bullet {
	float bulletVectorX;
	float bulletVectorY;
	float bulletSpeed = 2;
	bool bulletState = 0;
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
	sf::CircleShape playerBullet(20.0f);
	player.setOrigin(45.0f, 62.5f);
	player.setPosition(100.0f, 100.0f);

	sf::Texture playerTexture;
	sf::Texture playerBulletTexture;
	playerTexture.loadFromFile("TheLost-4.png");
	playerBulletTexture.loadFromFile("CharacterBullet-1.png");
	player.setTexture(&playerTexture);
	playerBullet.setTexture(&playerBulletTexture);

	Animation animation(&playerTexture, sf::Vector2u(4, 10), 0.3f);
	
	player_status player_status;
	player_bullet player_bullet;
	artifact artifact;

	float deltaTime = 0.0f;
	int playerPicRow = 0;
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

		sf::Vector2f playerPos = player.getPosition();
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
			if (playerPos.x > 0) {
				player.move(-0.75f, 0.0f);
			}
			playerPicRow = 3;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
			if (playerPos.x < screen_x - 1) {
				player.move(0.75f, 0.0f);
			}
			playerPicRow = 1;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
			if (playerPos.y > 0) {
				player.move(0.0f, -0.75f);
			}
			playerPicRow = 2;
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
			if (playerPos.y < screen_y - 1) {
				player.move(0.0f, 0.75f);
			}
			playerPicRow = 0;
		}
		if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			player_bullet.bulletState = 1;
			sf::Vector2i mousePos = sf::Mouse::getPosition(window);
			playerBullet.setPosition(playerPos);
			player_bullet.bulletAngle = (float)180 / PI * atan2(abs(mousePos.y - playerPos.y), abs(mousePos.x - playerPos.x));
			if (player_bullet.bulletState == 1) {
				if (playerPos.x <= mousePos.x) {
					if (player_bullet.bulletAngle <= 45) {
						player_bullet.bulletVectorX = player_bullet.bulletSpeed;
						player_bullet.bulletVectorY = 0;
					}
					else {
						if (playerPos.y < mousePos.y) {
							player_bullet.bulletVectorX = 0;
							player_bullet.bulletVectorY = player_bullet.bulletSpeed;
						}
						else {
							player_bullet.bulletVectorX = 0;
							player_bullet.bulletVectorY = (-1) * player_bullet.bulletSpeed;
						}
					}
				}
				else {
					if (player_bullet.bulletAngle <= 45) {
						player_bullet.bulletVectorX = (-1) * player_bullet.bulletSpeed;
						player_bullet.bulletVectorY = 0;
					}
					else {
						if (playerPos.y < mousePos.y) {
							player_bullet.bulletVectorX = 0;
							player_bullet.bulletVectorY = player_bullet.bulletSpeed;
						}
						else {
							player_bullet.bulletVectorX = 0;
							player_bullet.bulletVectorY = (-1) * player_bullet.bulletSpeed;
						}
					}
				}
			}
		}

		animation.Update(playerPicRow, deltaTime);
		player.setTextureRect(animation.uvRect);
		window.clear(sf::Color(150, 150, 150));
		window.draw(player);

		if (player_bullet.bulletState == 1) {
			playerBullet.move(player_bullet.bulletVectorX, player_bullet.bulletVectorY);
			sf::Vector2f playerBulletPos = playerBullet.getPosition();
			if (playerBulletPos.x <= 0 || playerBulletPos.y <= 0 || playerBulletPos.x >= screen_x || playerBulletPos.y >= screen_y) {
				player_bullet.bulletState = 0;
			}
			else {
				window.draw(playerBullet);
			}
		}
        window.display();
    }

    return 0;
}