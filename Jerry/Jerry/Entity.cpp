#include "Entity.h"

Entity::Entity(float x, float y, float width, float height, ALLEGRO_COLOR color, float velocity, float direction)
{
	m_Position = Position(x, y);
	m_Width = width;
	m_Height = height;
	m_Color = color;
	m_Velocity = velocity;
	m_Direction = direction;
}

Entity::~Entity()
{
}

Position* Entity::GetPosition()
{
	return &m_Position;
}

void Entity::SetPosition(float x, float y)
{
	m_Position.X = x;
	m_Position.Y = y;
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

void Entity::SetVelocity(float velocity)
{
	m_Velocity = velocity;
}

float Entity::GetVelocity()
{
	return m_Velocity;
}

void Entity::SetDirection(float direction)
{
	m_Direction = direction;
}

float Entity::GetDirection()
{
	return m_Direction;
}

void Entity::MoveToOffset(float x, float y)
{
	m_Position.X += x;
	m_Position.Y += y;
}

void Entity::MoveToOffset()
{
	Position *offset = GetOffset();

	m_Position.X += offset->X;
	m_Position.Y += offset->Y;

	delete offset;
}

Position* Entity::GetOffset()
{
	Position *offset = new Position();

	if (m_Direction == 0.0)
	{
		offset->X = 0.0;
		offset->Y = m_Velocity * -1;
	}
	else if (fmod(m_Direction, FM_3_PI_2) == 0.0)
	{
		offset->X = m_Velocity;
		offset->Y = 0.0;
	}
	else if (fmod(m_Direction, FM_PI) == 0.0)
	{
		offset->X = 0.0;
		offset->Y = m_Velocity;
	}
	else if (fmod(m_Direction, FM_PI_2) == 0.0)
	{
		offset->X = m_Velocity * -1;
		offset->Y = 0.0;
	}
	else if (m_Direction > FM_3_PI_2)
	{
		offset->X = cos(m_Direction - FM_3_PI_2) * m_Velocity;
		offset->Y = sin(m_Direction - FM_3_PI_2) * m_Velocity * -1;
	}
	else if (m_Direction > FM_PI)
	{
		offset->X = sin(m_Direction - FM_PI) * m_Velocity;
		offset->Y = cos(m_Direction - FM_PI) * m_Velocity;
	}
	else if (m_Direction > FM_PI_2)
	{
		offset->X = cos(m_Direction - FM_PI_2) * m_Velocity * -1;
		offset->Y = sin(m_Direction - FM_PI_2) * m_Velocity;
	}
	else
	{
		offset->X = sin(m_Direction) * m_Velocity * -1;
		offset->Y = cos(m_Direction) * m_Velocity * -1;
	}

	return offset;
}

