#include "WorldBlock.h"


WorldBlock::WorldBlock(double x1, double y1, double x2, double y2, ALLEGRO_COLOR color, ItemType type)
{
	m_A = Coordinates(x1, y1);
	m_B = Coordinates(x2, y2);
	m_Color = color;
	m_Type = type;
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

ItemType WorldBlock::GetType()
{
	return m_Type;
}
