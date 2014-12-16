#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <iostream>
#include "Entity.h"
#include "WorldBlock.h"
#include "Coordinates.h"
#include "VelocityVector.h"

using namespace std;

class Physics
{
private:
	static void Collide(Entity* ent, vector<WorldBlock*> *world);
	static void ApplyGravity(Entity* ent);
public:
	static VelocityVector* OffsetToVector(Coordinates* offset);
	static VelocityVector* OffsetToVector(float x, float y);
	static Coordinates* VectorToOffset(VelocityVector* vector);
	static Coordinates* VectorToOffset(float velocity, float angle);
	
	static void ApplyPhysics(vector<Entity*> *ent, vector<WorldBlock*> *world);
};

