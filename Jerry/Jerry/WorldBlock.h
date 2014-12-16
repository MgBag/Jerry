#pragma once
#include "Coordinates.h"

class WorldBlock
{
private:
	Coordinates m_A;
	Coordinates m_B;
public:
	WorldBlock(float x1, float y1, float x2, float y2);
	~WorldBlock();

	Coordinates* GetA();
	Coordinates* GetB();
};

