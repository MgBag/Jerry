#pragma once
#define _USE_MATH_DEFINES 

#include <vector>
#include <iostream>
#include "Entity.h"
#include "WorldBlock.h"
#include "Position.h"

using namespace std;

class Physics
{
private:
	static bool WillCollide(Entity* ent, vector<WorldBlock*> *world);
	static void ApplyGravity(Entity* ent);
	static inline void YCollide();
	static inline void XCollide();
public:
	static void ApplyPhysics(vector<Entity*> *ent, vector<WorldBlock*> *world);
};

