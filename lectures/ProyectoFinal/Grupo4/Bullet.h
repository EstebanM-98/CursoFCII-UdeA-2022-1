#ifndef BULLET_H
#define BULLET_H


#include<SFML/Graphics.hpp>
#include<iostream>

class Bullet
{
private:
    sf::Sprite shape;

	sf::Vector2f direction;
	float movementSpeed;


public:
    Bullet();
    Bullet(sf::Texture* texture, float pos_x, float pos_y, float dir_x, float dir_y, float movement_speed);
    ~Bullet();

	const sf::FloatRect getBounds() const; // for collision with enemies
	const sf::Vector2f getPosition() const;

	void update();
	void render(sf::RenderTarget* target);
};
#endif