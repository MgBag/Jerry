#include "Physics.h"

// TODO: Memory leaks mang
// TODO: Check logics mang, flows and flaws
// TODO: Stop deleteing entOff
// TODO: Meak air collision, might be easy?

void Physics::ApplyPhysics(list<Entity>* entities, list<WorldBlock>* world)
{
	ActiveParticles = -1;

	entities->begin()->SetIsAirBorn(true);

	for (list<Entity>::iterator ent = entities->begin(); ent != entities->end(); ++ent)
	{
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
		Collide(&(*ent), world, entities);
		ent->MoveToOffset();
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

void Physics::Collide(Entity* ent, list<WorldBlock>* world, list<Entity>* entities)
{
	Coordinates* entOff = ent->GetOffset();
	
	if (!ent->GetHit() && (entOff->X != 0.0 || entOff->Y != 0.0))
	{
		Coordinates* entACo = ent->GetACoordinates();
		Coordinates* entBCo = ent->GetBCoordinates();
		double entVel = sqrt(entOff->X * entOff->X + entOff->Y * entOff->Y);
		vector<Coordinates*> collisions;
		vector<Coordinates> possitions;
		vector<int> collisionPosition;
		vector<int> collisionType;
		vector<void*> collisionItem;


		// To make sure that a step isn't 1.0 if the offset is equal to teh velocity
		double xStep = (entOff->Y == 0 ? (entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0))) : entOff->X) / ceil(entVel);
		double yStep = (entOff->X == 0 ? (entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0))) : entOff->Y) / ceil(entVel);

		for (list<Entity>::iterator jel = entities->begin(); jel != entities->end(); ++jel)
		{
			if (ent == &*jel)
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

					Coordinates* colOff = GetCollisionOffset(ent, jelACo, jelBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

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
						else if (maxX > minX && !(maxY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
						{
							if (minYIsEnt)
							{
								possitions.push_back(Coordinates(entACo->X + colOff->X, jelBCo->Y));
								collisionPosition.push_back(DY);
							}
							else
							{
								possitions.push_back(Coordinates(entACo->X + colOff->X, jelACo->Y - ent->GetHeight()));
								collisionPosition.push_back(UY);
							}
						}
						else
						{
							cout << "bad mmkey\n";
						}

						// TODO: Fix this temp work around
						collisionItem.push_back((void*)&*jel);
						collisions.push_back(colOff);
						collisionType.push_back(JELLY);

						//delete colOff;
					}
				}
			}
		}

		for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
		{
			// TODO: Add might collide
			if (AreInRange(entOff, entACo, entBCo, wor->GetA(), wor->GetB()))
			{
				Coordinates* worACo = wor->GetA();
				Coordinates* worBCo = wor->GetB();

				if (AreColliding(entACo, entBCo, worACo, worBCo))
				{
					cout << "Well, there is your problem!\n";
				}


				bool minXIsEnt = false, minYIsEnt = false;
				double minX, minY, maxX, maxY;

				Coordinates* colOff = GetCollisionOffset(ent, worACo, worBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

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
					else if (maxX > minX && !(maxY - (minYIsEnt ? 0.0 : (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION)) > minY - (minYIsEnt ? (yStep < 0.0 ? yStep - PRECISION : yStep + PRECISION) : 0.0)))
					{
						if (minYIsEnt)
						{
							possitions.push_back(Coordinates(entACo->X + colOff->X, worBCo->Y));
							collisionPosition.push_back(DY);
						}
						else
						{
							possitions.push_back(Coordinates(entACo->X + colOff->X, worACo->Y - ent->GetHeight()));
							collisionPosition.push_back(UY);
						}
					}
					else
					{
						cout << "This is booshit";
					}

					collisionItem.push_back((void*)&*wor);
					collisions.push_back(colOff);
					collisionType.push_back(WORLD);
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

			if (closestX != -1 && closestY != -1)
			{
				if (entBCo->Y == (collisionType[closestX] == WORLD ? ((WorldBlock*)collisionItem[closestX])->GetA()->Y : ((Entity*)collisionItem[closestX])->GetACoordinates()->Y) ||
					entACo->Y == (collisionType[closestX] == WORLD ? ((WorldBlock*)collisionItem[closestX])->GetB()->Y : ((Entity*)collisionItem[closestX])->GetBCoordinates()->Y))
				{
					// X collision while the Y of the entity is the same as the item that will be collided with thus colliding with Y
					
					entOff->Y = entOff->Y - GRAVITY + (possitions[closestY].Y - entACo->Y) / entOff->Y * GRAVITY;
					ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);

					if (ent->getType() == PROJECTILE)
					{
						if (collisionType[closestY] == WORLD)
						{
							ent->SetHit(true);
							ent->SetOffset(0.0, 0.0);

							ent->SetWidth(ent->GetWidth() * 4);
							ent->SetHeight(ent->GetHeight() / 2);

							ent->MoveToOffset(ent->GetWidth() / 2 * -1, 0.0);
							if (collisionPosition[closestY] == UY) ent->MoveToOffset(0.0, ent->GetHeight());
						}
						else
						{
							ent->SetOffset(entOff->X * -1 * PROJECTILE_BOUNCINESS, entOff->Y * PROJECTILE_BOUNCINESS);
						}
					}
					else
					{
						if (collisionType[closestY] == WORLD || ent->GetIsCrouching())
						{
							ent->SetOffset(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0);
							ent->SetLastImpactType(WORLD);
							ent->SetIsAirBorn(false);
						}
						else
						{
							if (collisionType[closestY] == UY)
							{
								ent->SetOffset(entOff->X, entOff->Y * -1);
								ent->SetLastImpactType(JELLY);
							}
							else if (ent->GetLastImpactType() == WORLD)
							{
								if (ent->GetPreviousImpactHeight() < possitions[closestY].Y)
								{
									ent->SetOffset(entOff->X, entOff->Y * -1 - 0.5);
								}
								else
								{
									ent->SetOffset(entOff->X, PLAYER_BOUNCE_OFFSET * -1);
								}

								ent->SetPreviousImpactHeight(possitions[closestY].Y);
								ent->SetLastImpactType(JELLY);
							}
							else
							{
								if (ent->GetPreviousImpactHeight() > possitions[closestY].Y)
								{
									ent->SetOffset(entOff->X, PLAYER_BOUNCE_OFFSET * -1);
									ent->SetPreviousImpactHeight(possitions[closestY].Y);
									ent->SetLastImpactType(JELLY);
								}
								else
								{
									ent->SetOffset(entOff->X, entOff->Y * -1);
									ent->SetLastImpactType(JELLY);
								}
							}
						}
					}
				}
				else if (entBCo->X == (collisionType[closestY] == WORLD ? ((WorldBlock*)collisionItem[closestY])->GetA()->X : ((Entity*)collisionItem[closestY])->GetACoordinates()->X) ||
					entACo->X == (collisionType[closestY] == WORLD ? ((WorldBlock*)collisionItem[closestY])->GetB()->X : ((Entity*)collisionItem[closestY])->GetBCoordinates()->X))
				{
					// Y collision while the X of the entity is the same as the item that will be collided with thus colliding with X

					ent->SetCoordinates(possitions[closestX].X, possitions[closestX].Y);

					if (ent->getType() == PROJECTILE)
					{
						if (collisionType[closestX] == WORLD)
						{
							ent->SetOffset(0.0, 0.0);
							ent->SetHit(true);

							ent->SetHeight(ent->GetHeight() * 4);
							ent->SetWidth(ent->GetWidth() / 2);

							ent->MoveToOffset(0.0, ent->GetHeight() / 2 * -1);
							if (collisionPosition[closestX] == RX) ent->MoveToOffset(ent->GetWidth(), 0.0);
						}
						else
						{
							ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS, entOff->Y * PROJECTILE_BOUNCINESS * -1);
						}
					}
					else
					{
						if (collisionType[closestX] == WORLD || ent->GetIsCrouching())
						{
							ent->SetOffset(0.0, entOff->Y < FRICTION_STOP && entOff->Y > FRICTION_STOP * -1 ? 0.0 : entOff->Y * FRICTION);
							ent->SetLastImpactType(WORLD);
						}
						else
						{
							ent->SetOffset(PLAYER_BOUNCE_OFFSET / (collisionPosition[closestX] == RX ? -0.9 : 0.9), PLAYER_BOUNCE_OFFSET / -1.5);
							ent->SetLastImpactType(JELLY);
						}
					}
				}
				else
				{
					// Inner corner collision
					entOff->Y = entOff->Y - GRAVITY + (possitions[closestY].Y - entACo->Y) / entOff->Y * GRAVITY;
					ent->SetCoordinates(possitions[closestX].X, possitions[closestY].Y);

					if (ent->getType() == PROJECTILE)
					{
						if (collisionType[closestX] == WORLD)
						{
							ent->SetOffset(0.0, 0.0);
							ent->SetHit(true);

							ent->SetHeight(ent->GetHeight() * 4);
							ent->SetWidth(ent->GetWidth() / 2);

							ent->MoveToOffset(0.0, ent->GetHeight() / 2 * -1);
							if (collisionPosition[closestX] == RX) ent->MoveToOffset(ent->GetWidth(), 0.0);
						}
						else if (collisionType[closestY] == WORLD)
						{
							ent->SetHit(true);
							ent->SetOffset(0.0, 0.0);

							ent->SetWidth(ent->GetWidth() * 4);
							ent->SetHeight(ent->GetHeight() / 2);

							ent->MoveToOffset(ent->GetWidth() / 2 * -1, 0.0);
							if (collisionPosition[closestY] == UY) ent->MoveToOffset(0.0, ent->GetHeight());
						}
						else
						{
							ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS * -1, entOff->Y * PROJECTILE_BOUNCINESS * -1);
						}
					}
					else if (ent->getType() == PLAYER)
					{
						if ((collisionType[closestX] == WORLD && collisionType[closestY] == WORLD) || ent->GetIsCrouching())
						{
							ent->SetOffset(0.0, 0.0);
							ent->SetIsAirBorn(false);
						}
						else if (collisionType[closestY] == WORLD)
						{
							ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS * -1, 0.0);
							ent->SetIsAirBorn(false);
						}
						else if (collisionType[closestX] == WORLD)
						{
							ent->SetOffset(0.0, entOff->Y * PROJECTILE_BOUNCINESS * -1);
						}
						else
						{
							ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS * -1, entOff->Y * PROJECTILE_BOUNCINESS * -1);
						}
					}
				}
			}
			else if (closestY != -1)
			{
				// Calculate the offset at point of impact
				entOff->Y = entOff->Y - GRAVITY + (possitions[closestY].Y - entACo->Y) / entOff->Y * GRAVITY;
				ent->SetCoordinates(possitions[closestY].X, possitions[closestY].Y);
				

				if (ent->getType() == PROJECTILE)
				{
					if (collisionType[closestY] == WORLD)
					{
						ent->SetOffset(0.0, 0.0);
						ent->SetHit(true);

						ent->SetWidth(ent->GetWidth() * 4);
						ent->SetHeight(ent->GetHeight() / 2);

						ent->MoveToOffset(ent->GetWidth() / 2 * -1, 0.0);
						if (collisionPosition[closestY] == UY) ent->MoveToOffset(0.0, ent->GetHeight());
					}
					else
					{
						ent->SetOffset(entOff->X * PROJECTILE_BOUNCINESS, entOff->Y * -1 * PROJECTILE_BOUNCINESS);
					}
				}
				else // Type is player
				{
					if (collisionType[closestY] == WORLD || ent->GetIsCrouching())
					{
						ent->SetOffset(entOff->X < FRICTION_STOP && entOff->X > FRICTION_STOP * -1 ? 0.0 : entOff->X * FRICTION, 0.0);
						
						if (collisionPosition[closestY] == UY)
						{
							ent->SetLastImpactType(WORLD);
							ent->SetPreviousImpactHeight(possitions[closestY].Y);
						}

						ent->SetIsAirBorn(false);
					}
					else // Jelly collision
					{
						if (collisionPosition[closestY] == DY)
						{
							ent->SetOffset(entOff->X, entOff->Y * -1);
							ent->SetLastImpactType(JELLY);
						}
						else if (ent->GetLastImpactType() == WORLD)
						{
							if (ent->GetPreviousImpactHeight() < possitions[closestY].Y)
							{
								ent->SetOffset(entOff->X, entOff->Y * -1 - 0.5);
							}
							else
							{
								ent->SetOffset(entOff->X, PLAYER_BOUNCE_OFFSET * -1);
							}

							ent->SetPreviousImpactHeight(possitions[closestY].Y);
							ent->SetLastImpactType(JELLY);
						}
						else
						{
							if (ent->GetPreviousImpactHeight() >= possitions[closestY].Y)
							{
								ent->SetOffset(entOff->X, PLAYER_BOUNCE_OFFSET * -1);
								ent->SetPreviousImpactHeight(possitions[closestY].Y);
								ent->SetLastImpactType(JELLY);
							} 
							else
							{
								ent->SetOffset(entOff->X, entOff->Y * -1);
								ent->SetLastImpactType(JELLY);
							}
						}
					}
				}
			}
			else if (closestX != -1)
			{
				ent->SetCoordinates(possitions[closestX].X, possitions[closestX].Y);

				if (ent->getType() == PROJECTILE)
				{
					if (collisionType[closestX] == WORLD)
					{
						ent->SetOffset(0.0, 0.0);
						ent->SetHit(true);

						ent->SetHeight(ent->GetHeight() * 4);
						ent->SetWidth(ent->GetWidth() / 2);

						ent->MoveToOffset(0.0, ent->GetHeight() / 2 * -1);
						if (collisionPosition[closestX] == RX) ent->MoveToOffset(ent->GetWidth(), 0.0);
					}
					else
					{
						ent->SetOffset(entOff->X * -1 * PROJECTILE_BOUNCINESS, entOff->Y * PROJECTILE_BOUNCINESS);
					}
				}
				else // Type is player
				{
					if (collisionType[closestX] == WORLD || ent->GetIsCrouching())
					{
						ent->SetOffset(0.0, entOff->Y < FRICTION_STOP && entOff->Y > FRICTION_STOP * -1 ? 0.0 : entOff->Y * FRICTION);
						ent->SetLastImpactType(WORLD);
					}
					else
					{
						ent->SetOffset(PLAYER_BOUNCE_OFFSET / (collisionPosition[closestX] == RX ? -0.9 : 0.9 ), PLAYER_BOUNCE_OFFSET / -1.5);
						ent->SetLastImpactType(JELLY);
					}
				}
			}

			// TODO: Move all the entities that do not collide if this function is run again so that everything stays in sync
			if (WillCollide(ent, world, entities))
			{
				Collide(ent, world, entities);
			}
		}
	}
}

