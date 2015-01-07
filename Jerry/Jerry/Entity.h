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
	Coordinates m_ACoordinates;
	Coordinates m_BCoordinates;
	Coordinates m_Offset;
	double m_Width;
	double m_Height;
	ALLEGRO_COLOR m_Color;
	EntityType m_Type;
	bool m_Hit;
	int m_BurstID;

public:
	Entity(double x, double y, double width, double height, double xOffset, double yOffset, ALLEGRO_COLOR color, EntityType type, int burstID);
	~Entity();

	Coordinates* GetACoordinates();
	Coordinates* GetBCoordinates();
	void SetCoordinates(double x, double y);
	void MoveToOffset(double x, double y);
	void MoveToOffset();

	Coordinates* GetOffset();
	void SetOffset(double xOffset, double yOffset);

	ALLEGRO_COLOR GetColor();
	void SetColor(ALLEGRO_COLOR color);

	double GetWidth();
	double GetHeight();

	void SetWidth(double width);
	void SetHeight(double height);

	bool GetHit();
	void SetHit(bool hit);

	int GetBurstID();

	EntityType getType();
};