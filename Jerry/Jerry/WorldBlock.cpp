#include "WorldBlock.h"


WorldBlock::WorldBlock(float x1, float y1, float x2, float y2, ALLEGRO_COLOR color)
{
	m_A = Coordinates(x1, y1);
	m_B = Coordinates(x2, y2);
	m_Color = color;
}

WorldBlock::~WorldBlock()
{
}

Coordinates* WorldBlock::GetA()
{
	return &m_A;
}

Coordinates* WorldBlock::GetB()
{
	return &m_B;
}

void WorldBlock::SetColor(ALLEGRO_COLOR color)
{
	m_Color = color;
}

ALLEGRO_COLOR WorldBlock::GetColor()
{
	return m_Color;
}
