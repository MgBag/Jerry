 #include "Entity.h"

Entity::Entity(float x, float y, float width, float height, float xOffset, float yOffset, ALLEGRO_COLOR color, EntityType type)
{
	m_Coordinates = new Coordinates(x, y);
	m_Width = width;
	m_Height = height;
	m_Offset = new Coordinates(xOffset, yOffset);
	m_Color = color;
	m_Type = type;
	m_Hit = false;
}

Entity::~Entity()
{
}

// Do not delete! This returns the object in the Entity
Coordinates* Entity::GetCoordinates()
{
	return m_Coordinates;
}

void Entity::SetCoordinates(float x, float y)
{
	m_Coordinates->X = x;
	m_Coordinates->Y = y;
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
}

void Entity::SetWidth(float width)
{
	m_Width = width;
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
	m_Coordinates->X += x;
	m_Coordinates->Y += y;
}

void Entity::MoveToOffset(Coordinates* offset)
{
	m_Coordinates->X += offset->X;
	m_Coordinates->Y += offset->Y;
}

void Entity::MoveToOffset()
{
	m_Coordinates->X += m_Offset->X;
	m_Coordinates->Y += m_Offset->Y;
}

// Do not delete! This returns the object in the Entity
Coordinates* Entity::GetOffset()
{
	return m_Offset;
}

void Entity::SetOffset(float xOffset, float yOffset)
{
	m_Offset->X = xOffset;
	m_Offset->Y = yOffset;
}

void Entity::SetOffset(Coordinates* offset)
{
	delete m_Offset;
	m_Offset = offset;
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