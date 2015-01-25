#include "WorldEntity.h"


WorldEntity::WorldEntity(double x, double y, double width, double height, ALLEGRO_COLOR color, ItemType type)
{
	m_A = Coordinates(x, y);
	m_B = Coordinates(x + width, y + height);
	m_Width = width;
	m_Height = height;
	m_Color = color;
	m_Type = type;
	m_Remove = false;
}


WorldEntity::~WorldEntity()
{
}

Coordinates* WorldEntity::GetA()
{
	return &m_A;
}

Coordinates* WorldEntity::GetB()
{
	return &m_B;
}

void WorldEntity::SetColor(ALLEGRO_COLOR color)
{
	m_Color = color;
}

ALLEGRO_COLOR WorldEntity::GetColor()
{
	return m_Color;
}

ItemType WorldEntity::GetType()
{
	return m_Type;
}

double WorldEntity::GetWidth()
{
	return m_Width;
}

double WorldEntity::GetHeight()
{
	return m_Height;
}

bool WorldEntity::operator == (const WorldEntity& w)
{
	if (this == &w)
		return true;
	return false;
}

bool WorldEntity::GetRemove()
{
	return m_Remove;
}

void WorldEntity::SetRemove(bool r)
{
	m_Remove = r;
}