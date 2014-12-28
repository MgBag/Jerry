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
	static void Collide(Entity* ent, list<WorldBlock> *world, list<Entity>* entities);
	static void ApplyGravity(Entity* ent);
	static void MoveEntity(Entity* ent);
	static void setMinMax(Coordinates* min, Coordinates* max, Coordinates* minOffset, Coordinates* maxOffset, Coordinates* entACo, Coordinates* entBCo, Coordinates* entOff, Coordinates* worACo, Coordinates* worBCo);
	static bool WillCollide(Entity* entity, Coordinates* xA, Coordinates* xB);
	static bool AreColliding(Coordinates* a1, Coordinates* a2, Coordinates* b1, Coordinates* b2);
	static bool AreInRange(Entity* entity, Coordinates* xA, Coordinates* xB);
	static bool WillCollide(Entity* entity, list<WorldBlock>* world, list<Entity>* entities);
public:
	static VelocityVector* OffsetToVector(Coordinates* offset);
	static VelocityVector* OffsetToVector(float x, float y);
	static Coordinates* VectorToOffset(VelocityVector* vector);
	static Coordinates* VectorToOffset(float velocity, float angle);
	static float OffsetToAngle(float x, float y);
	
	static void ApplyPhysics(list<Entity>* ent, list<WorldBlock>* world);
};