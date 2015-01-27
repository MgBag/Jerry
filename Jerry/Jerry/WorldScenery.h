#pragma once

#include "Constants.h"
#include "Coordinates.h"

class WorldScenery
{
private:
	ItemType m_Type;
	void* m_Data;
	Coordinates m_Coordinates;
public:
	WorldScenery(int x, int y, ItemType type, void* data);
	~WorldScenery();

	ItemType GetType();

	void* GetData();

	Coordinates* GetCoordinates();
};