#pragma once
#define _USE_MATH_DEFINES 

#include <vector>
#include "Entity.h"
#include "WorldBlock.h"
#include "Position.h"

using namespace std;

class Physics
{
private:
	static bool WillCollide(Entity* ent, vector<WorldBlock*> *world);
	static void ApplyGravity(Entity* ent);
public:
	static void ApplyPhysics(vector<Entity*> *ent, vector<WorldBlock*> *world);
};

