#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <allegro5/allegro.h>
#include "Coordinates.h"
#include "Constants.h"
#include "Physics.h"
#include "VelocityVector.h"

class Entity
{
private:
	Coordinates* m_Coordinates;
	VelocityVector* m_VelocityVector;
	float m_Width;
	float m_Height;
	ALLEGRO_COLOR m_Color;
	Physics phys;
public:
	Entity(float x, float y, float width, float height, ALLEGRO_COLOR color, float velocity, float angle);
	~Entity();

	Coordinates* GetCoordinates();
	void SetCoordinates(float x, float y);
	void SetCoordinates(Coordinates* coordinates);
	void MoveToOffset(float x, float y);
	void MoveToOffset();

	VelocityVector* GetVelocityVector();
	void SetVelocityVector(float velocity, float angle);
	void SetVelocityVector(VelocityVector* velocityVector);

	ALLEGRO_COLOR GetColor();
	void SetColor(ALLEGRO_COLOR color);

	float GetWidth();
	float GetHeight();
};