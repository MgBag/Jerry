#pragma once
#include <allegro5/color.h>
#include "Coordinates.h"
#include "Constants.h"

class WorldBlock
{
private:
	Coordinates m_A;
	Coordinates m_B;
	double m_Width;
	double m_Height;
	ALLEGRO_COLOR m_Color;
	ItemType m_Type;
public:
	WorldBlock(double x, double y, double width, double height, ALLEGRO_COLOR color, ItemType type);
	~WorldBlock();

	Coordinates* GetA();
	Coordinates* GetB();
	
	double GetWidth();
	double GetHeight();
	
	ItemType GetType();

	void WorldBlock::SetColor(ALLEGRO_COLOR color);
	ALLEGRO_COLOR WorldBlock::GetColor();
};

