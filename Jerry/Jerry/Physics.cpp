#include "Physics.h"

void Physics::ApplyPhysics(list<Entity>* entities, list<WorldBlock>* world, list<WorldEntity>* worldEntities)
{
	ActiveParticles = -1;

	// Bool used to get if the player just landed used to trigger landing audio effect
	bool wasAirBorn = entities->begin()->GetIsAirBorn();
	// Asume that the player is airborn otherwise might be proven in the collisionbehavour
	entities->begin()->SetIsAirBorn(true);

	for (list<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		// Overflow protection so that no infinite loops can occur
		StackOverflowProtection = 0;

		if (ent->GetRemove())
		{
			// Remove the entity from the list if it's set to remove
			list<Entity>::iterator temp = ent;
			--ent;
			entities->erase(temp);
			--Particles;
			continue;
		}

		// Debug value stores the amount of active particles
		if (!ent->GetHit()) ++ActiveParticles;

		if (ent->getType() == PROJECTILE)
		{
			if (ent->GetAge() == MAX_ENTITY_AGE)
			{
				// Remove the entity if it's to old - it's by definition the last one
				entities->pop_back();

				--Particles;

				break;
			}
			else
			{
				// Increase the age by one tick and set his color to accordingly
				ent->IncAge();
				ent->SetColor(al_map_rgb(20, 220 - (200.0 / MAX_ENTITY_AGE * ent->GetAge()), 20));
			}
		}

		// Apply gravity offset to entity
		ApplyGravity(&*ent);
		// Collide the entities if the will
		Collide(&*ent, world, entities, worldEntities);
		// Move the entity
		ent->MoveToOffset();
	}

	// Player just landed
	if (entities->begin()->GetLastColPos() == DY && wasAirBorn && !entities->begin()->GetIsAirBorn())
	{
		ALLEGRO_EVENT e;
		e.type = 555;
		e.user.data1 = PLAYER_WORLD;
		al_emit_user_event(&UserEventSource, &e, 0);
	}
}

void Physics::ApplyGravity(Entity* ent)
{
	if ((ent->getType() == PLAYER && !FLY )|| !ent->GetHit())
	{
		Coordinates* offset = ent->GetOffset();
		// Move set offset including gravity per tic
		ent->SetOffset(offset->X, offset->Y + GRAVITY);
	}
}

