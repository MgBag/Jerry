#include "WorldBlock.h"


WorldBlock::WorldBlock(float x1, float y1, float x2, float y2)
{
	m_A = Position(x1, y1);
	m_B = Position(x2, y2);
}

WorldBlock::~WorldBlock()
{
}

Position* WorldBlock::GetA()
{
	return &m_A;
}

Position* WorldBlock::GetB()
{
	return &m_B;
}