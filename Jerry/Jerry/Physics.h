#pragma once
#define _USE_MATH_DEFINES 

#include <cmath>
#include <vector>
#include <list>
#include <iostream>
#include <math.h>
#include "Entity.h"
#include "WorldBlock.h"
#include "Coordinates.h"
#include "VelocityVector.h"
#include "Constants.h"
#include "WorldEntity.h"

using namespace std;

class Physics
{
private:
	static bool WillCollide(Entity* entity, list<WorldBlock>* world, list<Entity>* entities, list<WorldEntity>* worldEntities);
	static bool AreInRange(Coordinates* entOff, Coordinates* entACo, Coordinates* entBCo, Coordinates* xA, Coordinates* xB);
	static Coordinates* GetCollisionOffset(Entity* entity, Coordinates* xA, Coordinates* xB, double xStep, double yStep, double* minX, double* minY, double* maxX, double* maxY, bool* minXIsEnt, bool* minYIsEnt);
	static void XCollisionBehaviour(int xIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem);
	static void YCollisionBehaviour(int yIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem);
	static void XYCollisionBehaviour(int xIndex, int yIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem);
public:
	static bool AreColliding(Coordinates* a1, Coordinates* a2, Coordinates* b1, Coordinates* b2);
	static void Collide(list<Entity>::iterator ent, list<WorldBlock> *world, list<Entity>* entities, list<WorldEntity>* worldEntities);
	static void ApplyGravity(Entity* ent);
	static VelocityVector* OffsetToVector(Coordinates* offset);
	static VelocityVector* OffsetToVector(double x, double y);
	static Coordinates* VectorToOffset(VelocityVector* vector);
	static Coordinates* VectorToOffset(double velocity, double angle);
	static double OffsetToAngle(double x, double y);
	
	static void ApplyPhysics(list<Entity>* ent, list<WorldBlock>* world, list<WorldEntity>* worldEntities);
};