void Physics::Collide(Entity* ent, list<WorldBlock>* world, list<Entity>* entities, list<WorldEntity>* worldEntities, bool audioEvents)
{
	Coordinates* entOff = ent->GetOffset();

	if (!ent->GetHit() && (entOff->X != 0.0 || entOff->Y != 0.0))
	{
		Coordinates* entACo = ent->GetACoordinates();
		Coordinates* entBCo = ent->GetBCoordinates();
		double entVel = sqrt(entOff->X * entOff->X + entOff->Y * entOff->Y);
		
		// Stores the delta between the current ent position and where it will collide
		// Used to find the nearest collision
		vector<Coordinates*> collisions;
		// Stores the position where the collision occured
		vector<Coordinates> possitions;
		// Stores the ColPos, on what side the collision happens, evaluated from ent
		vector<CollPos> collisionPosition;
		// Stores the ItemType that is being collided with
		vector<ItemType> collisionType;
		// Stores the actual object that is being collided with
		// Dereffernce happens based on collisionType
		vector<void*> collisionItem;

		// Steps made inbetween the current location and the offset
		double xStep = (entOff->Y == 0 ? (entOff->X < 0.0 ? entVel * -0.1 : entVel * 0.1) : entOff->X) / ceil(entVel);
		double yStep = (entOff->X == 0 ? (entOff->Y < 0.0 ? entVel * -0.1 : entVel * 0.1) : entOff->Y) / ceil(entVel);

		// Check collision with the current ent with all other entities
		for (list<Entity>::iterator jel = entities->begin(); jel != entities->end(); ++jel)
		{
			if (&*ent == &*jel) // Skip self
				continue;
			if (!jel->GetHit()) // Skip all entities that are not hit
				continue;

			Coordinates* jelACo = jel->GetACoordinates();
			Coordinates* jelBCo = jel->GetBCoordinates();

			if (AreInRange(entOff, entACo, entBCo, jelACo, jelBCo))
			{
				// Only run this if ent en jel are within range
				if (!AreColliding(entACo, entBCo, jelACo, jelBCo))
				{
					// Do not run if ent and jel are already colliding
					double minX, minY, maxX, maxY;
					bool minXIsEnt = false, minYIsEnt = false;

					// Get excact collision location, returns NULL if there is no collision 
					Coordinates* colOff = GetCollisionOffset(&*ent, jelACo, jelBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

					if (colOff != NULL)
					{
						// Check if it's an X collision by checking if X is no longer colliding if on step back is made on the X axis
						// Hard to read due to PRECISION (implemented because of DFP)
						if (!(maxX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxY > minY)
						{
							if (minXIsEnt)
							{
								// Left X collision
								possitions.push_back(Coordinates(jelBCo->X, entACo->Y + colOff->Y));
								collisionPosition.push_back(LX);
							}
							else
							{
								// Right X collision
								possitions.push_back(Coordinates(jelACo->X - ent->GetWidth(), entACo->Y + colOff->Y));
								collisionPosition.push_back(RX);
							}
						}
						// Asumpsion is made that the following is an Y collision
						else
						{
							if (minYIsEnt)
							{
								// Upper Y collision
								possitions.push_back(Coordinates(entACo->X + colOff->X, jelBCo->Y));
								collisionPosition.push_back(UY);
							}
							else
							{
								// Lower y collision
								possitions.push_back(Coordinates(entACo->X + colOff->X, jelACo->Y - ent->GetHeight()));
								collisionPosition.push_back(DY);
							}
						}

						// Store the jel that is being collided with
						collisionItem.push_back((void*)&*jel);
						// Store the collision offset
						collisions.push_back(colOff);
						// Store the collision type
						collisionType.push_back(JELLY);
					}
				}
			}
		}

		for (list<WorldBlock>::reverse_iterator wor = world->rbegin(); wor != world->rend(); ++wor)
		{
			if (AreInRange(entOff, entACo, entBCo, wor->GetA(), wor->GetB()))
			{
				Coordinates* worACo = wor->GetA();
				Coordinates* worBCo = wor->GetB();

				bool minXIsEnt = false, minYIsEnt = false;
				double minX, minY, maxX, maxY;

				Coordinates* colOff = GetCollisionOffset(&*ent, worACo, worBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

				if (colOff != NULL)
				{
					if (!(maxX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxY > minY)
					{
						if (minXIsEnt)
						{
							possitions.push_back(Coordinates(worBCo->X, entACo->Y + colOff->Y));
							collisionPosition.push_back(LX);
						}
						else
						{
							possitions.push_back(Coordinates(worACo->X - ent->GetWidth(), entACo->Y + colOff->Y));
							collisionPosition.push_back(RX);
						}
					}
					else
					{
						if (minYIsEnt)
						{
							possitions.push_back(Coordinates(entACo->X + colOff->X, worBCo->Y));
							collisionPosition.push_back(UY);
						}
						else
						{
							possitions.push_back(Coordinates(entACo->X + colOff->X, worACo->Y - ent->GetHeight()));
							collisionPosition.push_back(DY);
						}
					}

					collisionItem.push_back((void*)&*wor);
					collisions.push_back(colOff);
					collisionType.push_back(wor->GetType());
				}
			}
		}

		for (list<WorldEntity>::iterator worE = worldEntities->begin(); worE != worldEntities->end(); ++worE)
		{
			// Don't run if ent is a projectile and worE a coin
			if (ent->getType() == PROJECTILE && worE->GetType() == COIN)
				continue;

			if (AreInRange(entOff, entACo, entBCo, worE->GetA(), worE->GetB()))
			{
				Coordinates* worACo = worE->GetA();
				Coordinates* worBCo = worE->GetB();

				bool minXIsEnt = false, minYIsEnt = false;
				double minX, minY, maxX, maxY;

				Coordinates* colOff = GetCollisionOffset(&*ent, worACo, worBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

				if (colOff != NULL)
				{
					if (!(maxX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxY > minY)
					{
						if (minXIsEnt)
						{
							possitions.push_back(Coordinates(worBCo->X, entACo->Y + colOff->Y));
							collisionPosition.push_back(LX);
						}
						else
						{
							possitions.push_back(Coordinates(worACo->X - ent->GetWidth(), entACo->Y + colOff->Y));
							collisionPosition.push_back(RX);
						}
					}
					else
					{
						if (minYIsEnt)
						{
							possitions.push_back(Coordinates(entACo->X + colOff->X, worBCo->Y));
							collisionPosition.push_back(UY);
						}
						else
						{
							possitions.push_back(Coordinates(entACo->X + colOff->X, worACo->Y - ent->GetHeight()));
							collisionPosition.push_back(DY);
						}
					}

					collisionItem.push_back((void*)&*worE);
					collisions.push_back(colOff);
					collisionType.push_back(worE->GetType());
				}
			}
		}

		if (collisions.size())
		{
			// Closest collision index
			int closestX = -1;
			int closestY = -1;

			for (int i = 0; i < collisions.size(); ++i)
			{
				if (collisionPosition[i] == LX || collisionPosition[i] == RX)
				{
					// Find the closest collision on the X axis based on the total distance of the collision (Pythagorean theorem)
					if (closestX == -1 ? true : (sqrt(pow(collisions[i]->X, 2) + pow(collisions[i]->Y, 2)) < sqrt(pow(collisions[closestX]->X, 2) + pow(collisions[closestX]->Y, 2))))
					{
						closestX = i;
					}
				}
				else if (collisionPosition[i] == UY || collisionPosition[i] == DY)
				{
					// Find the closest collision on the Y axis based on the total distance of the collision (Pythagorean theorem)
					if (closestY == -1 ? true : (sqrt(pow(collisions[i]->X, 2) + pow(collisions[i]->Y, 2)) < sqrt(pow(collisions[closestY]->X, 2) + pow(collisions[closestY]->Y, 2))))
					{
						closestY = i;
					}
				}
			}

			// Check if it should be a flat collision due to several entities or world items at same height or width
			if (closestX != -1 && closestY != -1)
			{
				bool actualY = true;
				bool actualX = true;

				for (int i = 0; i < collisions.size(); ++i)
				{
					if (closestX != i)
					{
						// If any of the possible collisions is not on the same offset as the X collision no flat collision on X
						if (collisionPosition[closestX] == LX)
						{
							if (!(((collisionType[closestX] == WORLD || collisionType[closestX] == BADWORLD || collisionType[closestX] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestX])->GetB()->X : ((Entity*)collisionItem[closestX])->GetBCoordinates()->X) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetB()->X : ((Entity*)collisionItem[i])->GetBCoordinates()->X)))
							{
								actualX = false;
							}
						}
						else if (collisionPosition[closestX] == RX)
						{
							if (!(((collisionType[closestX] == WORLD || collisionType[closestX] == BADWORLD || collisionType[closestX] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestX])->GetA()->X : ((Entity*)collisionItem[closestX])->GetACoordinates()->X) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetA()->X : ((Entity*)collisionItem[i])->GetACoordinates()->X)))
							{
								actualX = false;
							}
						}
					}

					if (closestY != i)
					{
						// If any of the possible collisions is not on the same offset as the Y collision no flat collision on Y
						if (collisionPosition[closestY] == UY)
						{
							if (!(((collisionType[closestY] == WORLD || collisionType[closestY] == BADWORLD || collisionType[closestY] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestY])->GetB()->Y : ((Entity*)collisionItem[closestY])->GetBCoordinates()->Y) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetB()->Y : ((Entity*)collisionItem[i])->GetBCoordinates()->Y)))
							{
								actualY = false;
							}
						}
						else if (collisionPosition[closestY] == DY)
						{
							if (!(((collisionType[closestY] == WORLD || collisionType[closestY] == BADWORLD || collisionType[closestY] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestY])->GetA()->Y : ((Entity*)collisionItem[closestY])->GetACoordinates()->Y) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetA()->Y : ((Entity*)collisionItem[i])->GetACoordinates()->Y)))
							{
								actualY = false;
							}
						}
					}
				}

				// Put in here during dev I think this cannot occur, but haven't proven it yet (no time) so it stay here to make sure no unexpected crashes occur
				if (actualX && actualY)
				{
					if (ent->GetLastColPos() == LX || ent->GetLastColPos() == RX)
					{
						closestY = -1;
					}
					else
					{
						closestX = -1;
					}
				}
				else if (actualX)
				{
					// Actual X collision thus no Y collision
					closestY = -1;
				}
				else if (actualY)
				{
					// Actual Y collision thus no X collision
					closestX = -1;
				}
			}

			if (closestX != -1 && closestY != -1)
			{
				// Inner corner collision
				XYCollisionBehaviour(closestX, closestY, &*ent, &possitions, &collisionPosition, &collisionType, &collisionItem, audioEvents);
			}
			else if (closestY != -1)
			{
				YCollisionBehaviour(closestY, &*ent, &possitions, &collisionPosition, &collisionType, &collisionItem, audioEvents);
			}
			else if (closestX != -1)
			{
				XCollisionBehaviour(closestX, &*ent, &possitions, &collisionPosition, &collisionType, &collisionItem, audioEvents);
			}

			// Checks whenever a collision will occur within the current tick
			// Possible of there are two objects close to each other and and entity is going fast enough to hit both in one tic
			if (WillCollide(&*ent, world, entities, worldEntities))
			{
				// To make sure no overflow occurs due to an infinite loop
				if (StackOverflowProtection < MAX_COLLISION_RECURSION)
				{
					++StackOverflowProtection;
					Collide(ent, world, entities, worldEntities, audioEvents);
				}
				else
				{
					// If too many recursions have happend break out of the recursion by setting the offset to 0.0
					ent->SetOffset(0.0, 0.0);
				}
			}
		}
	}
}

