 #include "Entity.h"

Entity::Entity(double x, double y, double width, double height, double xOffset, double yOffset, ALLEGRO_COLOR color, ItemType type)
{
	m_ACoordinates = Coordinates(x, y);
	m_BCoordinates = Coordinates(x + width, y + height);
	m_Width = width;
	m_Height = height;
	m_Offset = Coordinates(xOffset, yOffset);
	m_Color = color;
	m_Type = type;
	m_Hit = false;
	m_Age = 0;
	m_IsAirBorn = true;
	m_InitialImpactSpeed = 0.0;
	m_LastImpactType = WORLD;
	m_Delete = false;
	m_LastCollPos = UY;
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
	m_Offset.X = m_Offset.X < MAX_ENTITY_VELOCITY ? (m_Offset.X > (MAX_ENTITY_VELOCITY * -1) ? m_Offset.X : (MAX_ENTITY_VELOCITY * -1)) : MAX_ENTITY_VELOCITY;
	m_Offset.Y = m_Offset.Y < MAX_ENTITY_VELOCITY ? (m_Offset.Y > (MAX_ENTITY_VELOCITY * -1) ? m_Offset.Y : (MAX_ENTITY_VELOCITY * -1)) : MAX_ENTITY_VELOCITY;

	return &m_Offset;
}

void Entity::SetOffset(double xOffset, double yOffset)
{
	m_Offset.X = xOffset < MAX_ENTITY_VELOCITY ? (xOffset >(MAX_ENTITY_VELOCITY * -1) ? xOffset : (MAX_ENTITY_VELOCITY * -1)) : MAX_ENTITY_VELOCITY;
	m_Offset.Y = yOffset < MAX_ENTITY_VELOCITY ? (yOffset >(MAX_ENTITY_VELOCITY * -1) ? yOffset : (MAX_ENTITY_VELOCITY * -1)) : MAX_ENTITY_VELOCITY;
}

ItemType Entity::getType()
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

int Entity::GetAge()
{
	return m_Age;
}

void Entity::IncAge()
{
	++m_Age;
}

void Entity::SetAge(int age)
{
	m_Age = age;
}

bool Entity::GetIsAirBorn()
{
	return m_IsAirBorn;
}

void Entity::SetIsAirBorn(bool hit)
{
	m_IsAirBorn = hit;
}

double Entity::GetInitialImpactSpeed()
{
	return m_InitialImpactSpeed;
}

void Entity::SetInitialImpactSpeed(double speed)
{
	m_InitialImpactSpeed = speed;
}

bool Entity::GetIsCrouching()
{
	return m_IsCrouching;
}

void Entity::SetIsCrouching(bool isCrouching)
{
	m_IsCrouching = isCrouching;
}

ItemType Entity::GetLastImpactType()
{
	return m_LastImpactType;
}

void Entity::SetLastImpactType(ItemType impactType)
{
	m_LastImpactType = impactType;
}

double Entity::GetPreviousImpactHeight()
{
	return m_PreviousImpactHeight;
}

void Entity::SetPreviousImpactHeight(double preImpactHeight)
{
	m_PreviousImpactHeight = preImpactHeight;
}

bool Entity::GetDelete()
{
	return m_Delete;
}

void Entity::SetDelete(bool del)
{
	m_Delete = del;
}

bool Entity::operator == (const Entity& e)
{
	if (&*this == &e)
		return true;
	return false;
}

CollPos Entity::GetLastColPos()
{
	return m_LastCollPos;
}

void Entity::SetLastColPos(CollPos collpos)
{
	m_LastCollPos = collpos;
}