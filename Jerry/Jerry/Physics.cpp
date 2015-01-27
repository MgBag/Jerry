#include "Physics.h"

// TODO: Memory leaks mang
// TODO: Check logics mang, flows and flaws
// TODO: Stop deleteing entOff
// TODO: Meak air collision, might be easy?

void Physics::ApplyPhysics(list<Entity>* entities, list<WorldBlock>* world, list<WorldEntity>* worldEntities)
{
	ActiveParticles = -1;

	bool wasAirBorn = entities->begin()->GetIsAirBorn();
	entities->begin()->SetIsAirBorn(true);

	for (list<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
		StackOverflowProtection = 0;

		// TODO : Move this to into the the collision
		if (ent->GetRemove())
		{
			list<Entity>::iterator temp = ent;
			--ent;
			entities->erase(temp);
			--Particles;
			continue;
		}

		if (!ent->GetHit()) ++ActiveParticles;

		if (ent->getType() == PROJECTILE)
		{
			if (ent->GetAge() == MAX_ENTITY_AGE)
			{
				entities->pop_back();

				--Particles;

				break;
			}
			else
			{
				ent->IncAge();
				ent->SetColor(al_map_rgb(20, 220 - (200.0 / MAX_ENTITY_AGE * ent->GetAge()), 20));
			}
		}

		ApplyGravity(&(*ent));
		Collide(ent, world, entities, worldEntities);
		ent->MoveToOffset();
	}

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
	if (ent->getType() == PLAYER || !ent->GetHit())
	{
		Coordinates* offset = ent->GetOffset();

		ent->SetOffset(offset->X, offset->Y + GRAVITY);
	}
}