bool Physics::WillCollide(Entity* entity, list<WorldBlock>* world, list<Entity>* entities, list<WorldEntity>* worldEntities)
{
	if (!entity->GetHit() && !entity->GetRemove())
	{
		Coordinates* entACo = entity->GetACoordinates();
		Coordinates* entBCo = entity->GetBCoordinates();
		Coordinates* entOff = entity->GetOffset();

		double entVel = sqrt(entOff->X * entOff->X + entOff->Y * entOff->Y);

		double xStep = (entOff->Y == 0 ? (entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0))) : entOff->X) / ceil(entVel);
		double yStep = (entOff->X == 0 ? (entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0))) : entOff->Y) / ceil(entVel);

		for (list<WorldEntity>::iterator worE = worldEntities->begin(); worE != worldEntities->end(); ++worE)
		{
			if (entity->getType() == PROJECTILE && worE->GetType() == COIN)
				continue;

			if (worE->GetRemove())
			{
				worE = worldEntities->erase(worE);
				++Score;

				if (worE == worldEntities->end())
					break;
				continue;
			}

			if (AreInRange(entOff, entACo, entBCo, worE->GetA(), worE->GetB()))
			{
				bool minXIsEnt = false, minYIsEnt = false;
				double minX, minY, maxX, maxY;

				Coordinates* colOff = GetCollisionOffset(entity, worE->GetA(), worE->GetB(), xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

				if (colOff != NULL)
				{
					// If a collision occurs return true
					delete colOff;
					return true;
				}
			}
		}

		//TODO: Add the isHit conditions and shit
		for (list<Entity>::iterator jel = entities->begin(); jel != entities->end(); ++jel)
		{
			if (entity == &*jel)
				continue;
			if (!jel->GetHit())
				continue;

			Coordinates* jelACo = jel->GetACoordinates();
			Coordinates* jelBCo = jel->GetBCoordinates();

			if (AreInRange(entOff, entACo, entBCo, jelACo, jelBCo))
			{
				if (!AreColliding(entACo, entBCo, jelACo, jelBCo))
				{
					bool minXIsEnt = false, minYIsEnt = false;
					double minX, minY, maxX, maxY;

					Coordinates* colOff = GetCollisionOffset(entity, jelACo, jelBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

					if (colOff != NULL)
					{
						// If a collision occurs return true
						delete colOff;
						return true;
					}
				}
			}
		}

		for (list<WorldBlock>::reverse_iterator wor = world->rbegin(); wor != world->rend(); ++wor)
		{
			if (AreInRange(entOff, entACo, entBCo, wor->GetA(), wor->GetB()))
			{
				bool minXIsEnt = false, minYIsEnt = false;
				double minX, minY, maxX, maxY;

				Coordinates* colOff = GetCollisionOffset(entity, wor->GetA(), wor->GetB(), xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

				if (colOff != NULL)
				{
					// If a collision occurs return true
					delete colOff;
					return true;
				}
			}
		}
	}

	return false;
}

// Collision based on Seperate axis collision theorem 
// Found the theory at http://gamedevelopment.tutsplus.com/tutorials/collision-detection-using-the-separating-axis-theorem--gamedev-169
// Only used pictures as a quick refference, did all the thinking and testing myself
Coordinates* Physics::GetCollisionOffset(Entity* entity, Coordinates* xA, Coordinates* xB, double xStep, double yStep, double* minX, double* minY, double* maxX, double* maxY, bool* minXIsEnt, bool* minYIsEnt)
{
	Coordinates* entACo = entity->GetACoordinates();
	Coordinates* entBCo = entity->GetBCoordinates();
	Coordinates* entOff = entity->GetOffset();

	// Start at the position of the player and add the steps accordingly
	for (double x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X - PRECISION : x <= entOff->X + PRECISION) && (yStep < 0.0 ? y >= entOff->Y - PRECISION : y <= entOff->Y + PRECISION); x += xStep, y += yStep)
	{
		// X with Offset					
		if (xA->X < entACo->X + x)
		{
			*minX = entACo->X + x;
			// Variable used to see what kind of collision is made: UY, DY, LX or RX
			*minXIsEnt = true;

			*maxX = xB->X;
		}
		else
		{
			*minX = xA->X;
			*minXIsEnt = false;

			*maxX = entBCo->X + x;
		}

		//Y	with offset						
		if (xA->Y < entACo->Y + y)
		{
			*minY = entACo->Y + y;
			*minYIsEnt = true;

			*maxY = xB->Y;
		}
		else
		{
			*minY = xA->Y;
			*minYIsEnt = false;

			*maxY = entBCo->Y + y;
		}

		if (*maxX > *minX && *maxY > *minY)
		{
			// Retrun the offset where the collision happens
			return new Coordinates(x, y);
		}
	}

	return NULL;
}

