#include "Physics.h"

// TODO: Memory leaks mang
// TODO: Check logics mang, flows and flaws

void Physics::ApplyPhysics(list<Entity>* entities, list<WorldBlock>* world)
{
	for (list<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		//ApplyGravity(&(*ent));
		Collide(&(*ent), world, entities);
		//MoveEntity(&(*ent));
	}
}

void Physics::ApplyGravity(Entity* ent)
{
	if (ent->getType() == PLAYER || !ent->GetHit())
	{
		Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

		ent->SetVelocityVector(OffsetToVector(offset->X, offset->Y + GRAVITY));

		delete offset;
	}
}

void Physics::MoveEntity(Entity* ent)
{
	Coordinates* offset = VectorToOffset(ent->GetVelocityVector());

	ent->MoveToOffset(offset->X, offset->Y);

	delete offset;
}

void Physics::Collide(Entity* ent, list<WorldBlock>* world, list<Entity>* entities)
{
	Coordinates* entACo = ent->GetCoordinates();
	Coordinates* entBCo = new Coordinates(entACo->X + ent->GetWidth(), entACo->Y + ent->GetHeight());
	Coordinates* entOff = VectorToOffset(ent->GetVelocityVector()); // TODO: Add entity offset conlone function;
	float entVel = ent->GetVelocityVector()->Velocity;
	vector<Coordinates> collisions;
	vector<Coordinates> possitions;
	vector<int> collisionPosition;
	vector<int> collisionType;
	vector<WorldBlock*> collisionBlock;

	// To make sure that a step isn't 1.0 if the offset is equal to teh velocity
	// I fear the ceil useage in this function is the problem, TODO: think of a new way!
	float xStep = entOff->Y == 0 ? entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0)) : entOff->X / ceil(entVel);
	float yStep = entOff->X == 0 ? entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0)) : entOff->Y / ceil(entVel);

	//for (list<Entity>::iterator jel = ++entities->begin(); jel != entities->end(); ++jel)
	//{
	//	Coordinates* jelACo = jel->GetCoordinates();
	//	Coordinates* jelBCo = new Coordinates(jelACo->X + jel->GetWidth(), jelACo->Y + jel->GetHeight());

	//	if (ent != &*jel && jel->GetHit() && !AreColliding(entACo, entBCo, jelACo, jelBCo) && WillCollide(ent, jelACo, jelBCo))
	//	{
	//		float minX, minY, maxX, maxY;
	//		bool minXIsEnt = false, minYIsEnt = false;

	//		Coordinates* colOff = GetCollisionOffset(ent, jelACo, jelBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

	//		if (!(maxX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxY > minY)
	//		{
	//			if (minXIsEnt)
	//			{
	//				possitions.push_back(Coordinates(jelBCo->X, entACo->Y + colOff->Y));
	//				collisionPosition.push_back(LX);
	//			}
	//			else
	//			{
	//				possitions.push_back(Coordinates(jelACo->X - ent->GetWidth(), entACo->Y + colOff->Y));
	//				collisionPosition.push_back(RX);
	//			}

	//		}
	//		else if (maxX > minX && !(maxY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
	//		{
	//			if (minYIsEnt)
	//			{
	//				possitions.push_back(Coordinates(entACo->X + colOff->X, jelBCo->Y));
	//				collisionPosition.push_back(DY);
	//			}
	//			else
	//			{
	//				possitions.push_back(Coordinates(entACo->X + colOff->X, jelACo->Y - ent->GetHeight()));
	//				collisionPosition.push_back(UY);
	//			}
	//		}
	//		else
	//		{
	//			cout << "bad mmkey\n";
	//		}

	//		// TODO: Fix this temp work around
	//		collisionBlock.push_back(new WorldBlock(jelACo->X, jelACo->Y, jelBCo->X, jelBCo->Y, al_map_rgb(220, 20, 20)));
	//		collisions.push_back(Coordinates(colOff->X, colOff->Y));
	//		collisionType.push_back(JELLY);

	//		delete colOff;
	//	}

	//	delete jelBCo;
	//}

	for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
	{
		// TODO: Add might collide
		if (WillCollide(ent, wor->GetA(), wor->GetB()))
		{
			wor->SetColor(al_map_rgb(20, 220, 20));
			//	Coordinates* worACo = wor->GetA();
			//	Coordinates* worBCo = wor->GetB();
			//	float minOffsetX;
			//	float minOffsetY;
			//	float maxOffsetX;
			//	float maxOffsetY;
			//	bool minXIsEnt = false;
			//	bool minYIsEnt = false; 
			//	
			//	float minX, minY, maxX, maxY;

			//	Coordinates* colOff = GetCollisionOffset(ent, worACo, worBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

			//	if (colOff == NULL)
			//	{
			//		cout << "NONONONONOO NO NO. NO!\n";
			//	}

			//	if (AreColliding(entACo, entBCo, wor->GetA(), wor->GetB()))
			//	{
			//		cout << "NONONONONOO NO NO. NO!\n";
			//	}




			//	for (float x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X : x <= entOff->X) && (yStep < 0.0 ? y >= entOff->Y : y <= entOff->Y); x += xStep, y += yStep)
			//	{
			//		// X with Offset
			//		if (worACo->X < entACo->X)
			//		{
			//			minOffsetX = entACo->X + x;
			//			minXIsEnt = true;

			//			maxOffsetX = worBCo->X;
			//		}
			//		else
			//		{
			//			minOffsetX = worACo->X;

			//			maxOffsetX = entBCo->X + x;
			//		}

			//		//Y	with offset
			//		if (worACo->Y < entACo->Y) 
			//		{
			//			minOffsetY = entACo->Y + y;
			//			minYIsEnt = true;

			//			maxOffsetY = worBCo->Y;
			//		}
			//		else
			//		{
			//			minOffsetY = worACo->Y;

			//			maxOffsetY = entBCo->Y + y;
			//		}

			//		// Will Collide
			//		if (maxOffsetX > minOffsetX && maxOffsetY > minOffsetY)
			//		{
			//			if (!(maxOffsetX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minOffsetX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxOffsetY > minOffsetY)
			//			{
			//				if (minXIsEnt)
			//				{
			//					possitions.push_back(Coordinates(worBCo->X, entACo->Y + y));
			//					collisionPosition.push_back(LX);
			//				}
			//				else
			//				{
			//					possitions.push_back(Coordinates(worACo->X - ent->GetWidth(), entACo->Y + y));
			//					collisionPosition.push_back(RX);
			//				}
			//			}
			//			else if (maxOffsetX > minOffsetX && !(maxOffsetY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minOffsetY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
			//			{
			//				if (minYIsEnt)
			//				{
			//					possitions.push_back(Coordinates(entACo->X + x, worBCo->Y));
			//					collisionPosition.push_back(DY);
			//				}
			//				else
			//				{
			//					possitions.push_back(Coordinates(entACo->X + x, worACo->Y - ent->GetHeight()));
			//					collisionPosition.push_back(UY);
			//				}
			//			}

			//			collisionBlock.push_back(&(*wor));
			//			collisions.push_back(Coordinates(x, y));
			//			collisionType.push_back(WORLD);

			//			break;
			//		}
			//	}
		}
	}

	if (collisions.size())
	{
		int closestX = -1;
		int closestY = -1;

		for (int i = 0; i < collisions.size(); ++i)
		{
			if (collisionPosition[i] == LX || collisionPosition[i] == RX)
			{
				if (closestX == -1 ? true : (sqrt(pow(collisions[i].X, 2) + pow(collisions[i].Y, 2)) < sqrt(pow(collisions[closestX].X, 2) + pow(collisions[closestX].Y, 2))))
				{
					closestX = i;
				}
			}
			else if (collisionPosition[i] == UY || collisionPosition[i] == DY)
			{
				if (closestY == -1 ? true : (sqrt(pow(collisions[i].X, 2) + pow(collisions[i].Y, 2)) < sqrt(pow(collisions[closestY].X, 2) + pow(collisions[closestY].Y, 2))))
				{
					closestY = i;
				}
			}
		}

		if (closestX != -1 && closestY != -1)
		{
			if (entBCo->Y == collisionBlock[closestX]->GetA()->Y)
			{
				ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

				if (ent->getType() == PLAYER)
				{
					// Stop the entity if the movement gets below the FRICTION_STOP
					ent->SetVelocityVector(OffsetToVector(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0));
				}
				else
				{
					ent->SetVelocityVector(0.0, 0.0);
					ent->SetHit(true);
				}
			}
			else if (entACo->Y == collisionBlock[closestX]->GetB()->Y)
			{
				ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

				if (ent->getType() == PLAYER)
				{
					ent->SetVelocityVector(OffsetToVector(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0));
				}
				else
				{
					ent->SetVelocityVector(0.0, 0.0);
					ent->SetHit(true);
				}
			}
			else
			{
				ent->SetCoordinates(possitions[closestX].X, possitions[closestY].Y);

				ent->SetVelocityVector(0.0, 0.0);

				if (ent->getType() == PROJECTILE)
				{
					ent->SetHit(true);
				}
			}
		}
		else if (closestY != -1)
		{
			ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

			if (collisionType[closestY] == JELLY)
			{
				ent->SetVelocityVector(OffsetToVector(entOff->X, entOff->Y *-1));
			}
			else if (ent->getType() == PLAYER)
			{
				ent->SetVelocityVector(OffsetToVector(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0));
			}
			else
			{
				ent->SetVelocityVector(0.0, 0.0);
				ent->SetHit(true);
			}
		}
		else if (closestX != -1)
		{
			ent->SetCoordinates(possitions[closestX].X, possitions[closestX].Y);

			if (collisionType[closestX] == JELLY)
			{
				ent->SetVelocityVector(OffsetToVector(entOff->X *-1, entOff->Y));
			}
			else if (ent->getType() == PLAYER)
			{
				ent->SetVelocityVector(OffsetToVector(0.0, entOff->Y < FRICTION_STOP && entOff->Y > FRICTION_STOP * -1 ? 0.0 : entOff->Y * FRICTION));
			}
			else
			{
				ent->SetVelocityVector(0.0, 0.0);
				ent->SetHit(true);
			}
		}

		// Add entities will collide
		if (WillCollide(ent, world, entities))
		{
			Collide(ent, world, entities);
		}
	}

	delete entBCo;
	delete entOff;
}

