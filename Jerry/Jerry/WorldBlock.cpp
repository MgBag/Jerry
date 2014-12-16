#include "WorldBlock.h"


WorldBlock::WorldBlock(float x1, float y1, float x2, float y2)
{
	m_A = Coordinates(x1, y1);
	m_B = Coordinates(x2, y2);
}

WorldBlock::~WorldBlock()
{
}

Coordinates* WorldBlock::GetA()
{
	return &m_A;
}

Coordinates* WorldBlock::GetB()
{
	return &m_B;
}