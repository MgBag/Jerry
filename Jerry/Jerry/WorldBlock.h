#pragma once
#include <allegro5/color.h>
#include "Coordinates.h"

class WorldBlock
{
private:
	Coordinates m_A;
	Coordinates m_B;
	ALLEGRO_COLOR m_Color;
public:
	WorldBlock(double x1, double y1, double x2, double y2, ALLEGRO_COLOR color);
	~WorldBlock();

	Coordinates* GetA();
	Coordinates* GetB();

	void WorldBlock::SetColor(ALLEGRO_COLOR color);
	ALLEGRO_COLOR WorldBlock::GetColor();
};

