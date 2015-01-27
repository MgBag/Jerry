#include "WorldScenery.h"

WorldScenery::WorldScenery(int x, int y, ItemType type, void* data)
{
	m_Coordinates = Coordinates(x, y);
	m_Type = type;
	m_Data = data;
}

WorldScenery::~WorldScenery()
{
}

Coordinates* WorldScenery::GetCoordinates()
{
	return &m_Coordinates;
}

ItemType WorldScenery::GetType()
{
	return m_Type;
}

void* WorldScenery::GetData()
{
	return m_Data;
}