bool Physics::WillCollide(Entity* entity, list<WorldBlock>* world, list<Entity>* entities)
{
	if (!entity->GetHit())
	{
		Coordinates* entACo = entity->GetACoordinates();
		Coordinates* entBCo = entity->GetBCoordinates();
		Coordinates* entOff = entity->GetOffset();

		double entVel = sqrt(entOff->X * entOff->X + entOff->Y * entOff->Y);

		double xStep = (entOff->Y == 0 ? (entOff->X / (entOff->X * (entOff->X < 0.0 ? -10.0 : 10.0))) : entOff->X) / ceil(entVel);
		double yStep = (entOff->X == 0 ? (entOff->Y / (entOff->Y * (entOff->Y < 0.0 ? -10.0 : 10.0))) : entOff->Y) / ceil(entVel);

		// Check if the speed is 0 
		for (list<WorldBlock>::iterator wor = world->begin(); wor != world->end(); ++wor)
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

		//TODO: Add the isHit conditions and shit
		for (list<Entity>::iterator jel = entities->begin(); jel != entities->end(); ++jel)
		{
			Coordinates* jelACo = jel->GetACoordinates();
			Coordinates* jelBCo = jel->GetBCoordinates();


			if (entity != &*jel && jel->GetHit() && !AreColliding(entACo, entBCo, jelACo, jelBCo) && AreInRange(entOff, entACo, entBCo, jelACo, jelBCo))
			{
				bool minXIsEnt = false, minYIsEnt = false;
				double minX, minY, maxX, maxY;

				Coordinates* colOff = GetCollisionOffset(entity, jelACo, jelBCo, xStep, yStep, &minX, &minY, &maxX, &maxY, &minXIsEnt, &minYIsEnt);

				if (colOff != NULL)
				{
					jel->SetColor(al_map_rgb(220, 20, 220));
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
