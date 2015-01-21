#include "WorldBlock.h"


WorldBlock::WorldBlock(double x, double y, double width, double height, ALLEGRO_COLOR color, ItemType type)
{
	m_A = Coordinates(x, y);
	m_B = Coordinates(x + width, y + height);
	m_Width = width;
	m_Height = height;
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

double WorldBlock::GetWidth()
{
	return m_Width;
}

double WorldBlock::GetHeight()
{
	return m_Height;
}