#pragma once
#include <allegro5/color.h>
#include "Coordinates.h"
#include "Constants.h"

class WorldBlock
{
private:
	Coordinates m_A;
	Coordinates m_B;
	ALLEGRO_COLOR m_Color;
	ItemType m_Type;
public:
	WorldBlock(double x1, double y1, double x2, double y2, ALLEGRO_COLOR color, ItemType type);
	~WorldBlock();

	Coordinates* GetA();
	Coordinates* GetB();

	ItemType GetType();

	void WorldBlock::SetColor(ALLEGRO_COLOR color);
	ALLEGRO_COLOR WorldBlock::GetColor();
};