void Physics::Collide(list<Entity>::iterator ent, list<WorldBlock>* world, list<Entity>* entities, list<WorldEntity>* worldEntities)
{
	Coordinates* entOff = ent->GetOffset();

	if (!ent->GetHit() && (entOff->X != 0.0 || entOff->Y != 0.0))
	{
		Coordinates* entACo = ent->GetACoordinates();
		Coordinates* entBCo = ent->GetBCoordinates();
		double entVel = sqrt(entOff->X * entOff->X + entOff->Y * entOff->Y);
		vector<Coordinates*> collisions;
		vector<Coordinates> possitions;
		vector<CollPos> collisionPosition;
		vector<ItemType> collisionType;
		vector<void*> collisionItem;


		// To make sure that a step isn't 1.0 if the offset is equal to teh velocity
		double xStep = (entOff->Y == 0 ? (entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0))) : entOff->X) / ceil(entVel);
		double yStep = (entOff->X == 0 ? (entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0))) : entOff->Y) / ceil(entVel);

		for (list<Entity>::iterator jel = entities->begin(); jel != entities->end(); ++jel)
		{
			if (&*ent == &*jel)
				continue;
			if (!jel->GetHit())
				continue;

			Coordinates* jelACo = jel->GetACoordinates();
			Coordinates* jelBCo = jel->GetBCoordinates();

			if (AreInRange(entOff, entACo, entBCo, jelACo, jelBCo))
			{
				if (!AreColliding(entACo, entBCo, jelACo, jelBCo))
				{
					double minX, minY, maxX, maxY;
					bool minXIsEnt = false, minYIsEnt = false;

					Coordinates* colOff = GetCollisionOffset(&*ent, jelACo, jelBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

					if (colOff != NULL)
					{
						if (!(maxX - (minXIsEnt ? 0.0 : (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION)) > minX - (minXIsEnt ? (xStep < 0.0 ? xStep - PRECISION : xStep + PRECISION) : 0.0)) && maxY > minY)
						{
							if (minXIsEnt)
							{
								possitions.push_back(Coordinates(jelBCo->X, entACo->Y + colOff->Y));
								collisionPosition.push_back(LX);
							}
							else
							{
								possitions.push_back(Coordinates(jelACo->X - ent->GetWidth(), entACo->Y + colOff->Y));
								collisionPosition.push_back(RX);
							}

						}
						else /*if (maxX > minX && !(maxY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))*/
						{
							if (minYIsEnt)
							{
								possitions.push_back(Coordinates(entACo->X + colOff->X, jelBCo->Y));
								collisionPosition.push_back(UY);
							}
							else
							{
								possitions.push_back(Coordinates(entACo->X + colOff->X, jelACo->Y - ent->GetHeight()));
								collisionPosition.push_back(DY);
							}
						}

						collisionItem.push_back((void*)&*jel);
						collisions.push_back(colOff);
						collisionType.push_back(JELLY);
					}
				}
			}
		}

		for (list<WorldBlock>::reverse_iterator wor = world->rbegin(); wor != world->rend(); ++wor)
		{
			// TODO: Add might collide
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
					else /*if (maxX > minX && !(maxY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))*/
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
			if (ent->getType() == PROJECTILE && worE->GetType() == COIN)
				continue;

			// TODO: Add might collide
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
					else /*if (maxX > minX && !(maxY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))*/
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
			int closestX = -1;
			int closestY = -1;

			for (int i = 0; i < collisions.size(); ++i)
			{
				if (collisionPosition[i] == LX || collisionPosition[i] == RX)
				{
					if (closestX == -1 ? true : (sqrt(pow(collisions[i]->X, 2) + pow(collisions[i]->Y, 2)) < sqrt(pow(collisions[closestX]->X, 2) + pow(collisions[closestX]->Y, 2))))
					{
						closestX = i;
					}
				}
				else if (collisionPosition[i] == UY || collisionPosition[i] == DY)
				{
					if (closestY == -1 ? true : (sqrt(pow(collisions[i]->X, 2) + pow(collisions[i]->Y, 2)) < sqrt(pow(collisions[closestY]->X, 2) + pow(collisions[closestY]->Y, 2))))
					{
						closestY = i;
					}
				}
			}

			// Check if it should be a flat collision due to several entities or world items at same height or width
			if (closestX != -1 && closestY != -1)
			{
				//(collisionType[closestX] == WORLD || collisionType[closestX] == BADWORLD || collisionType[closestX] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestX])->SetColor(al_map_rgb(220, 220, 20)) : ((Entity*)collisionItem[closestX])->SetColor(al_map_rgb(220, 220, 20));
				//(collisionType[closestY] == WORLD || collisionType[closestY] == BADWORLD || collisionType[closestY] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestY])->SetColor(al_map_rgb(220, 220, 20)) : ((Entity*)collisionItem[closestY])->SetColor(al_map_rgb(220, 220, 20));
				bool actualY = false;
				bool actualX = false;

				int c1 = count(collisionPosition.begin(), collisionPosition.end(), collisionPosition[closestX]);
				int c2 = count(collisionPosition.begin(), collisionPosition.end(), collisionPosition[closestY]);
				
				if (!(c1 > 1 && c2 > 1))
				for (int i = 0; i < collisions.size(); ++i)
				{
					if (closestX != i && collisionPosition[i] != collisionPosition[closestX])
					{
						if (collisionPosition[closestX] == LX)
						{
							if (((collisionType[closestX] == WORLD || collisionType[closestX] == BADWORLD || collisionType[closestX] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestX])->GetB()->X : ((Entity*)collisionItem[closestX])->GetBCoordinates()->X) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetB()->X : ((Entity*)collisionItem[i])->GetBCoordinates()->X))
							{
								actualX = true;
							}
						}
						else if (collisionPosition[closestX] == RX)
						{
							if (((collisionType[closestX] == WORLD || collisionType[closestX] == BADWORLD || collisionType[closestX] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestX])->GetA()->X : ((Entity*)collisionItem[closestX])->GetACoordinates()->X) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetA()->X : ((Entity*)collisionItem[i])->GetACoordinates()->X))
							{
								actualX = true;
							}
						}
					}

					if (closestY != i && collisionPosition[i] != collisionPosition[closestY])
					{
						if (collisionPosition[closestY] == UY)
						{
							if (((collisionType[closestY] == WORLD || collisionType[closestY] == BADWORLD || collisionType[closestY] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestY])->GetB()->Y : ((Entity*)collisionItem[closestY])->GetBCoordinates()->Y) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetB()->Y : ((Entity*)collisionItem[i])->GetBCoordinates()->Y))
							{
								actualY = true;
							}
						}
						else if (collisionPosition[closestY] == DY)
						{
							if (((collisionType[closestY] == WORLD || collisionType[closestY] == BADWORLD || collisionType[closestY] == JELLYWORLD) ? ((WorldBlock*)collisionItem[closestY])->GetA()->Y : ((Entity*)collisionItem[closestY])->GetACoordinates()->Y) == ((collisionType[i] == WORLD || collisionType[i] == BADWORLD || collisionType[i] == JELLYWORLD) ? ((WorldBlock*)collisionItem[i])->GetA()->Y : ((Entity*)collisionItem[i])->GetACoordinates()->Y))
							{
								actualY = true;
							}
						}
					}
				}

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
					closestY = -1;
				}
				else if (actualY)
				{
					closestX = -1;
				}
			}

			if (closestX != -1 && closestY != -1)
			{
				// Inner corner collision
				XYCollisionBehaviour(closestX, closestY, &*ent, &possitions, &collisionPosition, &collisionType, &collisionItem);
			}
			else if (closestY != -1)
			{
				YCollisionBehaviour(closestY, &*ent, &possitions, &collisionPosition, &collisionType, &collisionItem);
			}
			else if (closestX != -1)
			{
				XCollisionBehaviour(closestX, &*ent, &possitions, &collisionPosition, &collisionType, &collisionItem);
			}

			// TODO: Move all the entities that do not collide if this function is run again so that everything stays in sync
			if (WillCollide(&*ent, world, entities, worldEntities))
			{
				if (StackOverflowProtection < MAX_COLLISION_RECURSION)
				{
					++StackOverflowProtection;
					Collide(ent, world, entities, worldEntities);
				}
				else
				{
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
					// TODO: use colOff so that it doesn't have to calculated again.
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
						// TODO: use colOff so that it doesn't have to calculated again.

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
					// TODO: use colOff so that it doesn't have to calculated again.
					delete colOff;
					return true;
				}
			}
		}
	}

	return false;
}

