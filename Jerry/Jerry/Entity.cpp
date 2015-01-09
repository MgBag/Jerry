 #include "Entity.h"

Entity::Entity(double x, double y, double width, double height, double xOffset, double yOffset, ALLEGRO_COLOR color, EntityType type, int burstID)
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
	m_BCoordinates.X = m_ACoordinates.X + m_Width;
	m_BCoordinates.Y = m_ACoordinates.Y + m_Height;

	return &m_BCoordinates;
}

void Entity::SetCoordinates(double x, double y)
{
	m_ACoordinates.X = x;
	m_ACoordinates.Y = y;
}

double Entity::GetHeight()
{
	return m_Height;

}

double Entity::GetWidth()
{
	return m_Width;
}

void Entity::SetHeight(double height)
{
	m_Height = height;
	m_BCoordinates.Y = m_ACoordinates.Y + height;
}

void Entity::SetWidth(double width)
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

void Entity::MoveToOffset(double x, double y)
{
	m_ACoordinates.X += x;
	m_ACoordinates.Y += y;	
}

void Entity::MoveToOffset()
{
	m_ACoordinates.X += m_Offset.X;
	m_ACoordinates.Y += m_Offset.Y;
}

// Do not delete! This returns the object in the Entity
Coordinates* Entity::GetOffset()
{
	return &m_Offset;
}

void Entity::SetOffset(double xOffset, double yOffset)
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