#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <allegro5/allegro.h>
#include "Position.h"
#include "Constants.h"

class Entity
{
private:
	Position m_Position;
	float m_Width;
	float m_Height;
	ALLEGRO_COLOR m_Color;
	float m_Velocity;
	float m_Direction;
public:
	Entity(float x1, float y1, float width, float height, ALLEGRO_COLOR color, float velocity, float direction);
	~Entity();

	Position* getPosition();

	void setPosition(float x, float y);

	ALLEGRO_COLOR getColor();
	void setColor(ALLEGRO_COLOR color);

	float getVelocity();
	void setVelocity(float velocity);

	float getDirection();
	void setDirection(float direction);

	void moveToOffset(float x, float y);
	void moveToOffset();

	float getWidth();
	float getHeight();

	Position* getOffset();

	void setVectorByOffset(float x, float y);
};