// TODO: Make this return hit Coordinates or null
Coordinates* Physics::GetCollisionOffset(Entity* entity, Coordinates* xA, Coordinates* xB, double xStep, double yStep, double* minX, double* minY, double* maxX, double* maxY, bool* minXIsEnt, bool* minYIsEnt)
{
	Coordinates* entACo = entity->GetACoordinates();
	Coordinates* entBCo = entity->GetBCoordinates();
	Coordinates* entOff = entity->GetOffset();

	for (double x = 0, y = 0; (xStep < 0.0 ? x >= entOff->X - PRECISION : x <= entOff->X + PRECISION) && (yStep < 0.0 ? y >= entOff->Y - PRECISION : y <= entOff->Y + PRECISION); x += xStep, y += yStep)
	{
		// X with Offset					
		if (xA->X < entACo->X + x)
		{
			*minX = entACo->X + x;
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
			// TODO: Delete this
			return new Coordinates(x, y);
		}
	}

	return NULL;
}

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

// TODO: Refine this function with conditionary statements
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

void Physics::XCollisionBehaviour(int xIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem)
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[xIndex] == JELLY)
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			ent->SetOffset(entOff->X * -1 * PROJECTILE_BOUNCINESS, entOff->Y * PROJECTILE_BOUNCINESS);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLY;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[xIndex] == BADWORLD || (*collisionType)[xIndex] == WORLD)
		{
			ent->SetCoordinates((*possitions)[xIndex].X, (*possitions)[xIndex].Y);

			ent->SetRemove(true);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_NOTJELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
	}
	else // Type is player
	{

		if ((*collisionType)[xIndex] == BADWORLD)
		{
			ent->SetOffset(0.0, 0.0);
			ent->SetCoordinates(Spawn.X, Spawn.Y);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_BADWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[xIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[xIndex])->SetRemove(true);
			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_COIN;
			al_emit_user_event(&UserEventSource, &e, 0);
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_JELLY;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
	}
}

void Physics::YCollisionBehaviour(int yIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem)
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[yIndex] == JELLY)
		{
			// Calculate the offset at point of impact
			entOff->Y = entOff->Y - GRAVITY + ((*possitions)[yIndex].Y - entACo->Y) / entOff->Y * GRAVITY;
			ent->SetCoordinates((*possitions)[yIndex].X, (*possitions)[yIndex].Y);

			ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS, entOff->Y * -1 * PROJECTILE_BOUNCINESS);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLY;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[yIndex] == BADWORLD || (*collisionType)[yIndex] == WORLD)
		{
			ent->SetRemove(true);
			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_NOTJELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_BADWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[yIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[yIndex])->SetRemove(true);
			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_COIN;
			al_emit_user_event(&UserEventSource, &e, 0);
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_JELLY;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
	}
}

void Physics::XYCollisionBehaviour(int xIndex, int yIndex, Entity* ent, vector<Coordinates>* possitions, vector<CollPos>* collisionPosition, vector<ItemType>* collisionType, vector<void*>* collisionItem)
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_NOTJELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[xIndex] == JELLYWORLD)
		{
			ent->SetOffset(0.0, 0.0);
			ent->SetHit(true);

			WorldBlock* wor = ((WorldBlock*)(*collisionItem)[xIndex]);

			if ((*collisionPosition)[yIndex] == RX)
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[yIndex] == JELLYWORLD)
		{
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLYWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		else if ((*collisionType)[yIndex] == JELLY || (*collisionType)[xIndex] == JELLY)
		{
			// TODO: Wat is deze g, was met jou
			ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS * -1, entOff->Y * PROJECTILE_BOUNCINESS * -1);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = JELLY_JELLY;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
	}
	else if (ent->getType() == PLAYER)
	{

		if ((*collisionType)[xIndex] == BADWORLD || (*collisionType)[yIndex] == BADWORLD)
		{
			ent->SetOffset(0.0, 0.0);
			ent->SetCoordinates(Spawn.X, Spawn.Y);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_BADWORLD;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
		if ((*collisionType)[xIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[xIndex])->SetRemove(true);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_COIN;
			al_emit_user_event(&UserEventSource, &e, 0);

			YCollisionBehaviour(yIndex, ent, possitions, collisionPosition, collisionType, collisionItem);
		}
		else if ((*collisionType)[yIndex] == COIN)
		{
			((WorldEntity*)(*collisionItem)[yIndex])->SetRemove(true);

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_COIN;
			al_emit_user_event(&UserEventSource, &e, 0);

			XCollisionBehaviour(xIndex, ent, possitions, collisionPosition, collisionType, collisionItem);
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

			ALLEGRO_EVENT e;
			e.type = 555;
			e.user.data1 = PLAYER_JELLY;
			al_emit_user_event(&UserEventSource, &e, 0);
		}
	}
}
