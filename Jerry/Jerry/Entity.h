#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <allegro5/color.h>
#include "Coordinates.h"
#include "Constants.h"
#include "VelocityVector.h"

class Entity
{
private:
	Coordinates* m_ACoordinates;
	Coordinates* m_BCoordinates;
	Coordinates* m_Offset;
	float m_Width;
	float m_Height;
	ALLEGRO_COLOR m_Color;
	EntityType m_Type;
	bool m_Hit;
	int m_BurstID;

public:
	Entity(float x, float y, float width, float height, float xOffset, float yOffset, ALLEGRO_COLOR color, EntityType type, int burstID);
	~Entity();

	Coordinates* GetACoordinates();
	Coordinates* GetBCoordinates();
	void SetCoordinates(float x, float y);
	void MoveToOffset(float x, float y);
	void MoveToOffset();
	void MoveToOffset(Coordinates* offset);

	Coordinates* GetOffset();
	void SetOffset(float xOffset, float yOffset);
	void SetOffset(Coordinates* velocityVector);

	ALLEGRO_COLOR GetColor();
	void SetColor(ALLEGRO_COLOR color);

	float GetWidth();
	float GetHeight();

	void SetWidth(float width);
	void SetHeight(float height);

	bool GetHit();
	void SetHit(bool hit);

	int GetBurstID();

	EntityType getType();
};