bool Physics::WillCollide(Entity* entity, list<WorldBlock>* world, list<Entity>* entities)
{
	// Check if the speed is 0 
	//for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
	//{
	//	if (WillCollide(entity, wor->GetA(), wor->GetB()))
	//	{
	//		return true;
	//	}
	//}

	for (list<Entity>::iterator jel = ++entities->begin(); jel != entities->end(); ++jel)
	{
		Coordinates* jelACo = jel->GetCoordinates();
		Coordinates* jelBCo = new Coordinates(jelACo->X + jel->GetWidth(), jelACo->Y + jel->GetHeight());

		if (WillCollide(entity, jelACo, jelBCo))
		{
			delete jelBCo;
			return true;
		}

		delete jelBCo;
	}

	return false;
}

// TODO: Make this return hit Coordinates or null
Coordinates* Physics::GetCollisionOffset(Entity* entity, Coordinates* xA, Coordinates* xB, float xStep, float yStep, float* minX, float* minY, float* maxX, float* maxY, bool* minXIsEnt, bool* minYIsEnt)
{
	Coordinates* entACo = entity->GetCoordinates();
	Coordinates* entBCo = new Coordinates(entACo->X + entity->GetWidth(), entACo->Y + entity->GetHeight());
	Coordinates* entOff = VectorToOffset(entity->GetVelocityVector());

	for (float x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X : x <= entOff->X) && (yStep < 0.0 ? y >= entOff->Y : y <= entOff->Y); x += xStep, y += yStep)
	{
		// X with Offset					//// X with Offset
		if (xA->X < entACo->X + x)			//if (worACo->X < entACo->X + x)
		{									//{
			*minX = entACo->X + x;			//	minOffsetX = entACo->X + x;
			*minXIsEnt = true;				//	minXIsEnt = true;
			//
			*maxX = xB->X;					//	maxOffsetX = worBCo->X;
		}									//}
		else								//else
		{									//{
			*minX = xA->X;					//	minOffsetX = worACo->X;
			//
			*maxX = entBCo->X + x;			//	maxOffsetX = entBCo->X + x;
		}									//}
		//
		//Y	with offset						////Y	with offset
		if (xA->Y < entACo->Y + y)			//if (worACo->Y < entACo->Y + y)
		{									//{
			*minY = entACo->Y + y;			//	minOffsetY = entACo->Y + y;
			*minYIsEnt = true;				//	minYIsEnt = true;
			//
			*maxY = xB->Y;					//	maxOffsetY = worBCo->Y;
		}									//}
		else								//else
		{									//{
			*minY = xA->Y;					//	minOffsetY = worACo->Y;
			//
			*maxY = entBCo->Y + y;			//	maxOffsetY = entBCo->Y + y;
		}									//}
		//
		if (*maxX > *minX && *maxY > *minY)		//// Will Collide
		{									//if (maxOffsetX > minOffsetX && maxOffsetY > minOffsetY)
			delete entBCo;					//{
			delete entOff;
			return new Coordinates(x, y);
		}
	}

	delete entBCo;
	delete entOff;
	return NULL;
}

