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
	WorldBlock(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color);
	~WorldBlock();

	Coordinates* GetA();
	Coordinates* GetB();

	void WorldBlock::SetColor(ALLEGRO_COLOR color);
	ALLEGRO_COLOR WorldBlock::GetColor();
};

