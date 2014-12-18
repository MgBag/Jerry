#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <list>
#include <iostream>
#include "Entity.h"
#include "WorldBlock.h"
#include "Coordinates.h"
#include "VelocityVector.h"

using namespace std;

class Physics
{
private:
	static void Collide(Entity* ent, list<WorldBlock> *world);
	static void ApplyGravity(Entity* ent);
	static void MoveEntity(Entity* ent);
	static void setMinMax(Coordinates* min, Coordinates* max, Coordinates* minOffset, Coordinates* maxOffset, Coordinates* entACo, Coordinates* entBCo, Coordinates* entOff, Coordinates* worACo, Coordinates* worBCo);

public:
	static VelocityVector* OffsetToVector(Coordinates* offset);
	static VelocityVector* OffsetToVector(float x, float y);
	static Coordinates* VectorToOffset(VelocityVector* vector);
	static Coordinates* VectorToOffset(float velocity, float angle);
	
	static void ApplyPhysics(vector<Entity>* ent, list<WorldBlock>* world);
};