//bool Physics::AreColliding(Coordinates* aa, Coordinates* ab, Coordinates* ba, Coordinates* bb)
//{
//	float minY;
//	float maxY;
//	float minX;
//	float maxX;
//
//	// X with Offset
//	if (ba->X < aa->X)
//	{
//		minX = aa->X;
//
//		maxX = bb->X;
//	}
//	else
//	{
//		minX = ba->X;
//
//		maxX = ab->X;
//	}
//
//	//Y	with offset
//	if (ba->Y < aa->Y)
//	{
//		minY = aa->Y;
//
//		maxY = bb->Y;
//	}
//	else
//	{
//		minY = ba->Y;
//
//		maxY = ab->Y;
//	}
//
//	if (maxX > minX && maxY > minY)
//	{
//		return true;
//	}
//
//	return false;
//}


// TODO: Refine this function with conditionary statements
bool Physics::AreInRange(Entity* entity, Coordinates* xA, Coordinates* xB)
{
	
}


float Physics::OffsetToAngle(float x, float y)
{
	if (x == 0.0 && y > 0.0)
	{
		return FM_PI;
	}
	else if (x == 0.0 && y < 0.0)
	{
		return 0.0;
	}
	else if (x > 0.0 && y == 0.0)
	{
		return FM_3_PI_2;
	}
	else if (x < 0.0 && y == 0.0)
	{
		return FM_PI_2;
	}
	else if (x < 0.0 && y < 0.0)
	{
		return atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		return atan(y / (x * -1.0)) + FM_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		return atan(x / y) + FM_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		return atan((y * -1.0) / x) + FM_3_PI_2;
	}
	else
	{
		return 0.0;
	}
}

