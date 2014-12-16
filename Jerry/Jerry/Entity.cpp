#include "Entity.h"

Entity::Entity(float x, float y, float width, float height, ALLEGRO_COLOR color, float velocity, float angle)
{
	m_Coordinates = new Coordinates(x, y);
	m_Width = width;
	m_Height = height;
	m_Color = color;
	m_VelocityVector = new VelocityVector(velocity, angle);
}

Entity::~Entity()
{
}

Coordinates* Entity::GetCoordinates()
{
	return m_Coordinates;
}

void Entity::SetCoordinates(float x, float y)
{
	m_Coordinates->X = x;
	m_Coordinates->Y = y;
}

void Entity::SetCoordinates(Coordinates* coordinates)
{
	m_Coordinates = coordinates;
}

float Entity::GetHeight()
{
	return m_Height;
}

float Entity::GetWidth()
{
	return m_Width;
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

void Entity::MoveToOffset(float x, float y)
{
	Coordinates* offset = phys.VectorToOffset(m_VelocityVector);

	m_Coordinates->X += offset->X;
	m_Coordinates->Y += offset->Y;

	delete offset;
}

VelocityVector* Entity::GetVelocityVector()
{
	return m_VelocityVector;
}

void Entity::SetVelocityVector(float velocity, float angle)
{
	m_VelocityVector->Velocity = velocity;
	m_VelocityVector->Angle = angle;
}

void Entity::SetVelocityVector(VelocityVector* velocityVector)
{
	m_VelocityVector = velocityVector;
}