// To see if two objects are already colliding
bool Physics::AreColliding(Coordinates* aa, Coordinates* ab, Coordinates* ba, Coordinates* bb)
{
	double minY;
	double maxY;
	double minX;
	double maxX;

	// X with Offset
	if (ba->X < aa->X)
	{
		minX = aa->X;

		maxX = bb->X;
	}
	else
	{
		minX = ba->X;

		maxX = ab->X;
	}

	//Y	with offset
	if (ba->Y < aa->Y)
	{
		minY = aa->Y;

		maxY = bb->Y;
	}
	else
	{
		minY = ba->Y;

		maxY = ab->Y;
	}

	if (maxX > minX && maxY > minY)
	{
		return true;
	}

	return false;
}

// Fucntion to check of the entity is in range and thus might collide 
bool Physics::AreInRange(Coordinates* entOff, Coordinates* entACo, Coordinates* entBCo, Coordinates* xA, Coordinates* xB)
{
	double minY;
	double maxY;
	double minX;
	double maxX;

	// X with Offset
	if (xA->X < entACo->X)
	{
		minX = entACo->X + entOff->X;

		maxX = xB->X;
	}
	else
	{
		minX = xA->X;

		maxX = entBCo->X + entOff->X;
	}

	//Y	with offset
	if (xA->Y < entACo->Y)
	{
		minY = entACo->Y + entOff->Y;

		maxY = xB->Y;
	}
	else
	{
		minY = xA->Y;

		maxY = entBCo->Y + entOff->Y;
	}

	if (maxX > minX && maxY > minY)
	{
		return true;
	}

	return false;
}