VelocityVector* Physics::OffsetToVector(float x, float y)
{
	VelocityVector* vec = new VelocityVector();

	vec->Velocity = sqrt(x * x + y * y);

	if (x == 0.0 && y > 0.0)
	{
		vec->Angle = FM_PI;
	}
	else if (x == 0.0 && y < 0.0)
	{
		vec->Angle = 0.0;
	}
	else if (x > 0.0 && y == 0.0)
	{
		vec->Angle = FM_3_PI_2;
	}
	else if (x < 0.0 && y == 0.0)
	{
		vec->Angle = FM_PI_2;
	}
	else if (x < 0.0 && y < 0.0)
	{
		vec->Angle = atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		vec->Angle = atan(y / (x * -1.0)) + FM_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		vec->Angle = atan(x / y) + FM_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		vec->Angle = atan((y * -1.0) / x) + FM_3_PI_2;
	}
	else
	{
		vec->Angle = 0.0;
	}

	return vec;
}

VelocityVector* Physics::OffsetToVector(Coordinates* coordinates)
{
	float x = coordinates->X;
	float y = coordinates->Y;

	VelocityVector* vec = new VelocityVector();

	vec->Velocity = sqrt(x * x + y * y);

	if (x == 0.0 && y > 0.0)
	{
		vec->Angle = FM_PI;
	}
	else if (x == 0.0 && y < 0.0)
	{
		vec->Angle = 0.0;
	}
	else if (x > 0.0 && y == 0.0)
	{
		vec->Angle = FM_3_PI_2;
	}
	else if (x < 0.0 && y == 0.0)
	{
		vec->Angle = FM_PI_2;
	}
	else if (x < 0.0 && y < 0.0)
	{
		vec->Angle = atan((x * -1.0) / (y * -1.0));
	}
	else if (x < 0.0 && y > 0.0)
	{
		vec->Angle = atan(y / (x * -1.0)) + FM_PI_2;
	}
	else if (x > 0.0 && y > 0.0)
	{
		vec->Angle = atan(x / y) + FM_PI;
	}
	else if (x > 0.0 && y < 0.0)
	{
		vec->Angle = atan((y * -1.0) / x) + FM_3_PI_2;
	}
	else
	{
		vec->Angle = 0.0;
	}

	return vec;
}

