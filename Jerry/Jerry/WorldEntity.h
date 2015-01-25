#pragma once

#include <allegro5/color.h>
#include "Coordinates.h"
#include "Constants.h"

class WorldEntity
{
private:
	Coordinates m_A;
	Coordinates m_B;
	double m_Width;
	double m_Height;
	ALLEGRO_COLOR m_Color;
	ItemType m_Type;
public:
	WorldEntity(double x, double y, double width, double height, ALLEGRO_COLOR color, ItemType type);
	~WorldEntity();
	Coordinates* GetA();
	Coordinates* GetB();

	double GetWidth();
	double GetHeight();

	ItemType GetType();

	void SetColor(ALLEGRO_COLOR color);
	ALLEGRO_COLOR GetColor();

	bool operator==(const WorldEntity& w);
};