// Calculates the angle based on the offset, returns in radians
double Physics::OffsetToAngle(double x, double y)
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

// Self documenting function name?
VelocityVector* Physics::OffsetToVector(double x, double y)
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

// Self documenting function name?
VelocityVector* Physics::OffsetToVector(Coordinates* coordinates)
{
	double x = coordinates->X;
	double y = coordinates->Y;

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

// Self documenting function name?
Coordinates* Physics::VectorToOffset(double velocity, double angle)
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

// Self documenting function name?
Coordinates* Physics::VectorToOffset(VelocityVector* vec)
{
	Coordinates *offset = new Coordinates();

	double velocity = vec->Velocity;
	double angle = vec->Angle;

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

// Behaviour for an X collision
void Physics::XCollisionBehaviour(int xIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem, bool audioEvents)
{
	Coordinates* entOff = ent->GetOffset();
	Coordinates* entACo = ent->GetACoordinates();
	Coordinates* entBCo = ent->GetBCoordinates();

	if (ent->getType() == PROJECTILE)
	{
		if ((*collisionType)[xIndex] == JELLYWORLD)
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			ent->SetOffset(0.0, 0.0);
			ent->SetHit(true);

			WorldBlock* wor = ((WorldBlock*)(*collisionItem)[xIndex]);

			if ((*collisionPosition)[xIndex] == RX)
			{
				if (wor->GetHeight() < ent->GetHeight() * 4)
				{
					ent->SetHeight(wor->GetHeight());
					ent->MoveToOffset(0.0, wor->GetA()->Y - entACo->Y);
				}
				else
				{
					ent->SetHeight(ent->GetHeight() * 4);
					ent->MoveToOffset(0.0, ent->GetHeight() / 2 * -1);

					if (entACo->Y < wor->GetA()->Y)
					{
						ent->MoveToOffset(0.0, wor->GetA()->Y - entACo->Y);
					}
					else if (entBCo->Y > wor->GetB()->Y)
					{
						ent->MoveToOffset(0.0, wor->GetB()->Y - entBCo->Y);
					}
				}

				if (wor->GetWidth() < ent->GetWidth() / 2)
				{
					ent->SetWidth(wor->GetWidth());
					ent->MoveToOffset(wor->GetA()->X - entACo->X, 0.0);
				}
				else
				{
					ent->SetWidth(ent->GetWidth() / 2);
					ent->MoveToOffset(ent->GetWidth() * 2, 0.0);
				}
			}
			else
			{
				if (wor->GetHeight() < ent->GetHeight() * 4)
				{
					ent->SetHeight(wor->GetHeight());
					ent->MoveToOffset(0.0, wor->GetA()->Y - entACo->Y);
				}
				else
				{
					ent->SetHeight(ent->GetHeight() * 4);
					ent->MoveToOffset(0.0, ent->GetHeight() / 2 * -1);

					if (entACo->Y < wor->GetA()->Y)
					{
						ent->MoveToOffset(0.0, wor->GetA()->Y - entACo->Y);
					}
					else if (entBCo->Y > wor->GetB()->Y)
					{
						ent->MoveToOffset(0.0, wor->GetB()->Y - entBCo->Y);
					}
				}

				if (wor->GetWidth() < ent->GetWidth() / 2)
				{
					ent->SetWidth(wor->GetWidth());
					ent->MoveToOffset(wor->GetA()->X - entACo->X, 0.0);
				}
				else
				{
					ent->SetWidth(ent->GetWidth() / 2);
					ent->MoveToOffset(ent->GetWidth() * -1, 0.0);
				}
			}

			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_JELLYWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[xIndex] == JELLY)
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			ent->SetOffset(entOff->X * -1 * PROJECTILE_BOUNCINESS, entOff->Y * PROJECTILE_BOUNCINESS);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_JELLY;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[xIndex] == BADWORLD || (*collisionType)[xIndex] == WORLD)
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			ent->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_NOTJELLYWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
	}
	else // Type is player
	{

		if ((*collisionType)[xIndex] == BADWORLD)
		{
			ent->SetOffset(0.0, 0.0);
			ent->SetCoordinates(Spawn.X, Spawn.Y);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_BADWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[xIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[xIndex])->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_COIN;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[xIndex] == WORLD || (*collisionType)[xIndex] == JELLYWORLD || ent->GetIsCrouching())
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			//ent->SetOffset(0.0, entOff->Y < FRICTION_STOP && entOff->Y > FRICTION_STOP * -1 ? 0.0 : entOff->Y * FRICTION);
			ent->SetOffset(0.0, entOff->Y);
			ent->SetLastColPos((*collisionPosition)[xIndex]);
			ent->SetLastImpactType(WORLD);
		}
		else if ((*collisionType)[xIndex] == JELLY)
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			ent->SetOffset(PLAYER_BOUNCE_OFFSET / ((*collisionPosition)[xIndex] == RX ? -PLAYER_SIDE_SIDE_BOUNCE : PLAYER_SIDE_SIDE_BOUNCE), PLAYER_BOUNCE_OFFSET / -PLAYER_SIDE_UP_BOUNCE);
			ent->SetLastColPos((*collisionPosition)[xIndex]);
			ent->SetLastImpactType(JELLY);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_JELLY;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
	}
}