Coordinates* Physics::VectorToOffset(float velocity, float angle)
{
	Coordinates *offset = new Coordinates();

	if (angle == 0.0)
	{
		offset->X = 0.0;
		offset->Y = velocity * -1;
	}
	else if (angle == FM_3_PI_2)
	{
		offset->X = velocity;
		offset->Y = 0.0;
	}
	else if (angle == FM_PI)
	{
		offset->X = 0.0;
		offset->Y = velocity * -1;
	}
	else if (angle == FM_PI_2)
	{
		offset->X = velocity * -1;
		offset->Y = 0.0;
	}
	else if (angle > FM_3_PI_2)
	{
		offset->X = cos(angle - FM_3_PI_2) * velocity;
		offset->Y = sin(angle - FM_3_PI_2) * velocity * -1;
	}
	else if (angle > FM_PI)
	{
		offset->X = sin(angle - FM_PI) * velocity;
		offset->Y = cos(angle - FM_PI) * velocity;
	}
	else if (angle > FM_PI_2)
	{
		offset->X = cos(angle - FM_PI_2) * velocity * -1;
		offset->Y = sin(angle - FM_PI_2) * velocity;
	}
	else if (angle < FM_PI_2 && angle > 0.0)
	{
		offset->X = sin(angle) * velocity * -1;
		offset->Y = cos(angle) * velocity * -1;
	}

	return offset;
}

Coordinates* Physics::VectorToOffset(VelocityVector* vec)
{
	Coordinates *offset = new Coordinates();

	float velocity = vec->Velocity;
	float angle = vec->Angle;

	if (angle == 0.0)
	{
		offset->X = 0.0;
		offset->Y = velocity * -1;
	}
	else if (angle == FM_3_PI_2)
	{
		offset->X = velocity;
		offset->Y = 0.0;
	}
	else if (angle == FM_PI)
	{
		offset->X = 0.0;
		offset->Y = velocity;
	}
	else if (angle == FM_PI_2)
	{
		offset->X = velocity * -1;
		offset->Y = 0.0;
	}
	else if (angle > FM_3_PI_2)
	{
		offset->X = cos(angle - FM_3_PI_2) * velocity;
		offset->Y = sin(angle - FM_3_PI_2) * velocity * -1;
	}
	else if (angle > FM_PI)
	{
		offset->X = sin(angle - FM_PI) * velocity;
		offset->Y = cos(angle - FM_PI) * velocity;
	}
	else if (angle > FM_PI_2)
	{
		offset->X = cos(angle - FM_PI_2) * velocity * -1;
		offset->Y = sin(angle - FM_PI_2) * velocity;
	}
	else if (angle < FM_PI_2 && angle > 0.0)
	{
		offset->X = sin(angle) * velocity * -1;
		offset->Y = cos(angle) * velocity * -1;
	}

	return offset;
}
