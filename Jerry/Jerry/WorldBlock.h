#pragma once
#include "Position.h"

class WorldBlock
{
private:
	Position m_A;
	Position m_B;
public:
	WorldBlock(float x1, float y1, float x2, float y2);
	~WorldBlock();

	Position* GetA();
	Position* GetB();
};