// Behaviour for an Y collision
void Physics::YCollisionBehaviour(int yIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem, bool audioEvents)
{
	Coordinates* entOff = ent->GetOffset();
	Coordinates* entACo = ent->GetACoordinates();
	Coordinates* entBCo = ent->GetBCoordinates();

	if (ent->getType() == PROJECTILE)
	{
		if ((*collisionType)[yIndex] == JELLYWORLD)
		{
			// Calculate the offset at point of impact
			entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
			ent->SetCoordinates((*possitions)[yIndex].X, (*possitions)[yIndex].Y);

			ent->SetOffset(0.0, 0.0);
			ent->SetHit(true);

			WorldBlock* wor = ((WorldBlock*)(*collisionItem)[yIndex]);

			if ((*collisionPosition)[yIndex] == DY)
			{
				if (wor->GetWidth() < ent->GetWidth() * 4)
				{
					ent->SetWidth(wor->GetWidth());
					ent->MoveToOffset(wor->GetA()->X - entACo->X, 0.0);
				}
				else
				{
					ent->SetWidth(ent->GetWidth() * 4);
					ent->MoveToOffset(ent->GetWidth() / 2 * -1, 0.0);

					if (entACo->X < wor->GetA()->X)
					{
						ent->MoveToOffset(wor->GetA()->X - entACo->X, 0.0);
					}
					else if (entBCo->X > wor->GetB()->X)
					{
						ent->MoveToOffset(wor->GetB()->X - entBCo->X, 0.0);
					}
				}

				if (wor->GetHeight() < ent->GetHeight() / 2)
				{
					ent->SetHeight(wor->GetHeight());
					ent->MoveToOffset(0.0, wor->GetA()->Y - entACo->Y);
				}
				else
				{
					ent->SetHeight(ent->GetHeight() / 2);
					ent->MoveToOffset(0.0, ent->GetHeight() * 2);
				}
			}
			else
			{
				if (wor->GetWidth() < ent->GetWidth() * 4)
				{
					ent->SetWidth(wor->GetWidth());
					ent->MoveToOffset(wor->GetA()->X - entACo->X, 0.0);
				}
				else
				{
					ent->SetWidth(ent->GetWidth() * 4);
					ent->MoveToOffset(ent->GetWidth() / 2 * -1, 0.0);

					if (entACo->X < wor->GetA()->X)
					{
						ent->MoveToOffset(wor->GetA()->X - entACo->X, 0.0);
					}
					else if (entBCo->X > wor->GetB()->X)
					{
						ent->MoveToOffset(wor->GetB()->X - entBCo->X, 0.0);
					}
				}

				if (wor->GetHeight() < ent->GetHeight() / 2)
				{
					ent->SetHeight(wor->GetHeight());
					ent->MoveToOffset(0.0, wor->GetA()->Y - entACo->Y);
				}
				else
				{
					ent->SetHeight(ent->GetHeight() / 2);
					ent->MoveToOffset(0.0, ent->GetHeight() * -1);
				}
			}
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_JELLYWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[yIndex] == JELLY)
		{
			// Calculate the offset at point of impact
			entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
			ent->SetCoordinates((*possitions)[yIndex].X, (*possitions)[yIndex].Y);

			ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS, entOff->Y * -1 * PROJECTILE_BOUNCINESS);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_JELLY;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[yIndex] == BADWORLD || (*collisionType)[yIndex] == WORLD)
		{
			ent->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_NOTJELLYWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
	}
	else // Type is player
	{

		if ((*collisionType)[yIndex] == BADWORLD)
		{
			// Calculate the offset at point of impact
			entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
			ent->SetCoordinates((*possitions)[yIndex].X, (*possitions)[yIndex].Y);

			ent->SetOffset(0.0, 0.0);
			ent->SetCoordinates(Spawn.X, Spawn.Y);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_BADWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[yIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[yIndex])->SetRemove(true);

			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_COIN;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[yIndex] == WORLD || (*collisionType)[yIndex] == JELLYWORLD || ent->GetIsCrouching())
		{
			// Calculate the offset at point of impact
			entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
			ent->SetCoordinates((*possitions)[yIndex].X, (*possitions)[yIndex].Y);

			ent->SetOffset(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0);

			if ((*collisionPosition)[yIndex] == DY)
			{
				ent->SetPreviousImpactHeight((*possitions)[yIndex].Y);
				ent->SetIsAirBorn(false);
			}
			else
			{
				ent->SetIsAirBorn(true);
			}

			ent->SetLastColPos((*collisionPosition)[yIndex]);
			ent->SetLastImpactType(WORLD);
		}
		else if ((*collisionType)[yIndex] == JELLY)
		{
			// Calculate the offset at point of impact
			entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
			ent->SetCoordinates((*possitions)[yIndex].X, (*possitions)[yIndex].Y);

			if ((*collisionPosition)[yIndex] == UY)
			{
				ent->SetOffset(entOff->X, entOff->Y * -1);
			}
			else
			{
				if (entOff->Y < PLAYER_BOUNCE_OFFSET)
				{
					ent->SetOffset(entOff->X, PLAYER_BOUNCE_OFFSET * -1);
				}
				else
				{
					ent->SetOffset(entOff->X, entOff->Y * -1);
				}
				ent->SetPreviousImpactHeight((*possitions)[yIndex].Y);
				ent->SetLastColPos((*collisionPosition)[yIndex]);
				ent->SetLastImpactType(JELLY);
			}
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_JELLY;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
	}
}

// Behaviour for an XY collision (corner)
void Physics::XYCollisionBehaviour(int xIndex, int yIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem, bool audioEvents)
{
	Coordinates* entOff = ent->GetOffset();
	Coordinates* entACo = ent->GetACoordinates();
	Coordinates* entBCo = ent->GetBCoordinates();

	entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
	ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[yIndex].Y);

	if (ent->getType() == PROJECTILE)
	{
		if ((*collisionType)[xIndex] == BADWORLD || (*collisionType)[yIndex] == BADWORLD || (*collisionType)[xIndex] == WORLD || (*collisionType)[yIndex] == WORLD)
		{
			ent->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_NOTJELLYWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[yIndex] == JELLYWORLD && (*collisionType)[xIndex] == JELLYWORLD)
		{
			YCollisionBehaviour(yIndex, ent, possitions, collisionPosition, collisionType, collisionItem, audioEvents);
		}
		else if ((*collisionType)[xIndex] == JELLYWORLD)
		{
			XCollisionBehaviour(xIndex, ent, possitions, collisionPosition, collisionType, collisionItem, audioEvents);
		}
		else if ((*collisionType)[yIndex] == JELLYWORLD)
		{
			YCollisionBehaviour(yIndex, ent, possitions, collisionPosition, collisionType, collisionItem, audioEvents);
		}
		else if ((*collisionType)[yIndex] == JELLY || (*collisionType)[xIndex] == JELLY)
		{
			// TODO: Wat is deze g, was met jou
			ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS * -1, entOff->Y * PROJECTILE_BOUNCINESS * -1);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = JELLY_JELLY;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
	}
	else if (ent->getType() == PLAYER)
	{
		if ((*collisionType)[xIndex] == BADWORLD || (*collisionType)[yIndex] == BADWORLD)
		{
			ent->SetOffset(0.0, 0.0);
			ent->SetCoordinates(Spawn.X, Spawn.Y);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_BADWORLD;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[xIndex] == COIN && (*collisionType)[yIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[xIndex])->SetRemove(true);
			((WorldEntity*)(*collisionItem)[yIndex])->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_COIN;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
		else if ((*collisionType)[xIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[xIndex])->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_COIN;
				al_emit_user_event(&UserEventSource, &e, 0);
			}

			YCollisionBehaviour(yIndex, ent, possitions, collisionPosition, collisionType, collisionItem, audioEvents);
		}
		else if ((*collisionType)[yIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[yIndex])->SetRemove(true);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_COIN;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
			XCollisionBehaviour(xIndex, ent, possitions, collisionPosition, collisionType, collisionItem, audioEvents);
		}
		else if ((((*collisionType)[xIndex] == WORLD || (*collisionType)[xIndex] == JELLYWORLD) && ((*collisionType)[yIndex] == WORLD || (*collisionType)[yIndex] == JELLYWORLD)) || ent->GetIsCrouching())
		{
			ent->SetOffset(0.0, 0.0);
			ent->SetLastColPos((*collisionPosition)[xIndex]);
			ent->SetIsAirBorn(false);
		}
		else if ((*collisionType)[yIndex] == WORLD || (*collisionType)[yIndex] == JELLYWORLD)
		{
			ent->SetOffset(PLAYER_BOUNCE_OFFSET / ((*collisionPosition)[xIndex] == RX ? -PLAYER_SIDE_SIDE_BOUNCE : PLAYER_SIDE_SIDE_BOUNCE), PLAYER_BOUNCE_OFFSET / -PLAYER_SIDE_UP_BOUNCE);
			ent->SetLastColPos((*collisionPosition)[xIndex]);
			ent->SetLastImpactType(JELLY);
		}
		else if ((*collisionType)[xIndex] == WORLD || (*collisionType)[xIndex] == JELLYWORLD)
		{
			ent->SetLastColPos((*collisionPosition)[xIndex]);
			ent->SetOffset(0.0, entOff->Y * PROJECTILE_BOUNCINESS * -1);
		}
		else if ((*collisionType)[yIndex] == JELLY || (*collisionType)[xIndex] == JELLY)
		{
			if ((*collisionPosition)[yIndex] == UY)
			{
				ent->SetOffset(entOff->X, entOff->Y * -1);
			}
			else
			{
				if (entOff->Y < PLAYER_BOUNCE_OFFSET)
				{
					ent->SetOffset(entOff->X, PLAYER_BOUNCE_OFFSET * -1);
				}
				else
				{
					ent->SetOffset(entOff->X, entOff->Y * -1);
				}
				ent->SetPreviousImpactHeight((*possitions)[yIndex].Y);
				ent->SetLastImpactType(JELLY);
			}

			ent->SetLastColPos((*collisionPosition)[xIndex]);
			if (audioEvents)
			{
				ALLEGRO_EVENT e;
				e.type = 555;
				e.user.data1 = PLAYER_JELLY;
				al_emit_user_event(&UserEventSource, &e, 0);
			}
		}
	}
}
