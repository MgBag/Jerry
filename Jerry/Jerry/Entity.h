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
	Entity(float x, float y, float width, float height, ALLEGRO_COLOR color, float velocity, float direction);
	~Entity();

	Position* GetPosition();

	void SetPosition(float x, float y);

	ALLEGRO_COLOR GetColor();
	void SetColor(ALLEGRO_COLOR color);

	float GetVelocity();
	void SetVelocity(float velocity);

	float GetDirection();
	void SetDirection(float direction);

	void MoveToOffset(float x, float y);
	void MoveToOffset();

	float GetWidth();
	float GetHeight();

	Position* GetOffset();

};