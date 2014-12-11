#include "Entity.h"

Entity::Entity(float x, float y, float width, float height, ALLEGRO_COLOR color, float velocity, float direction)
{
	m_Position = Position(x, y);
	m_Width = width;
	m_Height = height;
	m_Color = color;
	m_Velocity = velocity;
	SetDirection(direction);
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

	if (m_Direction > (3 * M_PI_2))
	{
		offset->X = cos(m_Direction - (3 * M_PI_2)) * m_Velocity;
		offset->Y = sin(m_Direction - (3 * M_PI_2)) * m_Velocity * -1;
	}
	else if (m_Direction > M_PI)
	{
		offset->X = sin(m_Direction - M_PI) * m_Velocity;
		offset->Y = cos(m_Direction - M_PI) * m_Velocity;
	}
	else if (m_Direction > M_PI_2)
	{
		offset->X = cos(m_Direction - M_PI_2) * m_Velocity * -1;
		offset->Y = sin(m_Direction - M_PI_2) * m_Velocity;
	}
	else
	{
		offset->X = sin(m_Direction) * m_Velocity * -1;
		offset->Y = cos(m_Direction) * m_Velocity * -1;
	}

	return offset;
}

void Entity::SetVectorByOffset(float x, float y)
{
	m_Velocity = sqrt(x * x + y * y);

	if (x < 0.0 && y < 0.0)
	{
		m_Direction = atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		m_Direction = atan(y / (x * -1.0)) + M_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		m_Direction = atan(x / y) + M_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		m_Direction = atan((y * -1.0) / x) + 3 * M_PI_2;
	}
	else if (x == 0.0 && y > 0.0)
	{
		m_Direction = M_PI;
	}
}