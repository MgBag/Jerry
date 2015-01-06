 #include "Entity.h"

Entity::Entity(float x, float y, float width, float height, float xOffset, float yOffset, ALLEGRO_COLOR color, EntityType type, int burstID)
{
	m_ACoordinates = Coordinates(x, y);
	m_BCoordinates = Coordinates(x + width, y + height);
	m_Width = width;
	m_Height = height;
	m_Offset = Coordinates(xOffset, yOffset);
	m_Color = color;
	m_Type = type;
	m_Hit = false;
	m_BurstID = burstID;
}

Entity::~Entity()
{
}

// Do not delete! This returns the object in the Entity
Coordinates* Entity::GetACoordinates()
{
	return &m_ACoordinates;
}

Coordinates* Entity::GetBCoordinates()
{
	return &m_BCoordinates;
}

void Entity::SetCoordinates(float x, float y)
{
	m_ACoordinates.X = x;
	m_ACoordinates.Y = y;
	m_BCoordinates.X = x + m_Width;
	m_BCoordinates.Y = y + m_Height;
}

float Entity::GetHeight()
{
	return m_Height;

}

float Entity::GetWidth()
{
	return m_Width;
}

void Entity::SetHeight(float height)
{
	m_Height = height;
	m_BCoordinates.Y = m_ACoordinates.Y + height;
}

void Entity::SetWidth(float width)
{
	m_Width = width;
	m_BCoordinates.X = m_ACoordinates.X + width;
}

void Entity::SetColor(ALLEGRO_COLOR color)
{
	m_Color = color;
}

ALLEGRO_COLOR Entity::GetColor()
{
	return m_Color;
}

void Entity::MoveToOffset(float x, float y)
{
	m_ACoordinates.X += x;
	m_ACoordinates.Y += y;	
	m_BCoordinates.X += x;
	m_BCoordinates.Y += y;
}

void Entity::MoveToOffset()
{
	m_ACoordinates.X += m_Offset.X;
	m_ACoordinates.Y += m_Offset.Y;
	m_BCoordinates.X += m_Offset.X;
	m_BCoordinates.Y += m_Offset.Y;
}

// Do not delete! This returns the object in the Entity
Coordinates* Entity::GetOffset()
{
	return &m_Offset;
}

void Entity::SetOffset(float xOffset, float yOffset)
{
	m_Offset.X = xOffset;
	m_Offset.Y = yOffset;
}

EntityType Entity::getType()
{
	return m_Type;
}

bool Entity::GetHit()
{
	return m_Hit;
}

void Entity::SetHit(bool hit)
{
	m_Hit = hit;
}

int Entity::GetBurstID()
{
	return m_BurstID;
}